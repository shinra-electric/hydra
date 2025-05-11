#include "core/horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"

#include "core/horizon/kernel/kernel.hpp"

namespace Hydra::Horizon::Services::Pl::SharedResource {

namespace {

constexpr usize SHARED_MEMORY_SIZE = 0x01100000;

} // namespace

DEFINE_SERVICE_COMMAND_TABLE(IPlatformSharedResourceManager, 0, RequestLoad, 1,
                             GetLoadState, 2, GetSize, 3,
                             GetSharedMemoryAddressOffset, 4,
                             GetSharedMemoryNativeHandle)

IPlatformSharedResourceManager::IPlatformSharedResourceManager() {
    shared_memory_handle_id = Kernel::Kernel::GetInstance().AddHandle(
        new Kernel::SharedMemory(SHARED_MEMORY_SIZE));
}

result_t IPlatformSharedResourceManager::RequestLoad(SharedFontType font_type) {
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetLoadState(SharedFontType font_type,
                                                      LoadState* out_state) {
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_state = LoadState::Loaded;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSize(SharedFontType font_type,
                                                 u32* out_size) {
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_size = SHARED_MEMORY_SIZE;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedMemoryAddressOffset(
    SharedFontType font_type, u32* out_address_offset) {
    LOG_DEBUG(HorizonServices, "Font type: {}", font_type);

    LOG_FUNC_STUBBED(HorizonServices);

    // HACK
    *out_address_offset = 0;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedMemoryNativeHandle(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = shared_memory_handle_id;
    return RESULT_SUCCESS;
}

result_t IPlatformSharedResourceManager::GetSharedFontInOrderOfPriority() {
    LOG_FUNC_NOT_IMPLEMENTED(HorizonServices);
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Pl::SharedResource
