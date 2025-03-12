#include "hw/tegra_x1/gpu/engines/3d.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

DEFINE_METHOD_TABLE(ThreeD, 0x45, LoadMmeInstructionRamPointer, u32, 0x46,
                    LoadMmeInstructionRam, u32, 0x47,
                    LoadMmeStartAddressRamPointer, u32, 0x48,
                    LoadMmeStartAddressRam, u32, 0x674, ClearBuffer,
                    ClearBufferData, 0x8c4, FirmwareCall4, u32)

ThreeD::ThreeD() {
    // TODO: choose based on Macro backend
    {
        macro_driver = new Macro::Interpreter::Driver(this);
    }
}

void ThreeD::FlushMacro() { macro_driver->Execute(); }

void ThreeD::Macro(u32 method, u32 arg) {
    u32 index = (method - MACRO_METHODS_REGION) >> 1;
    LOG_DEBUG(Engines, "Macro (index: 0x{:08x})", index);
    if ((method & 0x1) == 0x0) {
        LOG_DEBUG(Engines, "Parameter1: 0x{:08x}", arg);
        macro_driver->SetIndex(index);
        macro_driver->LoadParam1(arg);
    } else {
        LOG_DEBUG(Engines, "ParameterN: 0x{:08x}", arg);
        macro_driver->LoadParam(arg);
    }
}

TextureDescriptor
ThreeD::CreateTextureDescriptor(const RenderTarget& render_target) {
    return {
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(
            make_addr(render_target.addr_lo, render_target.addr_hi)),
        .color_surface_format = render_target.format,
        .kind = NvKind::Generic_16BX2, // TODO: correct?
        .width = render_target.width,
        .height = render_target.height,
        .stride = invalid<u32>(),                            // TODO
        .block_height_log2 = render_target.tile_mode.height, // TODO: correct?
        .pitch =
            render_target.array_pitch / render_target.height, // TODO: correct?
    };
}

void ThreeD::LoadMmeInstructionRamPointer(const u32 ptr) {
    macro_driver->LoadInstructionRamPointer(ptr);
}

void ThreeD::LoadMmeInstructionRam(const u32 data) {
    macro_driver->LoadInstructionRam(data);
}

void ThreeD::LoadMmeStartAddressRamPointer(const u32 ptr) {
    macro_driver->LoadStartAddressRamPointer(ptr);
}

void ThreeD::LoadMmeStartAddressRam(const u32 data) {
    macro_driver->LoadStartAddressRam(data);
}

void ThreeD::ClearBuffer(const ClearBufferData data) {
    LOG_FUNC_NOT_IMPLEMENTED(Engines);
    LOG_DEBUG(GPU,
              "Depth: {}, stencil: {}, red: {}, green: {}, blue: {}, alpha: "
              "{}, target id: {}, layer id: {}",
              data.depth, data.stencil, data.red, data.green, data.blue,
              data.alpha, data.target_id, data.layer_id);

    auto texture_descriptor =
        CreateTextureDescriptor(regs.color_targets[data.target_id]);
    auto texture =
        GPU::GetInstance().GetTextureCache().FindTexture(texture_descriptor);
}

void ThreeD::FirmwareCall4(const u32 data) {
    LOG_NOT_IMPLEMENTED(Engines, "Firmware call 4");

    // TODO: find out what this does
    regs.mme_firmware_args[0] = 0x1;
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
