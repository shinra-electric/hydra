#pragma once

#include "core/hw/tegra_x1/gpu/engines/inline_base.hpp"
#include "core/hw/tegra_x1/gpu/renderer/const.hpp"

#define REGS_3D engines::ThreeD::GetInstance().GetRegs()

namespace hydra::hw::tegra_x1::gpu::macro {
class DriverBase;
}

namespace hydra::hw::tegra_x1::gpu::renderer {
class BufferBase;
class TextureBase;
class SamplerBase;
class RenderPassBase;
class PipelineBase;
} // namespace hydra::hw::tegra_x1::gpu::renderer

namespace hydra::hw::tegra_x1::gpu::engines {

enum class TicHdrVersion : u32 {
    _1DBuffer = 0,
    PitchColorKey = 1,
    Pitch = 2,
    BlockLinear = 3,
    BlockLinearColorKey = 4,
};

struct TextureImageControl {
    // 0x00
    ImageFormatWord format_word;

    // 0x04
    u32 addr_lo;

    // 0x08
    u32 addr_hi : 16;
    u32 view_layer_base_3_7 : 5;
    TicHdrVersion hdr_version : 3;
    u32 load_store_hint_maybe : 1;
    u32 view_coherency_hash : 4;
    u32 view_layer_base_8_10 : 3;

    // 0x0C
    union {
        u16 width_minus_one_16_31;
        u16 pitch_5_20;
        struct {
            u16 tile_width_gobs_log2 : 3;
            u16 tile_height_gobs_log2 : 3;
            u16 tile_depth_gobs_log2 : 3;
            u16 : 1;
            u16 sparse_tile_width_gobs_log2 : 3;
            u16 gob_3d : 1;
            u16 : 2;
        };
    };
    u16 lod_aniso_quality_2 : 1;
    u16 lod_aniso_quality : 1;            // LodQuality
    u16 lod_iso_quality : 1;              // LodQuality
    u16 aniso_coarse_spread_modifier : 2; // AnisoSpreadModifier
    u16 aniso_spread_scale : 5;
    u16 use_header_opt_control : 1;
    u16 depth_texture : 1;
    u16 mip_max_levels : 4;

    // 0x10
    u32 width_minus_one : 16;
    u32 view_layer_base_0_2 : 3;
    u32 aniso_spread_max_log2 : 3;
    u32 is_sRGB : 1;
    u32 texture_type : 4;     // TextureType
    u32 sector_promotion : 2; // SectorPromotion
    u32 border_size : 3;      // BorderSize

    // 0x14
    u32 height_minus_one : 16;
    u32 depth_minus_one : 14;
    u32 is_sparse : 1;
    u32 normalized_coords : 1;

    // 0x18
    u32 color_key_op : 1;
    u32 trilin_opt : 5;
    u32 mip_lod_bias : 13;
    u32 aniso_bias : 4;
    u32 aniso_fine_spread_func : 2;     // AnisoSpreadFunc
    u32 aniso_coarse_spread_func : 2;   // AnisoSpreadFunc
    u32 max_anisotropy : 3;             // MaxAnisotropy
    u32 aniso_fine_spread_modifier : 2; // AnisoSpreadModifier

    // 0x1C
    union {
        u32 color_key_value;
        struct {
            u32 view_mip_min_level : 4;
            u32 view_mip_max_level : 4;
            u32 msaa_mode : 4; // MsaaMode
            u32 min_lod_clamp : 12;
            u32 : 8;
        };
    };
};

struct TextureSamplerControl {
    // 0x00
    u32 address_u : 3;
    u32 address_v : 3;
    u32 address_p : 3;
    u32 depth_compare : 1;
    u32 depth_compare_op : 3;
    u32 srgb_conversion : 1;
    u32 font_filter_width : 3;
    u32 font_filter_height : 3;
    u32 max_anisotropy : 3;
    u32 : 9;

    // 0x04
    u32 mag_filter : 2;
    u32 : 2;
    u32 min_filter : 2;
    u32 mip_filter : 2;
    u32 cubemap_anisotropy : 1;
    u32 cubemap_interface_filtering : 1;
    u32 reduction_filter : 2;
    u32 mip_lod_bias : 13;
    u32 float_coord_normalization : 1;
    u32 trilin_opt : 5;
    u32 : 1;

