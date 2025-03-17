#include "hw/tegra_x1/gpu/engines/3d.hpp"

#include "hw/tegra_x1/gpu/gpu.hpp"
#include "hw/tegra_x1/gpu/macro/interpreter/driver.hpp"
#include "hw/tegra_x1/gpu/renderer/render_pass_base.hpp"
#include "hw/tegra_x1/gpu/renderer/texture_base.hpp"

namespace Hydra::HW::TegraX1::GPU::Engines {

namespace {

u32 get_image_handle(u32 handle) { return extract_bits<u32, 0, 20>(handle); }
u32 get_sampler_handle(u32 handle) { return extract_bits<u32, 20, 12>(handle); }

} // namespace

DEFINE_METHOD_TABLE(ThreeD, 0x45, 1, LoadMmeInstructionRamPointer, u32, 0x46, 1,
                    LoadMmeInstructionRam, u32, 0x47, 1,
                    LoadMmeStartAddressRamPointer, u32, 0x48, 1,
                    LoadMmeStartAddressRam, u32, 0x35e, 1, DrawVertexArray, u32,
                    0x674, 1, ClearBuffer, ClearBufferData, 0x8c4, 1,
                    FirmwareCall4, u32, 0x8e4, 16, LoadConstBuffer, u32)

SINGLETON_DEFINE_GET_INSTANCE(ThreeD, Engines, "3D engine")

ThreeD::ThreeD() {
    SINGLETON_SET_INSTANCE(Engines, "3D engine");

    // TODO: choose based on Macro backend
    {
        macro_driver = new Macro::Interpreter::Driver(this);
    }
}

ThreeD::~ThreeD() {
    delete macro_driver;

    SINGLETON_UNSET_INSTANCE();
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

void ThreeD::LoadMmeInstructionRamPointer(const u32 index, const u32 ptr) {
    macro_driver->LoadInstructionRamPointer(ptr);
}

void ThreeD::LoadMmeInstructionRam(const u32 index, const u32 data) {
    macro_driver->LoadInstructionRam(data);
}

void ThreeD::LoadMmeStartAddressRamPointer(const u32 index, const u32 ptr) {
    macro_driver->LoadStartAddressRamPointer(ptr);
}

void ThreeD::LoadMmeStartAddressRam(const u32 index, const u32 data) {
    macro_driver->LoadStartAddressRam(data);
}

void ThreeD::DrawVertexArray(const u32 index, const u32 count) {
    RENDERER->BindRenderPass(GetRenderPass());

    RENDERER->BindPipeline(GetPipeline());
    // TODO: viewport and scissor

    for (u32 i = 0; i < VERTEX_ARRAY_COUNT; i++) {
        const auto& vertex_array = regs.vertex_arrays[i];
        if (!vertex_array.config.enable)
            continue;

        const auto buffer = GetVertexBuffer(i, count - 1);
        if (!buffer)
            continue;

        RENDERER->BindVertexBuffer(buffer, i);
    }

    // Constant buffer
    const uptr const_buffer_gpu_addr =
        make_addr(regs.const_buffer_selector_lo, regs.const_buffer_selector_hi);
    if (const_buffer_gpu_addr != 0x0) {
        const auto const_buffer =
            GPU::GetInstance().GetGPUMMU().Load<GraphicsDriverCbuf>(
                const_buffer_gpu_addr);

        const auto tex_header_pool_gpu_addr =
            make_addr(regs.tex_header_pool_lo, regs.tex_header_pool_hi);
        // TODO: check if the address is valid
        const auto tex_header_pool = reinterpret_cast<TextureImageControl*>(
            GPU::GetInstance().GetGPUMMU().UnmapAddr(tex_header_pool_gpu_addr));

        // TODO: configure all stages
        ConfigureShaderStage(ShaderStage::VertexB, Renderer::ShaderType::Vertex,
                             const_buffer, tex_header_pool);
        ConfigureShaderStage(ShaderStage::Fragment,
                             Renderer::ShaderType::Fragment, const_buffer,
                             tex_header_pool);
    }

    RENDERER->Draw(regs.begin.primitive_type, regs.vertex_array_start, count);
}

void ThreeD::ClearBuffer(const u32 index, const ClearBufferData data) {
    LOG_DEBUG(GPU,
              "Depth: {}, stencil: {}, color mask: 0x{:x}, target id: {}, "
              "layer id: {}",
              data.depth, data.stencil, data.color_mask, data.target_id,
              data.layer_id);

    // Deferred clear
    // TODO: implement

    // Regular clear
    RENDERER->BindRenderPass(GetRenderPass());

    if (data.color_mask != 0x0)
        RENDERER->ClearColor(data.target_id, data.layer_id, data.color_mask,
                             regs.clear_color);

    if (data.depth)
        LOG_NOT_IMPLEMENTED(Engines, "Depth clears");

    if (data.stencil)
        LOG_NOT_IMPLEMENTED(Engines, "Stencil clears");

    // Texture
    /*
    const auto texture = GetColorTargetTexture(data.target_id);
    const auto& texture_descriptor = texture->GetDescriptor();

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
    */
}

void ThreeD::FirmwareCall4(const u32 index, const u32 data) {
    LOG_NOT_IMPLEMENTED(Engines, "Firmware call 4");

    // TODO: find out what this does
    regs.mme_firmware_args[0] = 0x1;
}

void ThreeD::LoadConstBuffer(const u32 index, const u32 data) {
    const uptr const_buffer_gpu_addr =
        make_addr(regs.const_buffer_selector_lo, regs.const_buffer_selector_hi);
    const uptr gpu_addr =
        const_buffer_gpu_addr + regs.load_const_buffer_offset + index;
    uptr ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr);

    *reinterpret_cast<u32*>(ptr) = data;
}

Renderer::BufferBase* ThreeD::GetVertexBuffer(u32 vertex_array_index,
                                              u32 max_vertex) const {
    const auto& vertex_array = regs.vertex_arrays[vertex_array_index];

    const Renderer::BufferDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(
            make_addr(vertex_array.addr_lo, vertex_array.addr_hi)),
        .size = (max_vertex + 1) * vertex_array.config.stride,
    };

