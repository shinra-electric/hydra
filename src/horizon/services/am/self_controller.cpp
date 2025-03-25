#include "horizon/services/am/self_controller.hpp"

#include "horizon/state_manager.hpp"

namespace Hydra::Horizon::Services::Am {

DEFINE_SERVICE_COMMAND_TABLE(ISelfController, 1, LockExit, 2, UnlockExit)

void ISelfController::LockExit(REQUEST_COMMAND_PARAMS) {
    StateManager::GetInstance().LockExit();
}

void ISelfController::UnlockExit(REQUEST_COMMAND_PARAMS) {
    StateManager::GetInstance().UnlockExit();
}

} // namespace Hydra::Horizon::Services::Am