    // 0x08
    u32 min_lod_clamp : 12;
    u32 max_lod_clamp : 12;
    u32 srgb_border_color_r : 8;

    // 0x0C
    u32 : 12;
    u32 srgb_border_color_g : 8;
    u32 srgb_border_color_b : 8;
    u32 : 4;

    // 0x10
    u32 border_color_r;

    // 0x14
    u32 border_color_g;

    // 0x18
    u32 border_color_b;

    // 0x1C
    u32 border_color_a;
};

struct RenderTarget {
    Iova addr;
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

enum class ViewportZClip : u32 {
    MinusOneToOne,
    OneToOne,
};

enum class WindowOriginFlags : u32 {
    None = 0,
    LowerLeft = BIT(0),
    FlipY = BIT(4),
};
ENABLE_ENUM_BITMASK_OPERATORS(WindowOriginFlags)

enum class ViewportSwizzle : u32 {
    PositiveX = 0,
    NegativeX = 1,
    PositiveY = 2,
    NegativeY = 3,
    PositiveZ = 4,
    NegativeZ = 5,
    PositiveW = 6,
    NegativeW = 7,
};

// TODO: handle this differently
inline renderer::ShaderType to_renderer_shader_type(ShaderStage stage) {
    switch (stage) {
    case ShaderStage::VertexB:
        return renderer::ShaderType::Vertex;
    case ShaderStage::Fragment:
        return renderer::ShaderType::Fragment;
    default:
        ONCE(LOG_NOT_IMPLEMENTED(Engines, "Shader stage {}", stage));
        return renderer::ShaderType::Count;
    }
}

struct Regs3D {
    RegsInline regs_inline;

    u32 padding_0x6e[0x192];

    // 0x200 Render targets
    RenderTarget color_targets[COLOR_TARGET_COUNT];

    // 0x280 Viewport transforms
    struct {
        f32 scale_x;
        f32 scale_y;
        f32 scale_z;
        f32 offset_x;
        f32 offset_y;
        f32 offset_z;
        struct {
            ViewportSwizzle x : 3;
            ViewportSwizzle y : 3;
            ViewportSwizzle z : 3;
            ViewportSwizzle w : 3;
            u32 unused : 20;
        } swizzle;
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
            i16 x;
            i16 width;
        } horizontal;
        struct {
            i16 y;
            i16 height;
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
    uint4 clear_color;
    float clear_depth;

    u32 padding_0x365[0x2];

    // 0x367
    bool32 color_reduction_enable;
    u32 clear_stencil;

    u32 padding_0x369[0xb];

    // 0x374
    bool32 advanced_blend_enabled;

    u32 padding_0x375[0xb];

    // 0x380 scissor
    struct {
        bool32 enabled;
        struct {
            u16 min;
            u16 max;
        } horizontal;
        struct {
            u16 min;
            u16 max;
        } vertical;
        u32 _padding;
    } scissors[SCISSOR_COUNT];

    u32 padding_0x3c0[0x30];
    u32 padding_0x3f0[0x8];

    // 0x3f8 depth target
    Iova depth_target_addr;
    DepthSurfaceFormat depth_target_format;
    struct {
        u32 width : 4;
        u32 height : 4;
        u32 depth : 4;
        u32 padding : 20;
    } depth_target_tile_mode;
    u32 depth_target_layer_stride;

    u32 padding_0x3fd[0x53];
    u32 padding_0x450[0x8];

    // 0x458 vertex attribute states
    VertexAttribState vertex_attrib_states[VERTEX_ATTRIB_COUNT];

    u32 padding_0x478[0xf];

    // 0x487 color target control
    struct {
        u32 count : 4;
        u32 map0 : 3;
        u32 map1 : 3;
        u32 map2 : 3;
        u32 map3 : 3;
        u32 map4 : 3;
        u32 map5 : 3;
        u32 map6 : 3;
        u32 map7 : 3;
    } color_target_control;

