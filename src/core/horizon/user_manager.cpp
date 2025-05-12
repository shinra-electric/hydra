#include "core/horizon/user_manager.hpp"

#define DEFAULT_USER_NAME "Hydra user"

namespace hydra::horizon {

namespace {

constexpr uuid_t USER_ID_START = 0x80000000;
constexpr uuid_t INVALID_USER_ID = USER_ID_START;
constexpr uuid_t FIRST_USER_ID = USER_ID_START + 1;

}

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
    auto& user_pair = users[user_id];
    user_pair.first = {
        .name = DEFAULT_USER_NAME,
    };
    user_pair.second = true;

    return user_id;
}

void UserManager::Flush() {
    for (auto& [user_id, user_pair] : users)
        Serialize(user_id);
}

void UserManager::Serialize(uuid_t user_id) {
    auto& user_pair = users[user_id];
    if (!user_pair.second)
        return;

    LOG_FUNC_NOT_IMPLEMENTED(Horizon);

    user_pair.second = false;
}

void UserManager::Deserialize(uuid_t user_id) {
    auto& user_pair = users[user_id];
    if (user_pair.second)
        LOG_WARN(Horizon, "Overwriting user 0x{:08x}", user_id);

    LOG_FUNC_NOT_IMPLEMENTED(Horizon);

    user_pair.second = false;
}

} // namespace hydra::horizon
