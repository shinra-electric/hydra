#include "core/horizon/user_manager.hpp"

#define DEFAULT_USER_NAME "Hydra user"

namespace hydra::horizon {

SINGLETON_DEFINE_GET_INSTANCE(UserManager, Horizon)

UserManager::UserManager() {
    SINGLETON_SET_INSTANCE(UserManager, Horizon);

    // TODO: deserialize all users

    if (users.empty()) {
        auto user_id = Create();
        Serialize(user_id);
    }
}

UserManager::~UserManager() { SINGLETON_UNSET_INSTANCE(); }

uuid_t UserManager::Create() {
    // First, find an available ID
    uuid_t user_id = FIRST_USER_ID;
    while (true) {
        if (!users.contains(user_id))
            break;

        user_id++;
    }

    // Create
    User new_user(DEFAULT_USER_NAME);
    users.insert({user_id, {new_user, new_user.last_edit_timestamp}});

    return user_id;
}

void UserManager::Flush() {
    for (auto& [user_id, user_pair] : users)
        Serialize(user_id);
}

void UserManager::Serialize(uuid_t user_id) {
    auto& user_pair = GetPair(user_id);
    if (!user_pair.first.EditedSince(user_pair.second))
        return;

    // TODO
    LOG_FUNC_NOT_IMPLEMENTED(Horizon);

    user_pair.second = get_absolute_time();
}

void UserManager::Deserialize(uuid_t user_id) {
    {
        auto it = users.find(user_id);
        if (it != users.end()) {
            auto& user_pair = it->second;
            if (!user_pair.first.EditedSince(user_pair.second))
                LOG_WARN(Horizon, "Overwriting user 0x{:08x}", user_id);
        }
    }

    // TODO
    std::string nickname = "INVALID";
    LOG_FUNC_NOT_IMPLEMENTED(Horizon);

    User new_user(nickname);
    users.insert({user_id, {new_user, new_user.last_edit_timestamp}});
}

} // namespace hydra::horizon
