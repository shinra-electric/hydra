#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon {

struct User {
    std::string name;
    // TODO: more
};

class UserManager {
  public:
    static UserManager& GetInstance();

    UserManager();
    ~UserManager();

    uuid_t Create();

    void Flush();

    // Getters
    const User& Get(uuid_t user_id) const {
        auto it = users.find(user_id);
        ASSERT(it != users.end(), Horizon, "Invalid user 0x{:08x}", user_id);

        return it->second.first;
    }

    // Setters
    void Set(uuid_t user_id, const User& user) { users[user_id] = {user, true}; }

  private:
    std::map<uuid_t, std::pair<User, bool>> users;

    // Helpers
    void Serialize(uuid_t user_id);
    void Deserialize(uuid_t user_id);
};

} // namespace hydra::horizon