    u32 padding_0x488[0x2];

    // 0x48a depth target dimensions
    u32 depth_target_width;
    u32 depth_target_height;
    struct {
        u16 layers;
        bool volume;
    } depth_target_array_mode;

    u32 padding_0x48d[0x26];

    // 0x4b3
    bool32 depth_test_enabled;

    u32 padding_0x4b4[0x5];

    // 0x4b9
    bool32 independent_blend_enabled;
    bool32 depth_write_enabled;

    u32 padding_0x4bb[0x8];

    // 0x4c3
    DepthTestFunc depth_test_func;

    u32 padding_0x4c4[0x3];

    // 0x4c7
    float4 blend_constant;

    u32 padding_0x4cb[0x4];

    // 0x4cf
    struct {
        u32 separate_for_alpha; // TODO: is this a bitfield or something?
        u32 rgb_op;
        u32 src_rgb_factor;
        u32 dst_rgb_factor;
        u32 alpha_op;
        u32 src_alpha_factor;
        u32 global_color_key; // TODO: what is this?
        u32 dst_alpha_factor;
    } blend_state;
    u32 single_rop_control;
    u32 color_blend_enabled[COLOR_TARGET_COUNT];

    u32 padding_0x4e0[0xb];

    // 0x4eb
    WindowOriginFlags window_origin_flags;

    u32 padding_0x4ec[0x21];

    // 0x50d
    u32 base_vertex;
    u32 base_instance;

    u32 padding_0x50f[0x3f];

    // 0x54e
    bool32 depth_target_enabled;

    u32 padding_0x54f[0x8];

    // 0x557
    Iova tex_sampler_pool;
    u32 tex_sampler_pool_max_index;

    u32 padding_0x55a[0x3];

    // 0x55d
    Iova tex_header_pool;
    u32 tex_header_pool_max_index;

    u32 padding_0x560[0x22];

    // 0x582
    Iova shader_program_region;
    u32 attribute_default; // TODO: what is this?

    u32 end;
    struct {
        PrimitiveType primitive_type : 26;
        bool instance_next : 1;
        bool instance_ctrl : 1; // TODO: is the name correct?
    } begin;

    u32 padding_0x587[0x9];

    u32 padding_0x590[0x62];

    // 0x5f2 indexed draws
    Iova index_buffer_addr;
    Iova index_buffer_limit_addr;

    IndexType index_type;

    u32 vertex_elements_start;
    u32 not_a_register_0x5f8;

    u32 padding_0x5f9[0x27];

    // 0x620
    bool32 is_vertex_array_per_instance[VERTEX_ARRAY_COUNT];

    u32 padding_0x630[0x1b];

    // 0x64b
    bool32 viewport_transform_enabled;

    u32 padding_0x64c[0x34];

    // 0x680
    ColorWriteMask color_write_masks[COLOR_TARGET_COUNT];

    u32 padding_0x688[0x38];

    // 0x6c0 report semaphore
    Iova report_semaphore_addr;
    u32 report_semaphore_payload;
    u32 report_semaphore_todo; // TODO: should this be a reg?

    u32 padding_0x6c4[0x3c];

    // 0x700 vertex arrays
    struct {
        struct {
            u32 stride : 12;
            bool enable : 1;
            u32 padding : 19;
        } config;
        Iova addr;
        u32 divisor;
    } vertex_arrays[VERTEX_ARRAY_COUNT];

    u32 padding_0x740[0x40];

    // 0x780
    struct {
        u32 unknown; // TODO: is this just padding?
        u32 rgb_op;
        u32 src_rgb_factor;
        u32 dst_rgb_factor;
        u32 alpha_op;
        u32 src_alpha_factor;
        u32 dst_alpha_factor;
        u32 padding;
    } independent_blend_state[COLOR_TARGET_COUNT];

    Iova vertex_array_limits[VERTEX_ARRAY_COUNT];

    u32 padding_0x7e0[0x20];

    // 0x800 shader programs
    struct {
        struct {
            bool enable : 1;
            u32 padding1 : 3;
            ShaderStage stage : 4;
            u32 padding2 : 24;
        } config;
        u32 offset;
        u32 padding1;
        u32 num_registers;
        u32 padding2[0xc];
    } shader_programs[u32(ShaderStage::Count)];

