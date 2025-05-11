#include "core/horizon/services/nvdrv/ioctl/nvmap.hpp"

#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

DEFINE_IOCTL_TABLE(NvMap, DEFINE_IOCTL_TABLE_ENTRY(NvMap, 0x01, 0x01, Create, 0x03,
                                                   FromId, 0x04, Alloc, 0x05,
                                                   Free, 0x0E, GetId))

NvResult NvMap::Create(u32 size, handle_id_t* out_handle_id) {
    *out_handle_id = HW::TegraX1::GPU::GPU::GetInstance().CreateMap(size);
    return NvResult::Success;
}

NvResult NvMap::FromId(u32 id, handle_id_t* out_handle_id) {
    *out_handle_id =
        HW::TegraX1::GPU::GPU::GetInstance().GetMapHandleId(id);
    return NvResult::Success;
}

NvResult NvMap::Alloc(handle_id_t handle_id, u32 heap_mask, u32 flags, InOutSingle<u32> inout_alignment, aligned<u8, 8> kind, gpu_vaddr_t addr) {
    // TODO: flags wtf
    HW::TegraX1::GPU::GPU::GetInstance().AllocateMap(handle_id, addr,
                                                     flags == 1);
    inout_alignment = HW::TegraX1::GPU::PAGE_SIZE; // TODO: correct?
    return NvResult::Success;
}

NvResult NvMap::Free(aligned<handle_id_t, 8> handle_id, gpu_vaddr_t* out_addr, u64* out_size, u32* out_flags) {
    auto& gpu = HW::TegraX1::GPU::GPU::GetInstance();
    auto map = gpu.GetMap(handle_id);
    gpu.FreeMap(handle_id);

    *out_addr = map.addr;
    *out_size = map.size;
    *out_flags = map.write ? 1 : 0; // TODO: correct?
    return NvResult::Success;
}

NvResult NvMap::GetId(u32* out_id, handle_id_t handle_id) {
    *out_id = HW::TegraX1::GPU::GPU::GetInstance().GetMapId(handle_id);
    return NvResult::Success;
}

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
