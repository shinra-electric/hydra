#pragma once

#include "hw/tegra_x1/gpu/engines/const.hpp"
#include "hw/tegra_x1/gpu/engines/engine_base.hpp"
#include "hw/tegra_x1/gpu/renderer/const.hpp"

namespace Hydra::HW::TegraX1::GPU::Macro {
class DriverBase;
}

namespace Hydra::HW::TegraX1::GPU::Renderer {
class BufferBase;
class TextureBase;
class RenderPassBase;
class PipelineBase;
} // namespace Hydra::HW::TegraX1::GPU::Renderer

namespace Hydra::HW::TegraX1::GPU::Engines {

// TODO: move this to the inline engine
struct RegsInline {
    u32 line_length_in;
    u32 line_count;
    u32 offset_out_hi;
    u32 offset_out_lo;
    u32 pitch_out;
    u32 dst_block_size;
    u32 dst_width;
    u32 dst_height;
    u32 dst_depth;
    u32 dst_layer;
    u32 dst_origin_bytes_x;
    u32 dst_origin_samples_y;
    u32 not_a_reg_0xc;
    u32 not_a_reg_0xd;
};

// From Deko3D
struct BufferDescriptor {
    uptr gpu_addr;
    u32 size;
    u32 padding;
};

struct PerStageData {
    u32 textures[TEXTURE_BINDING_COUNT];
    u32 images[IMAGE_BINDING_COUNT];
    BufferDescriptor storageBufs[STORAGE_BUFFER_BINDING_COUNT];
};

struct GraphicsDriverCbuf {
    uint32_t baseVertex;
    uint32_t baseInstance;
    uint32_t drawId;
    uint32_t fbTexHandle;
    PerStageData data[u32(ShaderStage::Count) - 1];
};

struct ComputeDriverCbuf {
    uint32_t ctaSize[3];
    uint32_t gridSize[3];
    uint32_t _padding[2];
    uptr uniformBufs[UNIFORM_BUFFER_BINDING_COUNT];
    PerStageData data;
};

struct TextureImageControl {
    // 0x00
    ImageFormatWord format_word;

    // 0x04
    u32 address_lo;

    // 0x08
    u32 address_hi : 16;
    u32 view_layer_base_3_7 : 5;
    u32 hdr_version : 3;
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

enum class ViewportZClip : u32 {
    MinusOneToOne,
    OneToOne,
};

// TODO: handle this differently
inline Renderer::ShaderType to_renderer_shader_type(ShaderStage stage) {
    switch (stage) {
    case ShaderStage::VertexB:
        return Renderer::ShaderType::Vertex;
    case ShaderStage::Fragment:
        return Renderer::ShaderType::Fragment;
    default:
        LOG_NOT_IMPLEMENTED(Engines, "Shader stage {}", stage);
        return Renderer::ShaderType::Count;
    }
}

union Regs3D {
    struct {
        u32 padding_0x0[0x60];

        RegsInline inline_regs;

        u32 padding_0x6e[0x192];

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

        u32 padding_0x488[0xd5];

        // 0x55d
        u32 tex_header_pool_hi;
        u32 tex_header_pool_lo;
        u32 tex_header_pool_max_index;

        u32 padding_0x560[0x22];

        // 0x582
        u32 shader_program_region_hi;
        u32 shader_program_region_lo;

        u32 attribute_default; // TODO: what is this?

        u32 end;
        struct {
            PrimitiveType primitive_type : 26;
            bool instance_next : 1;
            bool instance_ctrl : 1; // TODO: is the name correct?
        } begin;

        u32 padding9[0x9];

        u32 padding10[0x90];

        // 0x620
        struct {
            bool enable : 32;
        } is_vertex_array_per_instance[VERTEX_ARRAY_COUNT];

        u32 padding_0x630[0x90];

        // 0x6c0 report semaphore
        u32 report_semaphore_addr_hi;
        u32 report_semaphore_addr_lo;
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
            u32 addr_hi;
            u32 addr_lo;
            u32 divisor;
        } vertex_arrays[VERTEX_ARRAY_COUNT];

        u32 padding12[0xc0];

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
        u32 const_buffer_selector_hi;
        u32 const_buffer_selector_lo;

        u32 load_const_buffer_offset;
        u32 not_a_register_0x8e4[0x10];

        u32 padding_0x8f4[0xc];

        u32 padding_0x900[0x400];

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
        LOG_DEBUG(Engines, "Writing to 3d reg 0x{:08x} (value: 0x{:08x})", reg,
                  value);
        regs.raw[reg] = value;
    }

    void Macro(u32 method, u32 arg) override;

  private:
    Regs3D regs{};

    std::vector<u32> inline_data;

    // Macros
    Macro::DriverBase* macro_driver;

    // Active state (for quick access)
    Renderer::ShaderBase* active_shaders[u32(Renderer::ShaderType::Count)] = {
        nullptr};

    // Commands
    void LoadMmeInstructionRamPointer(const u32 index, const u32 ptr);
    void LoadMmeInstructionRam(const u32 index, const u32 data);
    void LoadMmeStartAddressRamPointer(const u32 index, const u32 ptr);
    void LoadMmeStartAddressRam(const u32 index, const u32 data);

    void LaunchDMA(const u32 index, const u32 data);
    void LoadInlineData(const u32 index, const u32 data);

    void DrawVertexArray(const u32 index, u32 count);

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
    Renderer::BufferBase* GetVertexBuffer(u32 vertex_array_index,
                                          u32 max_vertex) const;
    Renderer::TextureBase* GetTexture(const TextureImageControl& tic) const;
    Renderer::TextureBase* GetColorTargetTexture(u32 render_target_index) const;
    Renderer::RenderPassBase* GetRenderPass() const;
    Renderer::ShaderBase* GetShaderUnchecked(ShaderStage stage) const;
    Renderer::ShaderBase* GetShader(ShaderStage stage);
    Renderer::PipelineBase* GetPipeline();

    void ConfigureShaderStage(const ShaderStage stage,
                              const GraphicsDriverCbuf& const_buffer,
                              const TextureImageControl* tex_header_pool);
};

} // namespace Hydra::HW::TegraX1::GPU::Engines
