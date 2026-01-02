#pragma once

namespace hydra::horizon::loader::api {

enum class Function {
    Query,
};

template <typename Result, typename T>
struct ReturnValue {
    Result res;
    T value;
};

enum class QueryType : u32 {
    Name = 0,
    DisplayVersion = 1,
    SupportedFormats = 2,
};

enum class QueryResult : u32 {
    Success = 0,
    BufferTooSmall = 1,
};

typedef ReturnValue<QueryResult, u64> (*QueryFnT)(QueryType, u8*, u64);

} // namespace hydra::horizon::loader::api

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::api::QueryResult, Success,
                       "success", BufferTooSmall, "buffer too small")
