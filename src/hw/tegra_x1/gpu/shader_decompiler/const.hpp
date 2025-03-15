#pragma once

#include "hw/tegra_x1/gpu/const.hpp"

namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler {

enum class Operation {
    Nop,
    Phi,
    Union,      // unify a new definition and several source values
    Split,      // $r0d -> { $r0, $r1 } ($r0d and $r0/$r1 will be coalesced)
    Merge,      // opposite of split, e.g. combine 2 32 bit into a 64 bit value
    Constraint, // copy values into consecutive registers
    Mov,        // simple copy, no modifiers allowed
    Load,
    Store,
    Add, // NOTE: add u64 + u32 is legal for targets w/o 64-bit integer adds
    Sub,
    Mul,
    Div,
    Mod,
    Mad,
    Fma,
    Sad, // abs(src0 - src1) + src2
    Shladd,
    // extended multiply-add (GM107+), does a lot of things.
    // see envytools for detailed documentation
    Xmad,
    Abs,
    Neg,
    Not,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Max,
    Min,
    Sat, // CLAMP(f32, 0.0, 1.0)
    Ceil,
    Floor,
    Trunc,
    Cvt,
    SetAnd, // dst = (src0 CMP src1) & src2
    SetOr,
    SetXor,
    Set,
    Selp, // dst = src2 ? src0 : src1
    Slct, // dst = (src2 CMP 0) ? src0 : src1
    Rcp,
    Rsq,
    Lg2,
    Sin,
    Cos,
    Ex2,
    Exp, // exponential (base M_E)
    Log, // natural logarithm
    Presin,
    Preex2,
    Sqrt,
    Pow,
    Bra,
    Call,
    Ret,
    Cont,
    Break,
    Preret,
    Precont,
    Prebreak,
    Brkpt,  // breakpoint (not related to loops)
    JoinAt, // push control flow convergence point
    Join,   // converge
    Discard,
    Exit,
    MemBar, // memory barrier (mfence, lfence, sfence)
    VFetch, // indirection 0 in attribute space, indirection 1 is vertex base
    PFetch, // fetch base address of vertex src0 (immediate) [+ src1]
    AFetch, // fetch base address of shader input (a[%r1+0x10])
    Export,
    LInterp,
    PInterp,
    Emit,    // emit vertex
    Restart, // restart primitive
    Tex,
    TxB,     // texture bias
    TxL,     // texure lod
    TxF,     // texel fetch
    TxQ,     // texture size query
    TxQ,     // texture derivatives
    TxG,     // texture gather
    TxLQ,    // texture query lod
    TexCSAA, // texture op for coverage sampling
    TexPrep, // turn cube map array into 2d array coordinates
    SuLdB,   // surface load (raw)
    SuLdP,   // surface load (formatted)
    SuStB,   // surface store (raw)
    SuStP,   // surface store (formatted)
    SuRedB,
    SuRedP,  // surface reduction (atomic op)
    SuLEA,   // surface load effective address
    SuBfM,   // surface bitfield manipulation
    SuClamp, // clamp surface coordinates
    SuEAU,   // surface effective address
    SuQ,     // surface query
    MadSP,   // special integer multiply-add
    TexBar,  // texture dependency barrier
    DfDX,
    DfDY,
    RdSV,  // read system value
    WrSV,  // write system value
    PixLd, // get info about raster object or surfaces
    QuadOp,
    QuadOn,
    QuadPop,
    PopCnt, // bitcount(src0 & src1)
    InsBf,  // insert first src1[8:15] bits of src0 into src2 at src1[0:7]
    ExtBf,  // place bits [K,K+N) of src0 into dst, src1 = 0xNNKK
    BFind,  // find highest/lowest set bit
    PerMT,  // dst = bytes from src2,src0 selected by src1 (nvc0's src order)
    Atom,
    Bar,  // execution barrier, sources = { id, thread count, predicate }
    Vadd, // byte/word vector operations
    Vavg,
    VMin,
    VMax,
    VSad,
    VSet,
    VShr,
    VShl,
    VSel,
    Cctl, // cache control
    Shfl, // warp shuffle
    Vote,
    BufQ, // buffer query

    Count,
};

} // namespace Hydra::HW::TegraX1::GPU::ShaderDecompiler
