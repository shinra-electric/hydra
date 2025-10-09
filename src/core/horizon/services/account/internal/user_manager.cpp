#include "core/horizon/services/account/internal/user_manager.hpp"

#include "core/horizon/filesystem/content_archive.hpp"
#include "core/horizon/filesystem/romfs.hpp"

#define DEFAULT_USER_NAME "Hydra user"

namespace hydra::horizon::services::account::internal {

namespace {

constexpr magic4_t HUSR_MAGIC = make_magic4('H', 'U', 'S', 'R');
constexpr u32 CURRENT_HUSR_VERSION = 2;

struct HusrHeader {
    magic4_t magic{HUSR_MAGIC};
    u32 version{CURRENT_HUSR_VERSION};
    u32 header_size{sizeof(HusrHeader)};
};

} // namespace

UserManager::UserManager() {
    // Create user directory
    std::filesystem::create_directories(GetUserPath());

    // Deserialize all users
    if (std::filesystem::exists(GetUserPath())) {
        for (const auto& dir_entry :
             std::filesystem::directory_iterator{GetUserPath()}) {
            auto filename = dir_entry.path().filename();

            auto extension = filename.extension().string();
            if (extension != ".husr")
                continue;

            auto user_id_str = filename.stem().string();
            if (user_id_str.size() != 32) {
                LOG_WARN(Horizon, "Invalid user ID {}", user_id_str);
                return;
            }

            u64 user_id_hi =
                std::stoull(user_id_str.substr(0, 16), nullptr, 16);
            u64 user_id_lo =
                std::stoull(user_id_str.substr(16, 16), nullptr, 16);
            uuid_t user_id = (uuid_t)user_id_lo | ((uuid_t)user_id_hi << 64);

            Deserialize(user_id);
        }
    }

    if (users.empty()) {
        auto user_id = Create();
        Serialize(user_id);
    }
}

uuid_t UserManager::Create() {
    // First, find an available ID
    uuid_t user_id = random128();
    while (user_id == 0x0 || users.contains(user_id))
        user_id = random128();

    // Create
    User new_user(DEFAULT_USER_NAME, {"10000001.szs", "00000001.szs"});
    users.insert({user_id, {new_user, 0}});

    return user_id;
}

void UserManager::Flush() {
    for (auto& [user_id, user_pair] : users)
        Serialize(user_id);
}

void UserManager::LoadSystemAvatars() {
    // NCA
    filesystem::FileBase* file;
    auto res = FILESYSTEM_INSTANCE.GetFile(
        fmt::format(FS_FIRMWARE_PATH "/{:016x}", 0x010000000000080a), file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Services, "Failed to get avatars file: {}", res);
        return;
    }

    filesystem::ContentArchive content_archive(file);

    // Data
    filesystem::FileBase* data_file;
    res = content_archive.GetFile("data", data_file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Services, "Failed to get avatars data: {}", res);
        return;
    }

    // RomFS
    filesystem::RomFS romfs(data_file);

    // Background
    filesystem::Directory* background_dir;
    res = romfs.GetDirectory("bg", background_dir);
    ASSERT(res == filesystem::FsResult::Success, Services,
           "Failed to get \"bg\" avatars directory: {}", res);
    LoadSystemAvatarSet(background_dir, avatar_bgs);

    // Characters
    filesystem::Directory* character_dir;
    res = romfs.GetDirectory("chara", character_dir);
    ASSERT(res == filesystem::FsResult::Success, Services,
           "Failed to get \"chara\" avatars directory: {}", res);
    LoadSystemAvatarSet(character_dir, avatar_chars);
}

void UserManager::Serialize(uuid_t user_id) {
    LOG_INFO(Horizon, "Serializing user with ID {:032x}", user_id);

    auto& user_pair = GetPair(user_id);
    const auto& user = user_pair.first;
    if (!user.EditedSince(user_pair.second))
        return;

    // Serialize
    std::ofstream ofs{fmt::format("{}/{:032x}.husr", GetUserPath(), user_id)};
    {
        StreamWriter writer(ofs);

        // Header
        HusrHeader header{};
        writer.Write(header);

        // Data
        writer.Write(user.base);
        writer.Write(user.data);
        writer.Write<u32>(user.avatar.bg_path.size());
        writer.WritePtr(user.avatar.bg_path.data(), user.avatar.bg_path.size());
        writer.Write<u32>(user.avatar.char_path.size());
        writer.WritePtr(user.avatar.char_path.data(),
                        user.avatar.char_path.size());
    }
    ofs.close();

    user_pair.second = get_absolute_time();
}

void UserManager::Deserialize(uuid_t user_id) {
    LOG_INFO(Horizon, "Deserializing user with ID {:032x}", user_id);

    auto it = users.find(user_id);
    if (it != users.end()) {
        auto& user_pair = it->second;
        if (!user_pair.first.EditedSince(user_pair.second))
            LOG_WARN(Horizon, "Overwriting user {:032x}", user_id);
    }

    // Deserialize
    std::ifstream ifs{fmt::format("{}/{:032x}.husr", GetUserPath(), user_id)};
    StreamReader reader(ifs);

    // Header
    const auto header = reader.Read<HusrHeader>();

    // Validate
    ASSERT(header.magic == HUSR_MAGIC, Horizon,
           "Invalid HUSR magic 0x{:08x} for user {:032x}", header.magic,
           user_id);
    if (header.version < 2) {
        LOG_WARN(Horizon, "Unsupported HUSR version {} for user {:032x}",
                 header.version, user_id);
        return;
    }
    ASSERT(header.version == CURRENT_HUSR_VERSION, Horizon,
           "Invalid HUSR version {} for user {:032x}", header.version, user_id);
    ASSERT(header.header_size == sizeof(HusrHeader), Horizon,
           "Invalid HUSR header size 0x{:x} for user {:032x}",
           header.header_size, user_id);

    // Data
    const auto base = reader.Read<ProfileBase>();
    const auto data = reader.Read<UserData>();
    Avatar avatar;
    {
        const auto size = reader.Read<u32>();
        avatar.bg_path.resize(size);
        reader.ReadPtr(avatar.bg_path.data(), size);
    }
    {
        const auto size = reader.Read<u32>();
        avatar.char_path.resize(size);
        reader.ReadPtr(avatar.char_path.data(), size);
    }

    User user(base, data, avatar);
    users.insert({user_id, {user, user.GetLastEditTimestamp()}});
}

void UserManager::LoadSystemAvatarSet(
    filesystem::Directory* dir,
    std::map<std::string, filesystem::FileBase*>& out_avatars) {
    for (const auto& [name, entry] : dir->GetEntries()) {
        // TODO
        LOG_INFO(Services, "Avatar: {}", name);
    }
}

void UserManager::LoadAvatarImage(filesystem::FileBase* file, u8*& out_data,
                                  usize& out_size) {
    // TODO
    LOG_FATAL(Services, "Not implemented");
}

} // namespace hydra::horizon::services::account::internal
