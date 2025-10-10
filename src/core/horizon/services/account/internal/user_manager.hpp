#pragma once

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/services/account/internal/user.hpp"

namespace hydra::horizon::services::account::internal {

class UserManager {
  public:
    UserManager();

    uuid_t Create();

    void Flush();

    std::vector<uuid_t> GetUserIDs() const {
        std::vector<uuid_t> ids;
        ids.reserve(users.size());
        for (const auto& [id, _] : users)
            ids.push_back(id);

        return ids;
    }

    User& Get(uuid_t user_id) { return GetPair(user_id).first; }
    bool Exists(uuid_t user_id) const { return users.contains(user_id); }
    usize GetCount() const { return users.size(); }

    // Avatar
    void LoadSystemAvatars();
    usize GetAvatarImageSize(uuid_t user_id);
    void LoadAvatarBackgroundImage(uuid_t user_id, u8*& out_data) {
        LoadAvatarImage(avatar_bgs.at(Get(user_id).avatar.bg_path), out_data);
    }
    void LoadAvatarCharacterImage(uuid_t user_id, u8*& out_data) {
        LoadAvatarImage(avatar_chars.at(Get(user_id).avatar.char_path),
                        out_data);
    }

  private:
    std::map<uuid_t, std::pair<User, u64>> users;
    std::map<std::string, filesystem::FileBase*> avatar_bgs;
    std::map<std::string, filesystem::FileBase*> avatar_chars;

    // Helpers
    std::pair<User, u64>& GetPair(uuid_t user_id) {
        auto it = users.find(user_id);
        ASSERT(it != users.end(), Horizon, "Invalid user {:032x}", user_id);

        return it->second;
    }

    static std::string GetUserPath() {
        return fmt::format("{}/user", CONFIG_INSTANCE.GetAppDataPath());
    }

    void Serialize(uuid_t user_id);
    void Deserialize(uuid_t user_id);

    void LoadSystemAvatarSet(
        filesystem::Directory* dir,
        std::map<std::string, filesystem::FileBase*>& out_avatars);
    void LoadAvatarImage(filesystem::FileBase* file, u8*& out_data);
};

} // namespace hydra::horizon::services::account::internal
