#pragma once

#include "hw/tegra_x1/gpu/const.hpp"
#include "hw/tegra_x1/gpu/engines/engine_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {
class DriverBase;
}

namespace Hydra::HW::TegraX1::GPU::Engines {

struct RenderTarget {
    u32 addr_hi;
    u32 addr_lo;
    u32 width;
    u32 height;
    ColorSurfaceFormat format;
    struct {
        u32 width : 4;
        u32 height : 4;
        u32 depth : 4;
        bool is_linear : 1;
        u32 unused : 3;
        bool is_3d : 1;
    } tile_mode;
    struct {
        u16 layers;
        bool volume : 1; // TODO: what is this?
    } array_mode;
    u32 array_pitch;
    u32 base_layer;
    u32 mark; // TODO: what is this?
    u32 padding[6];
};

union Regs3D {
    struct {
        u32 padding1[0x200];

        // Render targets
        RenderTarget color_targets[COLOR_TARGET_COUNT];

        // Viewport transforms
        struct {
            float scale_x;
            float scale_y;
            float scale_z;
            float offset_x;
            float offset_y;
            float offset_z;
            struct {
                u8 x : 3;
                u8 y : 3;
                u8 z : 3;
                u8 w : 3;
                u32 unused : 20;
            } coord_swizzle;
            struct {
                u8 x : 5;
                u32 unused1 : 3;
                u8 y : 5;
                u32 unused2 : 19;
            } subpixel_precision_bias;
        } viewport_transforms[VIEWPORT_COUNT];

        // Viewports
        struct {
            struct {
                u16 x;
                u16 width;
            } horizontal;
            struct {
                u16 y;
                u16 height;
            } vertical;
            float near;
            float far;
        } viewports[VIEWPORT_COUNT];

        u32 padding2[0x9b0];

        u32 mme_firmware_args[8];
    };

    u32 raw[MACRO_METHODS_REGION];
};

class ThreeD : public EngineBase {
  public:
    ThreeD();

    void Method(u32 method, u32 arg) override;

    void FlushMacro() override;

    u32 GetReg(u32 reg) const {
        ASSERT_DEBUG(reg < MACRO_METHODS_REGION, Macro, "Invalid register {}",
                     reg);
        return regs.raw[reg];
    }

  protected:
    void WriteReg(u32 reg, u32 value) override {
        LOG_DEBUG(Engines, "Writing to 3d reg 0x{:08x}", reg);
        regs.raw[reg] = value;
    }

    void Macro(u32 method, u32 arg) override;

    // Helpers
    TextureDescriptor
    CreateTextureDescriptor(const RenderTarget& render_target);

  private:
    Regs3D regs;

    // Macros
    Macro::DriverBase* macro_driver;

    // Commands
    void LoadMmeInstructionRamPointer(const u32 ptr);
    void LoadMmeInstructionRam(const u32 data);
    void LoadMmeStartAddressRamPointer(const u32 ptr);
    void LoadMmeStartAddressRam(const u32 data);

    struct ClearBufferData {
        bool depth : 1;
        bool stencil : 1;
        bool red : 1;
        bool green : 1;
        bool blue : 1;
        bool alpha : 1;
        u32 target_id : 4;
        u32 layer_id : 11;
    };

    void ClearBuffer(const ClearBufferData data);

    void FirmwareCall4(const u32 data);
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
