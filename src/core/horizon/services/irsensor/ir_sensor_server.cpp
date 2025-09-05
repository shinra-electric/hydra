#include "core/horizon/services/irsensor/ir_sensor_server.hpp"

#include "core/horizon/kernel/process.hpp"
#include "core/horizon/kernel/shared_memory.hpp"

namespace hydra::horizon::services::irsensor {

DEFINE_SERVICE_COMMAND_TABLE(IIrSensorServer, 303, DeactivateIrsensor, 304,
                             GetIrsensorSharedMemoryHandle, 311,
                             GetNpadIrCameraHandle, 319,
                             ActivateIrsensorWithFunctionLevel)

IIrSensorServer::IIrSensorServer()
    : shared_mem{new kernel::SharedMemory(0x8000)} {}

result_t IIrSensorServer::GetIrsensorSharedMemoryHandle(
    kernel::Process* process, u64 aruid,
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = process->AddHandle(shared_mem);
    return RESULT_SUCCESS;
}

result_t IIrSensorServer::GetNpadIrCameraHandle(
    ::hydra::horizon::hid::NpadIdType npad_id, IrCameraHandle* out_handle) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Npad ID: {}", npad_id);

    // HACK
    *out_handle = {
        .player_number = 0,
        .device_type = 0,
    };
    return RESULT_SUCCESS;
}

result_t IIrSensorServer::ActivateIrsensorWithFunctionLevel(
    PackedFunctionLevel function_level, u64 aruid) {
    LOG_FUNC_STUBBED(Services);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::irsensor
