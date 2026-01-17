#include "core/hw/tegra_x1/gpu/pfifo.hpp"

#include "core/hw/tegra_x1/gpu/const.hpp"
#include "core/hw/tegra_x1/gpu/gpu.hpp"

namespace hydra::hw::tegra_x1::gpu {

namespace {

enum class SecondaryOpcode : u32 {
    Grp0UseTert = 0,
    IncMethod = 1,
    Grp2UseTert = 2,
    NonIncMethod = 3,
    ImmDataMethod = 4,
    OneInc = 5,
    Reserved = 6,
    EndPbSegment = 7,
};

// TODO: do both GRP0 and GRP2 use the same tertiary opcodes?
enum class TertiaryOpcode : u32 {
    Grp0IncMethod = 0,
    Grp2NonIncMethod = 0,
    Grp0SetSubDevMask = 1,
    Grp0StoreSubDevMask = 2,
    Grp0UseSubDevMask = 3,
};

} // namespace

} // namespace hydra::hw::tegra_x1::gpu

ENABLE_ENUM_FORMATTING(hydra::hw::tegra_x1::gpu::SecondaryOpcode, Grp0UseTert,
                       "GRP0 use tertiary opcode", IncMethod,
                       "incrementing method", Grp2UseTert,
                       "GRP2 use tertiary opcode", NonIncMethod,
                       "non-incremental method", ImmDataMethod,
                       "immediate data method", OneInc, "one increment",
                       Reserved, "reserved", EndPbSegment, "End PB segment")

ENABLE_ENUM_FORMATTING(hydra::hw::tegra_x1::gpu::TertiaryOpcode, Grp0IncMethod,
                       "GRP0 increment method / GRP2 non-increment method",
                       Grp0SetSubDevMask, "GRP0 set subdevice mask",
                       Grp0StoreSubDevMask, "GRP0 store subdevice mask",
                       Grp0UseSubDevMask, "GRP0 use subdevice mask")

namespace hydra::hw::tegra_x1::gpu {

namespace {

struct CommandHeader {
    u32 method : 12;
    u32 reserved : 1;
    u32 subchannel : 3;
    u32 arg : 13;
    SecondaryOpcode secondary_opcode : 3;
};

} // namespace

void Pfifo::SubmitEntries(GMmu& gmmu, const std::vector<GpfifoEntry>& entries,
                          GpfifoFlags flags) {
    // TODO: flags
    (void)flags;
    LOG_DEBUG(Gpu, "Flags: {}", flags);

    RENDERER_INSTANCE.LockMutex();
    for (const auto& entry : entries) {
        SubmitEntry(gmmu, entry);
    }
    RENDERER_INSTANCE.EndCommandBuffer();
    RENDERER_INSTANCE.UnlockMutex();
}

void Pfifo::SubmitEntry(GMmu& gmmu, const GpfifoEntry entry) {
    LOG_DEBUG(
        Gpu,
        "Gpfifo entry (addr lo: {:#x}, addr hi: {:#x}, size: {:#x}, allow "
        "flush: {}, is push buffer: {}, sync: {})",
        entry.gpu_addr_lo, entry.gpu_addr_hi, entry.size, entry.allow_flush,
        entry.is_push_buffer, entry.sync);

    uptr gpu_addr = static_cast<u64>(entry.gpu_addr_lo) |
                    (static_cast<u64>(entry.gpu_addr_hi) << 32);
    gpu_addr &= ~0x3llu; // Clear the 2 lsb
    uptr end = gpu_addr + entry.size * sizeof(u32);

    while (gpu_addr < end) {
        try {
            if (!SubmitCommand(gmmu, gpu_addr))
                break;
        } catch (Gpu::GetEngineAtSubchannelError error) {
            break;
        } catch (engines::EngineBase::Error error) {
            break;
        }
    }
}

bool Pfifo::SubmitCommand(GMmu& gmmu, uptr& gpu_addr) {
    const auto header = Read<CommandHeader>(gmmu, gpu_addr);
    LOG_DEBUG(
        Gpu, "Method: {:#x}, subchannel: {}, arg: {:#x}, secondary opcode: {}",
        header.method, header.subchannel, header.arg, header.secondary_opcode);

    // HACK
    if (header.subchannel >= SUBCHANNEL_COUNT) {
        ONCE(LOG_WARN(Gpu, "Invalid subchannel {}", header.subchannel));
        return false;
    }

    u32 offset = header.method;
    switch (header.secondary_opcode) {
    case SecondaryOpcode::Grp0UseTert: {
        const auto tert = static_cast<TertiaryOpcode>(header.arg & 0x3);
        switch (tert) {
        default:
            ONCE(LOG_NOT_IMPLEMENTED(Gpu, "Tertiary opcode {}", tert));
            break;
        }
        break;
    }
    case SecondaryOpcode::IncMethod:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(gmmu, header.subchannel, gpu_addr, offset, true);
        break;
    case SecondaryOpcode::Grp2UseTert: {
        const auto tert = static_cast<TertiaryOpcode>(header.arg & 0x3);
        switch (tert) {
        default:
            ONCE(LOG_NOT_IMPLEMENTED(Gpu, "Tertiary opcode {}", tert));
            break;
        }
        break;
    }
    case SecondaryOpcode::NonIncMethod:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(gmmu, header.subchannel, gpu_addr, offset, false);
        break;
    case SecondaryOpcode::ImmDataMethod:
        Gpu::GetInstance().SubchannelMethod(gmmu, header.subchannel, offset,
                                            header.arg);
        break;
    case SecondaryOpcode::OneInc:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(gmmu, header.subchannel, gpu_addr, offset, i == 0);
        break;
    default:
        LOG_NOT_IMPLEMENTED(Gpu, "Secondary opcode {}",
                            header.secondary_opcode);
        break;
    }

    // TODO: is it okay to prefetch the parameters and then execute the
    // macro?
    if (header.method >= MACRO_METHODS_REGION)
        Gpu::GetInstance().SubchannelFlushMacro(gmmu, header.subchannel);

    return true;
}

void Pfifo::ProcessMethodArg(GMmu& gmmu, u32 subchannel, uptr& gpu_addr,
                             u32& method, bool increment) {
    u32 arg = Read<u32>(gmmu, gpu_addr);
    Gpu::GetInstance().SubchannelMethod(gmmu, subchannel, method, arg);
    if (increment)
        method++;
}

} // namespace hydra::hw::tegra_x1::gpu
