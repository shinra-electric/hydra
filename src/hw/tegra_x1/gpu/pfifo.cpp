#include "hw/tegra_x1/gpu/pfifo.hpp"

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
    Subchannel subchannel : 3;
    u32 arg : 13;
    SecondaryOpcode secondary_opcode : 3;
};

} // namespace

void Pfifo::SubmitEntries(const std::vector<GpfifoEntry>& entries) {
    for (const auto& entry : entries) {
        SubmitEntry(entry);
    }
}

void Pfifo::SubmitEntry(const GpfifoEntry entry) {
    LOG_DEBUG(GPU,
              "Gpfifo entry (address: 0x{:08x}, num_cmds: 0x{:08x}, allow "
              "flush: {}, is push buffer: {}, sync: {})",
              entry.gpu_addr, entry.num_cmds, entry.allow_flush,
              entry.is_push_buffer, entry.sync);

    uptr gpu_addr = entry.gpu_addr;
    for (u32 i = 0; i < entry.num_cmds; i++) {
        gpu_addr = SubmitCommand(gpu_addr);
    }
}

uptr Pfifo::SubmitCommand(uptr gpu_addr) {
    const auto header = Read<CommandHeader>(gpu_addr);

    if (header.method >= 0xe00) { // TODO: correct?
        // Macro
        LOG_NOT_IMPLEMENTED(GPU, "GPU macros");
        return gpu_addr;
    }

    u32 offset = header.method;
    switch (header.secondary_opcode) {
    case SecondaryOpcode::IncMethod: {
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, true);
        break;
    }
    case SecondaryOpcode::NonIncMethod: {
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, false);
        break;
    }
    case SecondaryOpcode::ImmDataMethod: {
        GPU::GetInstance().WriteSubchannelReg(header.subchannel, offset,
                                              header.arg);
        break;
    }
    case SecondaryOpcode::OneInc: {
        for (u32 i = 0; i < header.arg; i++)
            ProcessMethodArg(header.subchannel, gpu_addr, offset, i == 0);
        break;
    }
    default:
        LOG_NOT_IMPLEMENTED(GPU, header.secondary_opcode);
        break;
    }

    return gpu_addr;
}

void Pfifo::ProcessMethodArg(Subchannel subchannel, uptr& gpu_addr, u32& offset,
                             bool increment) {
    u32 arg = Read<u32>(gpu_addr);
    GPU::GetInstance().WriteSubchannelReg(subchannel, offset, arg);
    if (increment)
        offset += sizeof(u32);
}

} // namespace Hydra::HW::TegraX1::GPU

ENABLE_ENUM_FORMATTING(Hydra::HW::TegraX1::GPU::SecondaryOpcode, Grp0UseTert,
                       "GRP0 use tertiary opcode", IncMethod,
                       "incrementing method", Grp2UseTert,
                       "GRP2 use tertiary opcode", NonIncMethod,
                       "non-incremental method", ImmDataMethod,
                       "immediate data method", OneInc, "one increment",
                       Reserved, "reserved", EndPbSegment, "End PB segment")