    return GPU::GetInstance().GetBufferCache().Find(descriptor);
}

Renderer::TextureBase*
ThreeD::GetTexture(const TextureImageControl& tic) const {
    const uptr gpu_addr = make_addr(tic.address_lo, tic.address_hi);
    if (gpu_addr == 0x0)
        return nullptr;

    const Renderer::TextureDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr),
        .format = Renderer::to_texture_format(tic.format_word.image_format),
        .kind = NvKind::Generic_16BX2, // TODO: correct?
        .width = static_cast<usize>(tic.width_minus_one + 1),
        .height = static_cast<usize>(tic.height_minus_one + 1),
        .block_height_log2 = tic.tile_height_gobs_log2, // TODO: correct?
        .stride = static_cast<usize>((tic.width_minus_one + 1) * 4), // HACK
    };

    return GPU::GetInstance().GetTextureCache().Find(descriptor);
}

Renderer::TextureBase*
ThreeD::GetColorTargetTexture(u32 render_target_index) const {
    const auto& render_target = regs.color_targets[render_target_index];

    const auto gpu_addr =
        make_addr(render_target.addr_lo, render_target.addr_hi);
    if (gpu_addr == 0x0)
        return nullptr;

    const Renderer::TextureDescriptor descriptor{
        .ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr),
        .format = Renderer::to_texture_format(render_target.format),
        .kind = NvKind::Generic_16BX2, // TODO: correct?
        .width = render_target.width,
        .height = render_target.height,
        .block_height_log2 = render_target.tile_mode.height, // TODO: correct?
        .stride = render_target.width * 4,                   // HACK
    };

    return GPU::GetInstance().GetTextureCache().Find(descriptor);
}

