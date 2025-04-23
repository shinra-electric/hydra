#include "core/horizon/services/spl/general_interface.hpp"

namespace Hydra::Horizon::Services::Spl {

namespace {

enum class ConfigItem : u32 {
    DisableProgramVerification = 1,
    DramId,
    SecurityEngineInterruptNumber,
    FuseVersion,
    HardwareType,
    HardwareState,
    IsRecoveryBoot,
    DeviceId,
    BootReason,
    MemoryMode,
    IsDevelopmentFunctionEnabled,
    KernelConfiguration,
    IsChargerHiZModeEnabled,
    RetailInteractiveDisplayState,
    RegulatorType,
    DeviceUniqueKeyGeneration,
    Package2Hash,
};

}

} // namespace Hydra::Horizon::Services::Spl

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::Services::Spl::ConfigItem, DisableProgramVerification,
    "disable program verification", DramId, "DRAM ID",
    SecurityEngineInterruptNumber, "security engine interrupt number",
    FuseVersion, "fuse version", HardwareType, "hardware type", HardwareState,
    "hardware state", IsRecoveryBoot, "is recovery boot", DeviceId, "device ID",
    BootReason, "boot reason", MemoryMode, "memory mode",
    IsDevelopmentFunctionEnabled, "is development function enabled",
    KernelConfiguration, "kernel configuration", IsChargerHiZModeEnabled,
    "is charger HiZ mode enabled", RetailInteractiveDisplayState,
    "retail interactive display state", RegulatorType, "regulator type",
    DeviceUniqueKeyGeneration, "device unique key generation", Package2Hash,
    "package 2 hash")

namespace Hydra::Horizon::Services::Spl {

DEFINE_SERVICE_COMMAND_TABLE(IGeneralInterface, 0, GetConfig)

void IGeneralInterface::GetConfig(REQUEST_COMMAND_PARAMS) {
    const auto item = readers.reader.Read<ConfigItem>();
    LOG_DEBUG(HorizonServices, "Config item: {}", item);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u64>(0);
}

} // namespace Hydra::Horizon::Services::Spl
