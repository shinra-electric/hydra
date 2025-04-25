#include "core/horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"

#include "core/horizon/kernel/kernel.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

namespace {

constexpr usize SHARED_MEMORY_SIZE = 0x01100000;

enum class SharedFontType : u32 {
    JapanUsEurope,
    ChineseSimplified,
    ExtendedChineseSimplified,
    ChineseTraditional,
    Korean,
    NintendoExtended,
};

enum class LoadState : u32 {
    Loading,
    Loaded,
};

} // namespace

} // namespace Hydra::Horizon::Services::Pl::SharedResource

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::Services::Pl::SharedResource::SharedFontType, JapanUsEurope,
    "Japan/US/Europe", ChineseSimplified, "Chinese Simplified",
    ExtendedChineseSimplified, "Extended Chinese Simplified",
    ChineseTraditional, "Chinese Traditional", Korean, "Korean",
    NintendoExtended, "Nintendo Extended")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::Services::Pl::SharedResource::LoadState,
                       Loading, "Loading", Loaded, "Loaded")

namespace Hydra::Horizon::Services::Pl::SharedResource {

DEFINE_SERVICE_COMMAND_TABLE(IPlatformSharedResourceManager, 0, RequestLoad, 1,
                             GetLoadState, 2, GetSize, 3,
                             GetSharedMemoryAddressOffset, 4,
                             GetSharedMemoryNativeHandle)

IPlatformSharedResourceManager::IPlatformSharedResourceManager() {
    shared_memory_handle_id = Kernel::Kernel::GetInstance().AddHandle(
        new Kernel::SharedMemory(SHARED_MEMORY_SIZE));
}

void IPlatformSharedResourceManager::RequestLoad(REQUEST_COMMAND_PARAMS) {
    const auto font_type = readers.reader.Read<SharedFontType>();
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);
}

void IPlatformSharedResourceManager::GetLoadState(REQUEST_COMMAND_PARAMS) {
    const auto font_type = readers.reader.Read<SharedFontType>();
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write(LoadState::Loaded);
}

void IPlatformSharedResourceManager::GetSize(REQUEST_COMMAND_PARAMS) {
    const auto font_type = readers.reader.Read<SharedFontType>();
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write((u32)SHARED_MEMORY_SIZE);
}

void IPlatformSharedResourceManager::GetSharedMemoryAddressOffset(
    REQUEST_COMMAND_PARAMS) {
    const auto font_type = readers.reader.Read<SharedFontType>();
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    writers.writer.Write<u32>(0);
}

void IPlatformSharedResourceManager::GetSharedMemoryNativeHandle(
    REQUEST_COMMAND_PARAMS) {
    writers.copy_handles_writer.Write(shared_memory_handle_id);
}

void IPlatformSharedResourceManager::GetSharedFontInOrderOfPriority(
    REQUEST_COMMAND_PARAMS) {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
}

} // namespace Hydra::Horizon::Services::Pl::SharedResource
