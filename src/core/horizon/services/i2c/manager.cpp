#include "core/horizon/services/i2c/manager.hpp"

#include "core/horizon/services/i2c/session.hpp"

namespace hydra::horizon::services::i2c {

DEFINE_SERVICE_COMMAND_TABLE(IManager, 1, OpenSession, 2, HasDevice)

result_t IManager::OpenSession(RequestContext* ctx, I2cDevice device) {
    AddService(*ctx, new ISession(device));
    return RESULT_SUCCESS;
}

result_t IManager::HasDevice(I2cDevice device, bool* out_has) {
    LOG_FUNC_WITH_ARGS_STUBBED(Services, "device: {}", device);

    // HACK
    *out_has = true;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::i2c
