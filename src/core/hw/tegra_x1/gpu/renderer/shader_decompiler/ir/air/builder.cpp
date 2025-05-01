#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/ir/air/builder.hpp"

#include "core/hw/tegra_x1/gpu/renderer/shader_cache.hpp"
#include "core/hw/tegra_x1/gpu/renderer/shader_decompiler/analyzer/analyzer.hpp"

namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR {

// TODO: make this configurable
static constexpr bool FAST_MATH_ENABLED = true;

Builder::Builder(const Analyzer::Analyzer& analyzer, const ShaderType type,
                 const GuestShaderState& state, std::vector<u8>& out_code,
                 ResourceMapping& out_resource_mapping)
    : BuilderBase(analyzer, type, state, out_code, out_resource_mapping),
      module("default", context), types(context) {}

void Builder::Start() {
    // TODO: diagnostics handler

    // TODO: move this to the API
    module.setSourceFileName("airconv_generated.metal");
    module.setTargetTriple("air64-apple-macosx14.0.0");
    module.setDataLayout(
        "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:"
        "64-f32:32:32-f64:"
        "64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:"
        "64-v96:128:128-"
        "v128:128:128-v192:256:256-v256:256:256-v512:512:512-"
        "v1024:1024:1024-n8:"
        "16:32");
    module.setSDKVersion(llvm::VersionTuple(14, 0));
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Error, "wchar_size", 4);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max, "frame-pointer",
                         2);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max,
                         "air.max_device_buffers", 31);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max,
                         "air.max_constant_buffers", 31);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max,
                         "air.max_threadgroup_buffers", 31);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max, "air.max_textures",
                         128);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max,
                         "air.max_read_write_textures", 8);
    module.addModuleFlag(llvm::Module::ModFlagBehavior::Max, "air.max_samplers",
                         16);

    auto createUnsignedInteger = [&](uint32_t s) {
        return llvm::ConstantAsMetadata::get(
            llvm::ConstantInt::get(context, llvm::APInt{32, s, false}));
    };
    auto createString = [&](auto s) { return llvm::MDString::get(context, s); };

    auto airVersion = module.getOrInsertNamedMetadata("air.version");
    airVersion->addOperand(llvm::MDTuple::get(
        context, {createUnsignedInteger(2), createUnsignedInteger(6),
                  createUnsignedInteger(0)}));
    auto airLangVersion =
        module.getOrInsertNamedMetadata("air.language_version");
    airLangVersion->addOperand(llvm::MDTuple::get(
        context, {createString("Metal"), createUnsignedInteger(3),
                  createUnsignedInteger(0), createUnsignedInteger(0)}));

    auto airCompileOptions =
        module.getOrInsertNamedMetadata("air.compile_options");
    airCompileOptions->addOperand(llvm::MDTuple::get(
        context, {createString("air.compile.denorms_disable")}));
    airCompileOptions->addOperand(llvm::MDTuple::get(
        context,
        {FAST_MATH_ENABLED ? createString("air.compile.fast_math_enable")
                           : createString("air.compile.fast_math_disable")}));
    airCompileOptions->addOperand(llvm::MDTuple::get(
        context, {createString("air.compile.framebuffer_fetch_enable")}));

    // Builder
    luft::FunctionSignatureBuilder signature_builder;
    // TODO: define inputs and outputs
    auto [function, meta] =
        signature_builder.CreateFunction("main_", context, module, 0, false);

    auto entry_bb = llvm::BasicBlock::Create(context, "entry", function);
    builder = new llvm::IRBuilder<>(entry_bb);
    builder->getFastMathFlags().setFast(FAST_MATH_ENABLED);

    std::string stage_name;
    switch (type) {
    case ShaderType::Vertex:
        stage_name = "air.vertex";
        break;
    case ShaderType::Fragment:
        stage_name = "air.fragment";
        break;
    default:
        stage_name = "air.unknown";
        break;
    }
    module.getOrInsertNamedMetadata(stage_name)->addOperand(meta);
}

void Builder::Finish() {
    delete builder;

    // TODO: enable optimizations
    // RunOptimizationPasses(llvm::OptimizationLevel::O2);

    // TODO: remove this
    module.print(llvm::outs(), nullptr);

    // TODO: write to the output code
    llvm::SmallVector<char, 0> vec;
    llvm::raw_svector_ostream os(vec);
    luft::metallib::MetallibWriter writer;
    writer.Write(module, os);
}

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

void Builder::RunOptimizationPasses(llvm::OptimizationLevel opt) {
    /*
            if (!llvm_overwrite.test_and_set()) {
                auto Map = cl::getRegisteredOptions();
                auto InfiniteLoopThreshold =
                    Map["instcombine-infinite-loop-threshold"];
                if (InfiniteLoopThreshold) {
                    reinterpret_cast<cl::opt<unsigned>*>(InfiniteLoopThreshold)
                        ->setValue(1000);
                }
            }
            */

    // Create the analysis managers.
    // These must be declared in this order so that they are destroyed in
    // the correct order due to inter-analysis-manager references.
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    // Create the new pass manager builder.
    // Take a look at the PassBuilder constructor parameters for more
    // customization, e.g. specifying a TargetMachine or various debugging
    // options.
    llvm::PassBuilder PB;

    // Register all the basic analyses with the managers.
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(opt);

    llvm::FunctionPassManager FPM;
    FPM.addPass(llvm::ScalarizerPass());

    MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
    MPM.addPass(llvm::VerifierPass());

    // Optimize the IR!
    MPM.run(module, MAM);
}

} // namespace Hydra::HW::TegraX1::GPU::Renderer::ShaderDecompiler::IR::AIR
