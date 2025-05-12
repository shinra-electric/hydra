#include "core/horizon/state_manager.hpp"

namespace hydra::horizon {

SINGLETON_DEFINE_GET_INSTANCE(StateManager, Horizon, "State manager")

StateManager::StateManager() : msg_event(new kernel::Event()) {
    SINGLETON_SET_INSTANCE(Horizon, "State manager");
}

StateManager::~StateManager() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::horizon
