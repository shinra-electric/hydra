#include "core/horizon/state_manager.hpp"

namespace hydra::horizon {

SINGLETON_DEFINE_GET_INSTANCE(StateManager, Horizon)

StateManager::StateManager()
    : msg_event(new kernel::Event(kernel::EventFlags::None, "Message event")) {
    SINGLETON_SET_INSTANCE(StateManager, Horizon);
}

StateManager::~StateManager() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::horizon