    u32 padding_0x860[0x80];

    // 0x8e0 constant buffers
    u32 const_buffer_selector_size;
    Iova const_buffer_selector;

    u32 load_const_buffer_offset;
    u32 not_a_register_0x8e4[0x10];

    u32 padding_0x8f4[0xc];
    u32 padding_0x900[0x82];

    // 0x982
    u32 bindless_texture_const_buffer_slot;

    u32 padding_0x983[0x37d];

    // 0xd00
    u32 mme_scratch[0x80];
} PACKED;

class ThreeD : public EngineWithRegsBase<Regs3D>, public InlineBase {
  public:
    static ThreeD& GetInstance();

    ThreeD();
    ~ThreeD() override;

    void Method(u32 method, u32 arg) override;

    void FlushMacro() override;

    // Getters
    const Regs3D& GetRegs() const { return regs; }

  protected:
    void Macro(u32 method, u32 arg) override;

  private:
    // Macros
    macro::DriverBase* macro_driver;

    // Active state (for quick access)
    renderer::ShaderBase* active_shaders[u32(renderer::ShaderType::Count)] = {
        nullptr};

    // State
    uptr bound_const_buffers[CONST_BUFFER_BINDING_COUNT] = {0x0};

    // Methods
    DEFINE_INLINE_ENGINE_METHODS;

    void LoadMmeInstructionRamPointer(const u32 index, const u32 ptr);
    void LoadMmeInstructionRam(const u32 index, const u32 data);
    void LoadMmeStartAddressRamPointer(const u32 index, const u32 ptr);
    void LoadMmeStartAddressRam(const u32 index, const u32 data);

    void DrawVertexArray(const u32 index, u32 count);
    void DrawVertexElements(const u32 index, u32 count);

    struct ClearBufferData {
        bool depth : 1;
        bool stencil : 1;
        u8 color_mask : 4;
        u32 target_id : 4;
        u32 layer_id : 11;
    };

    void ClearBuffer(const u32 index, const ClearBufferData data);

    // HACK
    void SetReportSemaphore(const u32 index, const u32 data);

    void FirmwareCall4(const u32 index, const u32 data);

    void LoadConstBuffer(const u32 index, const u32 data);
    void BindGroup(const u32 index, const u32 data);

    // Helpers
    renderer::BufferBase* GetVertexBuffer(u32 vertex_array_index) const;
    renderer::TextureBase* GetTexture(const TextureImageControl& tic) const;
    renderer::SamplerBase* GetSampler(const TextureSamplerControl& tsc) const;
    renderer::TextureBase* GetColorTargetTexture(u32 render_target_index) const;
    renderer::TextureBase* GetDepthStencilTargetTexture() const;
    renderer::RenderPassBase* GetRenderPass() const;
    renderer::ShaderBase* GetShaderUnchecked(ShaderStage stage) const;
    renderer::ShaderBase* GetShader(ShaderStage stage);
    renderer::PipelineBase* GetPipeline();

    void ConfigureShaderStage(const ShaderStage stage,
                              const TextureImageControl* tex_header_pool,
                              const TextureSamplerControl* tex_sampler_pool);

    bool DrawInternal();
};

} // namespace hydra::hw::tegra_x1::gpu::engines

ENABLE_ENUM_FORMATTING(hydra::hw::tegra_x1::gpu::engines::TicHdrVersion,
                       _1DBuffer, "1D buffer", PitchColorKey, "pitch color key",
                       Pitch, "pitch", BlockLinear, "block linear",
                       BlockLinearColorKey, "block linear color key")

ENABLE_ENUM_FORMATTING(hydra::hw::tegra_x1::gpu::engines::ViewportSwizzle,
                       PositiveX, "positive X", NegativeX, "negative X",
                       PositiveY, "positive Y", NegativeY, "negative Y",
                       PositiveZ, "positive Z", NegativeZ, "negative Z",
                       PositiveW, "positive W", NegativeW, "negative W")
