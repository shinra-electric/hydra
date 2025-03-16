#include "hw/tegra_x1/gpu/shader_decompiler/decompiler.hpp"

#include "hw/tegra_x1/gpu/shader_decompiler/lang/msl/builder.hpp"
#include "hw/tegra_x1/gpu/shader_decompiler/tables.hpp"

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
    // HACK: just for testing
    ASSERT_DEBUG(header.version == 3, ShaderDecompiler,
                 "Invalid shader version {}", header.version);

    // Decompile
    for (u32 i = 0; i < 12; i++) {
        u64 inst = code_reader.Read<u64>();
        LOG_DEBUG(ShaderDecompiler, "Instruction 0x{:016x}", inst);

        ParseInstruction(inst);
    }

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

void Decompiler::ParseInstruction(u64 inst) {
    INST0(0xfbe0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "out");
    INST(0xf6e0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "out");
    INST(0xf0f8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sync");
    INST(0xf0f0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "depbar");
    INST(0xf0c8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mov");
    INST(0xf0c0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "r2b");
    INST(0xf0b8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "b2r");
    INST(0xf0a8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bar");
    INST(0xeff0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "st");
    INST(0xefe8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pixld");
    INST(0xefd8000000000000, 0xfff8000000000000)
    LOG_DEBUG(ShaderDecompiler, "ld {}", GetOperandEff0_0(inst));
    INST(0xefd0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "isberd");
    INST(0xefa0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "al2p");
    INST(0xef98000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "membar");
    INST(0xef90000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ld");
    INST(0xef80000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cctll");
    INST(0xef60000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cctl");
    INST(0xef58000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "st");
    INST(0xef50000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "st");
    INST(0xef48000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ld");
    INST(0xef40000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ld");
    INST(0xef10000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shfl");
    INST(0xeef0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "atom");
    INST(0xeed8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "stg");
    INST(0xeed0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ldg");
    INST(0xeec8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ldg");
    INST(0xeea0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "stp");
    INST(0xee00000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "atoms");
    INST(0xed00000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "atom");
    INST(0xec00000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "atoms");
    INST(0xebf8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "red");
    INST(0xebf0000000000000, 0xfff9000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cctlt");
    INST(0xebe0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "out");
    INST(0xeb40000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sured");
    INST(0xeb20000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sust");
    INST(0xeb00000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "suld");
    INST(0xeac0000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "suatom");
    INST(0xea80000000000000, 0xffc0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "suatom");
    INST(0xea60000000000000, 0xffe0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "suatom");
    INST(0xea00000000000000, 0xffc0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "suatom");
    INST(0xe3a0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bpt");
    INST(0xe390000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ide");
    INST(0xe380000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ram");
    INST(0xe370000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sam");
    INST(0xe360000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "rtt");
    INST(0xe350000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cont");
    INST(0xe340000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "brk");
    INST(0xe330000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "kil");
    INST(0xe320000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ret");
    INST(0xe310000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "longjmp");
    INST(0xe300000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "exit");
    INST(0xe2f0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "setlmembase");
    INST(0xe2e0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "setcrsptr");
    INST(0xe2d0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "getlmembase");
    INST(0xe2c0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "getcrsptr");
    INST(0xe2b0000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pcnt");
    INST(0xe2b0000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pcnt");
    INST(0xe2a0000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pbk");
    INST(0xe2a0000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pbk");
    INST(0xe290000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ssy");
    INST(0xe290000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ssy");
    INST(0xe280000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "plongjmp");
    INST(0xe280000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "plongjmp");
    INST(0xe270000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pret");
    INST(0xe270000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pret");
    INST(0xe260000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cal");
    INST(0xe260000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cal");
    INST(0xe250000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "brx");
    INST(0xe250000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "brx");
    INST(0xe240000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bra");
    INST(0xe240000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bra");
    INST(0xe230000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pexit");
    INST(0xe220000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jcal");
    INST(0xe220000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jcal");
    INST(0xe210000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jmp");
    INST(0xe210000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jmp");
    INST(0xe200000000000020, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jmx");
    INST(0xe200000000000000, 0xfff0000000000020)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "jmx");
    INST(0xe00000000000ff00, 0xff0000400000ff00)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ipa");
    INST(0xe000004000000000, 0xff00004000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ipa");
    INST(0xdf60000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tmml");
    INST(0xdf58000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tmml");
    INST(0xdf50000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "txq");
    INST(0xdf48000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "txq");
    INST(0xdf40000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "txa");
    INST(0xdf00000000000000, 0xff40000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tld4s");
    INST(0xdef8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tld4");
    INST(0xdeb8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tex");
    INST(0xde78000000000000, 0xfffc000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "txd");
    INST(0xde38000000000000, 0xfffc000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "txd");
    INST(0xdd38000000000000, 0xff38000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tld");
    INST(0xdc38000000000000, 0xff38000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tld");
    INST(0xd200000000000000, 0xf600000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tlds");
    INST(0xd000000000000000, 0xf600000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "texs");
    INST(0xc838000000000000, 0xfc38000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tld4");
    INST(0xc038000000000000, 0xfc38000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "tex");
    INST(0xa000000000000000, 0xe000000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "st");
    INST(0x8000000000000000, 0xe000000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ld");
    INST(0x7e80000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hsetp2");
    INST(0x7e00000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hsetp2");
    INST(0x7c80000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hset2");
    INST(0x7c00000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hset2");
    INST(0x7a00000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hadd2");
    INST(0x7a80000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hadd2");
    INST(0x7800000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hmul2");
    INST(0x7880000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hmul2");
    INST(0x7080000000000000, 0xf880000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hfma2");
    INST(0x7000000000000000, 0xf880000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hfma2");
    INST(0x6080000000000000, 0xf880000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hfma2");
    INST(0x5f00000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vmad");
    INST(0x5d20000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hsetp2");
    INST(0x5d18000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hset2");
    INST(0x5d10000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hadd2");
    INST(0x5d08000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hmul2");
    INST(0x5d00000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hfma2");
    INST(0x5cf8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shf");
    INST(0x5cf0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "r2p");
    INST(0x5ce8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "p2r");
    INST(0x5ce0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2i");
    INST(0x5cc0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd3");
    INST(0x5cb8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2f");
    INST(0x5cb0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2i");
    INST(0x5ca8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2f");
    INST(0x5ca0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sel");
    INST(0x5c98000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mov");
    INST(0x5c90000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "rro");
    INST(0x5c88000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fchk");
    INST(0x5c80000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmul");
    INST(0x5c70000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dadd");
    INST(0x5c68000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmul");
    INST(0x5c60000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmnmx");
    INST(0x5c58000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fadd");
    INST(0x5c50000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmnmx");
    INST(0x5c48000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shl");
    INST(0x5c40000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop");
    INST(0x5c38000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imul");
    INST(0x5c30000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "flo");
    INST(0x5c28000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shr");
    INST(0x5c20000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imnmx");
    INST(0x5c18000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iscadd");
    INST(0x5c10000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd");
    INST(0x5c08000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "popc");
    INST(0x5c00000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfe");
    INST(0x5bf8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shf");
    INST(0x5bf0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfi");
    INST(0x5be0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop3");
    INST(0x5bd8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lea");
    INST(0x5bd0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lea");
    INST(0x5bc0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "prmt");
    INST(0x5bb0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fsetp");
    INST(0x5ba0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fcmp");
    INST(0x5b80000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dsetp");
    INST(0x5b70000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dfma");
    INST(0x5b60000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "isetp");
    INST(0x5b50000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iset");
    INST(0x5b40000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "icmp");
    INST(0x5b00000000000000, 0xffc0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "xmad");
    INST(0x5a80000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imadsp");
    INST(0x5a00000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imad");
    INST(0x5980000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ffma");
    INST(0x5900000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dset");
    INST(0x5800000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fset");
    INST(0x5700000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vshl");
    INST(0x5600000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vshr");
    INST(0x5400000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vabsdiff");
    INST(0x53f8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "idp");
    INST(0x53f0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfi");
    INST(0x53d8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "idp");
    INST(0x53c0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "prmt");
    INST(0x53a0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fcmp");
    INST(0x5370000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dfma");
    INST(0x5340000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "icmp");
    INST(0x5280000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imadsp");
    INST(0x5200000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imad");
    INST(0x5180000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ffma");
    INST(0x5100000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "xmad");
    INST(0x50f8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fswzadd");
    INST(0x50f0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vsetp");
    INST(0x50e0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vote");
    INST(0x50d8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vote");
    INST(0x50d0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lepc");
    INST(0x50c8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cs2r");
    INST(0x50b0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "nop");
    INST(0x50a0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "csetp");
    INST(0x5098000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "cset");
    INST(0x5090000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "psetp");
    INST(0x5088000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "pset");
    INST(0x5080000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mufu");
    INST(0x5000000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vabsdiff4");
    INST(0x4e00000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "xmad");
    INST(0x4cf0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "r2p");
    INST(0x4ce8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "p2r");
    INST(0x4ce0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2i");
    INST(0x4cc0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd3");
    INST(0x4cb8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2f");
    INST(0x4cb0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2i");
    INST(0x4ca8000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2f");
    INST(0x4ca0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sel");
    INST(0x4c98000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mov");
    INST(0x4c90000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "rro");
    INST(0x4c88000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fchk");
    INST(0x4c80000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmul");
    INST(0x4c70000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dadd");
    INST(0x4c68000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmul");
    INST(0x4c60000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmnmx");
    INST(0x4c58000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fadd");
    INST(0x4c50000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmnmx");
    INST(0x4c48000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shl");
    INST(0x4c40000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop");
    INST(0x4c38000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imul");
    INST(0x4c30000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "flo");
    INST(0x4c28000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shr");
    INST(0x4c20000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imnmx");
    INST(0x4c18000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iscadd");
    INST(0x4c10000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd");
    INST(0x4c08000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "popc");
    INST(0x4c00000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfe");
    INST(0x4bf0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfi");
    INST(0x4bd0000000000000, 0xfff8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lea");
    INST(0x4bc0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "prmt");
    INST(0x4bb0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fsetp");
    INST(0x4ba0000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fcmp");
    INST(0x4b80000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dsetp");
    INST(0x4b70000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dfma");
    INST(0x4b60000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "isetp");
    INST(0x4b50000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iset");
    INST(0x4b40000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "icmp");
    INST(0x4a80000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imadsp");
    INST(0x4a00000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imad");
    INST(0x4980000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ffma");
    INST(0x4900000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dset");
    INST(0x4800000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fset");
    INST(0x4000000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vset");
    INST(0x3c00000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop3");
    INST(0x3a00000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vmnmx");
    INST(0x38f8000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shf");
    INST(0x38f0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "r2p");
    INST(0x38e8000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "p2r");
    INST(0x38e0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2i");
    INST(0x38c0000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd3");
    INST(0x38b8000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "i2f");
    INST(0x38b0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2i");
    INST(0x38a8000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2f");
    INST(0x38a0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "f2f");
    INST(0x3898000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mov");
    INST(0x3890000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "rro");
    INST(0x3888000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fchk");
    INST(0x3880000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmul");
    INST(0x3870000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dadd");
    INST(0x3868000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmul");
    INST(0x3860000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmnmx");
    INST(0x3858000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fadd");
    INST(0x3850000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dmnmx");
    INST(0x3848000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shl");
    INST(0x3840000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop");
    INST(0x3838000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imul");
    INST(0x3830000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "flo");
    INST(0x3828000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shr");
    INST(0x3820000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imnmx");
    INST(0x3818000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iscadd");
    INST(0x3810000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd");
    INST(0x3808000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "popc");
    INST(0x3800000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfe");
    INST(0x36f8000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "shf");
    INST(0x36f0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "bfi");
    INST(0x36d0000000000000, 0xfef8000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lea");
    INST(0x36c0000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "prmt");
    INST(0x36b0000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fsetp");
    INST(0x36a0000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fcmp");
    INST(0x3680000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dsetp");
    INST(0x3670000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dfma");
    INST(0x3660000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "isetp");
    INST(0x3650000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iset");
    INST(0x3640000000000000, 0xfef0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "icmp");
    INST(0x3600000000000000, 0xfec0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "xmad");
    INST(0x3480000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imadsp");
    INST(0x3400000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imad");
    INST(0x3280000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ffma");
    INST(0x3200000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "dset");
    INST(0x3000000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fset");
    INST(0x2c00000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hadd2_32i");
    INST(0x2a00000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hmul2_32i");
    INST(0x2800000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "hfma2_32i");
    INST(0x2000000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "vadd");
    INST(0x1f00000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imul32i");
    INST(0x1e00000000000000, 0xff00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fmul32i");
    INST(0x1d80000000000000, 0xff80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd32i");
    INST(0x1c00000000000000, 0xfe80000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iadd32i");
    INST(0x1800000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lea");
    INST(0x1400000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "iscadd32i");
    INST(0x1000000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "imad32i");
    INST(0x0c00000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "ffma32i");
    INST(0x0800000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "fadd32i");
    INST(0x0400000000000000, 0xfc00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop32i");
    INST(0x0200000000000000, 0xfe00000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "lop3");
    INST(0x0100000000000000, 0xfff0000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "mov32i");
    INST(0x0000000000000000, 0x8000000000000000)
    LOG_NOT_IMPLEMENTED(ShaderDecompiler, "sched");
    else {
        LOG_ERROR(ShaderDecompiler, "Unknown instruction 0x{:016x}", inst);
    }
}

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
