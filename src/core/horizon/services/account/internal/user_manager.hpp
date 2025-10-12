#pragma once

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/services/account/internal/user.hpp"

namespace hydra::horizon::services::account::internal {

class UserManager {
  public:
    UserManager();
    ~UserManager() { Flush(); }

    void Flush();

    uuid_t CreateUser();

    std::vector<uuid_t> GetUserIDs() const {
        std::vector<uuid_t> ids;
        ids.reserve(users.size());
        for (const auto& [id, _] : users)
            ids.push_back(id);

        return ids;
    }

    usize GetUserCount() const { return users.size(); }
    User& GetUser(uuid_t user_id) { return GetPair(user_id).first; }
    bool UserExists(uuid_t user_id) const { return users.contains(user_id); }

    // Avatar
    void LoadSystemAvatars(filesystem::Filesystem& fs);
    void LoadAvatarImage(uuid_t user_id, std::vector<u8>& out_data,
                         usize& out_dimensions);
    void LoadAvatarImageAsJpeg(uuid_t user_id, std::vector<u8>& out_data);
    // TODO: isn't there a better way?
    usize GetAvatarImageSize(uuid_t user_id) {
        std::vector<u8> data;
        usize dimensions;
        LoadAvatarImage(user_id, data, dimensions);
        return data.size();
    }
    usize GetAvatarImageAsJpegSize(uuid_t user_id) {
        std::vector<u8> data;
        LoadAvatarImageAsJpeg(user_id, data);
        return data.size();
    }

  private:
    std::map<uuid_t, std::pair<User, u64>> users;
    std::map<std::string, filesystem::FileBase*> avatar_images;

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
};

} // namespace hydra::horizon::services::account::internal
