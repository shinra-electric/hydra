#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/air/builder.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_cache.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR {

void Builder::Start() { LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler); }

void Builder::Finish() { LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler); }

void Builder::OpExit() { LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler); }

void Builder::OpMove(reg_t dst, Operand src) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpAdd(Operand dst, Operand src1, Operand src2) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpMultiply(Operand dst, Operand src1, Operand src2) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpFloatFma(reg_t dst, reg_t src1, Operand src2, Operand src3) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpShiftLeft(reg_t dst, reg_t src, u32 shift) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpMathFunction(MathFunc func, reg_t dst, reg_t src) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpLoad(reg_t dst, Operand src) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpStore(AMem dst, reg_t src) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpInterpolate(reg_t dst, AMem src) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

void Builder::OpTextureSample(reg_t dst0, reg_t dst1, u32 const_buffer_index,
                              reg_t coords_x, reg_t coords_y) {
    LOG_FUNC_NOT_IMPLEMENTED(ShaderDecompiler);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR
