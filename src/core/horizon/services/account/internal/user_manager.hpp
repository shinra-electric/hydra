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
    void LoadAvatarBackgroundImage(const User& user, u8*& out_data,
                                   usize& out_size) {
        LoadAvatarImage(avatar_bgs[user.avatar.bg_path], out_data, out_size);
    }
    void LoadAvatarCharacterImage(const User& user, u8*& out_data,
                                  usize& out_size) {
        LoadAvatarImage(avatar_bgs[user.avatar.char_path], out_data, out_size);
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
    void LoadAvatarImage(filesystem::FileBase* file, u8*& out_data,
                         usize& out_size);
};

} // namespace hydra::horizon::services::account::internal
