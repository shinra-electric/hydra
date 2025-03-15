#include "hw/tegra_x1/gpu/pfifo.hpp"

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"

namespace Hydra::HW::TegraX1::GPU {

namespace {

enum class SecondaryOpcode {
    Grp0UseTert,
    IncMethod,
    Grp2UseTert,
    NonIncMethod,
    ImmDataMethod,
    OneInc,
    Reserved,
    EndPbSegment,
};

struct CommandHeader {
    u32 method : 12;
    u32 reserved : 1;
    u32 subchannel : 3;
    u32 arg : 13;
    u32 secondary_opcode : 3; // TODO: use SecondaryOpcode as the type
};

} // namespace

void Pfifo::SubmitEntries(const std::vector<GpfifoEntry>& entries,
                          GpfifoFlags flags) {
    LOG_DEBUG(GPU, "Flags: {}", flags);
    RENDERER->BeginCommandBuffer();
    for (const auto& entry : entries) {
        SubmitEntry(entry);
    }
    RENDERER->EndCommandBuffer();
}

void Pfifo::SubmitEntry(const GpfifoEntry entry) {
    LOG_DEBUG(GPU,
              "Gpfifo entry (address: 0x{:08x}, size: 0x{:08x}, allow "
              "flush: {}, is push buffer: {}, sync: {})",
              entry.gpu_addr, entry.size, entry.allow_flush,
              entry.is_push_buffer, entry.sync);

    uptr gpu_addr = entry.gpu_addr;
    uptr end = entry.gpu_addr + entry.size * sizeof(u32);

    // HACK: the last 4 words seem to always be the same, subchannel: 6, method:
    // 0x00b
    // end -= 4 * sizeof(u32);

    while (gpu_addr < end) {
        if (!SubmitCommand(gpu_addr))
            break;
    }
}

bool Pfifo::SubmitCommand(uptr& gpu_addr) {
    const auto header = Read<CommandHeader>(gpu_addr);

    SecondaryOpcode secondary_opcode = (SecondaryOpcode)header.secondary_opcode;
    LOG_DEBUG(GPU, "Secondary opcode: {}", secondary_opcode);

    // HACK
    if (header.subchannel >= SUBCHANNEL_COUNT) {
        LOG_WARNING(GPU, "Invalid subchannel {}", header.subchannel);
        return false;
    }

    u32 offset = header.method;
    switch (secondary_opcode) {
    case SecondaryOpcode::IncMethod:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, true);
        break;
    case SecondaryOpcode::NonIncMethod:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, false);
        break;
    case SecondaryOpcode::ImmDataMethod:
        GPU::GetInstance().SubchannelMethod(header.subchannel, offset,
                                            header.arg);
        break;
    case SecondaryOpcode::OneInc:
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, i == 0);
        break;
    default:
        LOG_NOT_IMPLEMENTED(GPU, "{}", secondary_opcode);
        break;
    }

    // TODO: is it okay to prefetch the parameters and then execute the macro?
    if (header.method >= MACRO_METHODS_REGION)
        GPU::GetInstance().SubchannelFlushMacro(header.subchannel);

    return true;
}

void Pfifo::ProcessMethodArg(u32 subchannel, uptr& gpu_addr, u32& method,
                             bool increment) {
    u32 arg = Read<u32>(gpu_addr);
    GPU::GetInstance().SubchannelMethod(subchannel, method, arg);
    if (increment)
        method++;
}

} // namespace Hydra::HW::TegraX1::GPU

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::SecondaryOpcode, Grp0UseTert,
                       "GRP0 use tertiary opcode", IncMethod,
                       "incrementing method", Grp2UseTert,
                       "GRP2 use tertiary opcode", NonIncMethod,
                       "non-incremental method", ImmDataMethod,
                       "immediate data method", OneInc, "one increment",
                       Reserved, "reserved", EndPbSegment, "End PB segment")
