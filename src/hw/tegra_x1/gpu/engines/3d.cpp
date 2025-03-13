#include "hw/tegra_x1/gpu/engines/3d.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

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

Renderer::TextureDescriptor
ThreeD::CreateTextureDescriptor(u32 render_target_index) {
    const auto& render_target = regs.color_targets[render_target_index];

    return {
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(
            make_addr(render_target.addr_lo, render_target.addr_hi)),
        .surface_format = render_target.surface_format,
        .kind = NvKind::Generic_16BX2, // TODO: correct?
        .width = render_target.width,
        .height = render_target.height,
        .block_height_log2 = render_target.tile_mode.height, // TODO: correct?
        .stride = render_target.width * 4,                   // HACK
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
    LOG_DEBUG(GPU,
              "Depth: {}, stencil: {}, red: {}, green: {}, blue: {}, alpha: "
              "{}, target id: {}, layer id: {}",
              data.depth, data.stencil, data.red, data.green, data.blue,
              data.alpha, data.target_id, data.layer_id);

    // Texture
    auto texture_descriptor = CreateTextureDescriptor(data.target_id);
    auto texture =
        GPU::GetInstance().GetTextureCache().Find(texture_descriptor);

    // HACK
    u32* d = new u32[texture_descriptor.width * texture_descriptor.height];
    for (usize i = 0; i < texture_descriptor.width * texture_descriptor.height;
         i++) {
        u8* ptr = reinterpret_cast<u8*>(&d[i]);
        ptr[0] = regs.clear_color[0] * 0xff;
        ptr[1] = regs.clear_color[1] * 0xff;
        ptr[2] = regs.clear_color[2] * 0xff;
        ptr[3] = regs.clear_color[3] * 0xff;
    }
    RENDERER->UploadTexture(texture, d);
    delete[] d;
}

void ThreeD::FirmwareCall4(const u32 data) {
    LOG_NOT_IMPLEMENTED(Engines, "Firmware call 4");

    // TODO: find out what this does
    regs.mme_firmware_args[0] = 0x1;
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