Renderer::RenderPassBase* ThreeD::GetRenderPass() const {
    Renderer::RenderPassDescriptor descriptor;

    // Color targets
    for (u32 i = 0; i < COLOR_TARGET_COUNT; i++) {
        descriptor.color_targets[i] = {
            .texture = GetColorTargetTexture(i),
        };
    }

    // Depth stencil target
    descriptor.depth_stencil_target = {
        .texture = nullptr, // TODO
        // TODO: stencil
    };

    return GPU::GetInstance().GetRenderPassCache().Find(descriptor);
}

Renderer::ShaderBase* ThreeD::GetShader(ShaderStage stage) const {
    const auto& program = regs.shader_programs[usize(stage)];
    if (!program.config.enable)
        return nullptr;

    uptr gpu_addr = make_addr(regs.shader_program_region_lo,
                              regs.shader_program_region_hi) +
                    program.offset;
    uptr ptr = GPU::GetInstance().GetGPUMMU().UnmapAddr(gpu_addr);

    GuestShaderDescriptor descriptor{
        .stage = stage,
        .code_ptr = ptr,
    };

    // Vertex attribute states
    for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
        descriptor.state.vertex_attrib_states[i] = regs.vertex_attrib_states[i];
    }

    // Color target formats
    for (u32 i = 0; i < COLOR_TARGET_COUNT; i++) {
        const auto& render_target = regs.color_targets[i];
        const auto addr =
            make_addr(render_target.addr_lo, render_target.addr_hi);
        if (addr == 0x0)
            continue;

        descriptor.state.color_target_formats[i] =
            Renderer::to_texture_format(regs.color_targets[i].format);
    }

    return GPU::GetInstance().GetShaderCache().Find(descriptor);
}

Renderer::PipelineBase* ThreeD::GetPipeline() const {
    Renderer::PipelineDescriptor descriptor;

    // Shaders
    for (u32 shader_stage = 0; shader_stage < usize(ShaderStage::Count);
         shader_stage++) {
        ShaderStage stage = ShaderStage(shader_stage);
        auto renderer_shader_type = to_renderer_shader_type(stage);

        // HACK
        if (renderer_shader_type == Renderer::ShaderType::Count)
            continue;

        descriptor.shaders[u32(renderer_shader_type)] = GetShader(stage);
    }

    // Vertex state

    // Vertex attribute states
    for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
        descriptor.vertex_state.vertex_attrib_states[i] =
            regs.vertex_attrib_states[i];
    }

    // Vertex arrays
    for (u32 i = 0; i < VERTEX_ARRAY_COUNT; i++) {
        const auto& vertex_array = regs.vertex_arrays[i];
        descriptor.vertex_state.vertex_arrays[i] = {
            .enable = vertex_array.config.enable,
            .stride = vertex_array.config.stride,
            .is_per_instance = regs.is_vertex_array_per_instance[i].enable,
            .divisor = vertex_array.divisor,
        };
    }

    return GPU::GetInstance().GetPipelineCache().Find(descriptor);
}

void ThreeD::ConfigureShaderStage(const ShaderStage stage,
                                  const Renderer::ShaderType type,
                                  const GraphicsDriverCbuf& const_buffer,
                                  const TextureImageControl* tex_header_pool) {
    const u32 stage_index = static_cast<u32>(stage) -
                            1; // 1 is subtracted, because VertexA is skipped

    // TODO: how are uniform buffers handled?
    // TODO: storage buffers

    // Textures
    for (u32 i = 0; i < TEXTURE_BINDING_COUNT; i++) {
        // TODO: check if the texture is needed by the shader

        const auto texture_handle = const_buffer.data[stage_index].textures[i];

        // Image
        const auto image_handle = get_image_handle(texture_handle);
        const auto& tic = tex_header_pool[image_handle];
        const auto texture = GetTexture(tic);
        if (texture)
            RENDERER->BindTexture(texture, type, i);
    }

    // TODO: images
}

} // namespace Hydra::HW::TegraX1::GPU::Engines
