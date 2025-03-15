#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/engines/engine_base.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {
class DriverBase;
}

namespace Hydra::HW::TegraX1::GPU::Renderer {
class RenderPassBase;
class PipelineBase;
} // namespace Hydra::HW::TegraX1::GPU::Renderer

namespace Hydra::HW::TegraX1::GPU::Engines {

struct RenderTarget {
    u32 addr_hi;
    u32 addr_lo;
    u32 width;
    u32 height;
    SurfaceFormat surface_format;
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

enum class ViewportZClip : u32 {
    MinusOneToOne,
    OneToOne,
};

union Regs3D {
    struct {
        u32 padding1[0x200];

        // 0x200 Render targets
        RenderTarget color_targets[COLOR_TARGET_COUNT];

        // 0x280 Viewport transforms
        struct {
            float scale_x;
            float scale_y;
            float scale_z;
            float offset_x;
            float offset_y;
            float offset_z;
            struct {
                u32 x : 3;
                u32 y : 3;
                u32 z : 3;
                u32 w : 3;
                u32 unused : 20;
            } coord_swizzle;
            struct {
                u32 x : 5;
                u32 unused1 : 3;
                u32 y : 5;
                u32 unused2 : 19;
            } subpixel_precision_bias;
        } viewport_transforms[VIEWPORT_COUNT];

        // 0x300 Viewports
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

        // 0x340 Window rectangle
        struct {
            struct {
                u16 min;
                u16 max;
            } horizontal;
            struct {
                u16 min;
                u16 max;
            } vertical;
        } window_rects[8];

        u32 padding2[0xd];

        // 0x35d
        u32 vertex_array_start;

        u32 not_a_register_0x35e;

        // 0x35f
        ViewportZClip viewport_z_clip;

        // 0x360 Clear data
        u32 clear_color[4];
        float clear_depth;

        u32 padding3[0x2];

        bool color_reduction_enable : 32;
        u32 clear_stencil;

        u32 padding4[0x7];
        u32 padding5[0xe0];
        u32 padding6[0x8];

        // 0x458 vertex attribute state
        VertexAttribState vertex_attrib_states[VERTEX_ATTRIB_COUNT];

        u32 padding7[0x8];
        u32 padding8[0x1c0];

        // 0x620
        struct {
            bool enable : 32;
        } is_vertex_array_per_instance[VERTEX_ARRAY_COUNT];

        u32 padding9[0xe0];

        // 0x700 vertex array
        struct {
            struct {
                u32 stride : 12;
                bool enable : 1;
                u32 padding : 19;
            } config;
            u32 addr_hi;
            u32 addr_lo;
            u32 divisor;
        } vertex_arrays[VERTEX_ARRAY_COUNT];

        u32 padding10[0x590];

        // 0xd00
        u32 mme_firmware_args[8];
    };

    u32 raw[MACRO_METHODS_REGION];
};

class ThreeD : public EngineBase {
  public:
    static ThreeD& GetInstance();

    ThreeD();
    ~ThreeD() override;

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

  private:
    Regs3D regs;

    // Macros
    Macro::DriverBase* macro_driver;

    // Commands
    void LoadMmeInstructionRamPointer(const u32 ptr);
    void LoadMmeInstructionRam(const u32 data);
    void LoadMmeStartAddressRamPointer(const u32 ptr);
    void LoadMmeStartAddressRam(const u32 data);

    void DrawVertexArray(const u32 count);

    struct ClearBufferData {
        bool depth : 1;
        bool stencil : 1;
        u8 color_mask : 4;
        u32 target_id : 4;
        u32 layer_id : 11;
    };

    void ClearBuffer(const ClearBufferData data);

    void FirmwareCall4(const u32 data);

    // Texture
    Renderer::TextureBase* GetColorTargetTexture(u32 render_target_index) const;

    // Helpers
    Renderer::RenderPassBase* GetRenderPass() const;
    Renderer::PipelineBase* GetPipeline() const;
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
