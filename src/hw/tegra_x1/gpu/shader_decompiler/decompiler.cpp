#include "hw/tegra_x1/gpu/shader_decompiler/decompiler.hpp"

#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

struct ShaderHeader {
    // CommonWord0
    u32 sph_type : 5;
    u32 version : 5;
    u32 shader_type : 4;
    u32 mrt_enable : 1;
    u32 kills_pixels : 1;
    u32 does_global_store : 1;
    u32 sass_version : 4;
    u32 _reserved0_1 : 1;
    u32 _reserved0_2 : 1;
    u32 _reserved0_3 : 1;
    u32 is_fast_gs : 1;
    u32 vsh_unk_flag : 1;
    u32 does_load_or_store : 1;
    u32 does_fp64 : 1;
    u32 stream_out_mask : 4;

    // CommonWord1
    u32 sh_local_mem_lo_sz : 24;
    u32 per_patch_attrib_cnt : 8;

    // CommonWord2
    u32 sh_local_mem_hi_sz : 24;
    u32 thr_per_input_prim : 8;

    // CommonWord3
    u32 sh_local_mem_crs_sz : 24;
    u32 output_topology : 4;
    u32 _reserved3 : 4;

    // CommonWord4
    u32 max_out_vtx_cnt : 12;
    u32 store_req_start : 8;
    u32 _reserved4 : 4;
    u32 store_req_end : 8;

    // ImapSystemValuesA/ImapSystemValuesB
    u32 imap_sysvals_ab;

    union {
        struct {
            u8 imap_generic_vector[16];
            u16 imap_color;
            u16 imap_sysvals_c;
            u8 imap_fixed_fnc_tex[5];
            u8 _reserved0;
            u32 omap_sysvals_ab;
            u8 omap_generic_vector[16];
            u16 omap_color;
            u16 omap_sysvals_c;
            u8 omap_fixed_fnc_tex[5];
            u8 omap_extra;
        } __attribute__((packed)) vtg;

        struct {
            u8 imap_generic_vector[32];
            u16 imap_color;
            u16 imap_sysvals_c;
            u8 imap_fixed_fnc_tex[10];
            u16 _reserved0;
            u32 omap_target;
            u32 omap_sample_mask : 1;
            u32 omap_depth : 1;
            u32 _reserved1 : 30;
        } ps;
    };
};

void Decompiler::Decompile(Reader& code_reader, Renderer::ShaderType type,
                           std::vector<u8>& out_code) {
    // Builder
    BuilderBase* builder;
    // TODO: choose based on the Shader Decompiler backend
    {
        builder = new Lang::MSL::Builder();
    }

    // Header
    // TODO: don't read in case of compute shaders
    const ShaderHeader header = code_reader.Read<ShaderHeader>();
    LOG_DEBUG(ShaderDecompiler, "Version: {} (should be 3)", header.version);

    // Decompile
    // TODO

    // TODO: don't throw
    throw;

    // HACK
    switch (type) {
    case Renderer::ShaderType::Vertex:
        static std::string vertex_shader_source = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct VertexIn {
                float3 position [[attribute(0)]];
                float3 color [[attribute(1)]];
            };

            struct VertexOut {
                float4 position [[position]];
                float3 color [[user(locn0)]];
            };

            vertex VertexOut main_(VertexIn in [[stage_in]]) {
                VertexOut out;
                out.position = float4(in.position, 1.0);
                out.color = in.color;

                return out;
            }
        )";
        out_code.assign(vertex_shader_source.begin(),
                        vertex_shader_source.end());
        break;
    case Renderer::ShaderType::Fragment:
        static std::string fragment_shader_source = R"(
            #include <metal_stdlib>
            using namespace metal;

            struct VertexOut {
                float4 position [[position]];
                float3 color [[user(locn0)]];
            };

            fragment float4 main_(VertexOut in [[stage_in]]) {
                return float4(in.color, 1.0);
            }
        )";
        out_code.assign(fragment_shader_source.begin(),
                        fragment_shader_source.end());
        break;
    default:
        LOG_ERROR(ShaderDecompiler, "Unknown shader type {}", type);
        break;
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
