#include "hw/tegra_x1/gpu/renderer/metal/pipeline.hpp"

#include "hw/tegra_x1/gpu/renderer/metal/maxwell_to_mtl.hpp"
#include "hw/tegra_x1/gpu/renderer/metal/renderer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::Metal {

Pipeline::Pipeline(const PipelineDescriptor& descriptor)
    : PipelineBase(descriptor) {
    MTL::RenderPipelineDescriptor* pipeline_descriptor =
        MTL::RenderPipelineDescriptor::alloc()->init();

    // Shaders
    // TODO: don't hardcode
    static std::string shader_source = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct Vertex {
            float3 position;
            float3 color;
        };

        constant Vertex vertices[3] = {
            Vertex{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            Vertex{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            Vertex{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        };

        struct MainVertexOut {
            float4 position [[position]];
            float3 color;
        };

        vertex MainVertexOut main_vertex(ushort vid [[vertex_id]]) {
            MainVertexOut out;
            out.position = float4(vertices[vid].position, 1.0);
            out.color = vertices[vid].color;

            return out;
        }

        fragment float4 main_fragment(MainVertexOut in [[stage_in]]) {
            return float4(in.color, 1.0);
        }
    )";
    auto library = CreateLibraryFromSource(Renderer::GetInstance().GetDevice(),
                                           shader_source);
    auto vertex_function = library->newFunction(ToNSString("main_vertex"));
    auto fragment_function = library->newFunction(ToNSString("main_fragment"));
    pipeline_descriptor->setVertexFunction(vertex_function);
    pipeline_descriptor->setFragmentFunction(fragment_function);
    vertex_function->release();
    fragment_function->release();

    // Vertex state

    // Vertex attribute states
    for (u32 i = 0; i < VERTEX_ATTRIB_COUNT; i++) {
        const auto& vertex_attrib_state =
            descriptor.vertex_state.vertex_attrib_states[i];
        if (vertex_attrib_state.type == Engines::VertexAttribType::None)
            continue;

        // HACK: how are attributes disabled?
        if (vertex_attrib_state.is_fixed)
            continue;

        ASSERT_DEBUG(i != 31, MetalRenderer,
                     "Vertex attribute 31 is not available on Metal");

        // LOG_DEBUG(MetalRenderer,
        //           "Buffer ID: {}, is fixed: {}, offset: 0x{:08x}",
        //           vertex_attrib_state.buffer_id,
        //           vertex_attrib_state.is_fixed, vertex_attrib_state.offset);

        auto vertex_attrib_descriptor =
            pipeline_descriptor->vertexDescriptor()->attributes()->object(i);
        vertex_attrib_descriptor->setBufferIndex(
            GetVertexBufferIndex(vertex_attrib_state.buffer_id));
        // TODO: is fixed
        vertex_attrib_descriptor->setOffset(vertex_attrib_state.offset);
        vertex_attrib_descriptor->setFormat(get_mtl_vertex_format(
            vertex_attrib_state.type, vertex_attrib_state.size,
            vertex_attrib_state.bgra));
    }

    // Vertex arrays
    for (u32 i = 0; i < VERTEX_ARRAY_COUNT; i++) {
        const auto& vertex_array = descriptor.vertex_state.vertex_arrays[i];
        if (!vertex_array.enable)
            continue;

        auto vertex_array_descriptor =
            pipeline_descriptor->vertexDescriptor()->layouts()->object(
                GetVertexBufferIndex(i));
        vertex_array_descriptor->setStride(vertex_array.stride);
        vertex_array_descriptor->setStepFunction(
            vertex_array.is_per_instance ? MTL::VertexStepFunctionPerInstance
                                         : MTL::VertexStepFunctionPerVertex);
        // TODO: divisor
        // vertex_array_descriptor->setStepRate(1);
    }

    // HACK
    pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(
        MTL::PixelFormatBGRA8Unorm);

    // Pipeline
    NS::Error* error;
    pipeline = Renderer::GetInstance().GetDevice()->newRenderPipelineState(
        pipeline_descriptor, &error);
    pipeline_descriptor->release();
    if (error) {
        LOG_ERROR(MetalRenderer, "Failed to create pipeline: {}",
                  error->localizedDescription()->utf8String());
        error->release(); // TODO: release?
        return;
    }
}

Pipeline::~Pipeline() { pipeline->release(); }

} // namespace Hydra::HW::TegraX1::GPU::Renderer::Metal
