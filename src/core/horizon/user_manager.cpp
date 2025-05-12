#include "core/horizon/user_manager.hpp"

namespace hydra::horizon {

SINGLETON_DEFINE_GET_INSTANCE(UserManager, Horizon)

UserManager::UserManager() {
    SINGLETON_SET_INSTANCE(UserManager, Horizon);
}

UserManager::~UserManager() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::horizon
