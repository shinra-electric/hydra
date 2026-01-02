#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::loader::extensions::api {

enum class Function {
    GetApiVersion,
    CreateContext,
    Query,
    LoadFile,
};

template <typename Result, typename T>
struct ReturnValue {
    Result res;
    T value;
};

template <typename T>
struct Slice {
    T* data;
    u64 size;

    Slice() : data{nullptr}, size{0} {}
    Slice(T* data_, u64 size_) : data{data_}, size{size_} {}
    Slice(std::span<T> span) : data{span.data()}, size{span.size()} {}
};

typedef u32 (*GetApiVersionFnT)();

enum class CreateContextResult : u32 {
    Success = 0,
    AllocationFailed = 1,
    InvalidOptions = 2,
};

typedef ReturnValue<CreateContextResult, void*> (*CreateContextFnT)(
    Slice<Slice<const char>>);

enum class QueryType : u32 {
    Name = 0,
    DisplayVersion = 1,
    SupportedFormats = 2,
};

enum class QueryResult : u32 {
    Success = 0,
    BufferTooSmall = 1,
};

typedef ReturnValue<QueryResult, u64> (*QueryFnT)(void*, QueryType, Slice<u8>);

struct Stream {
    void* handle;
    u64 (*get_seek)(Stream*);
    void (*seek_to)(Stream*, u64);
    void (*seek_by)(Stream*, u64);
    u64 (*get_size)(Stream*);
    void (*read_raw)(Stream*, Slice<u8>);
};

struct DirectoryFunctions {
    void (*add_file)(filesystem::Directory*, Slice<char>, Stream);
    filesystem::Directory* (*add_directory)(filesystem::Directory*,
                                            Slice<char>);
};

enum class LoadFileResult : u32 {
    Success = 0,
    UnsupportedFile = 1,
};

typedef ReturnValue<LoadFileResult, u64> (*LoadFileFnT)(void*,
                                                        DirectoryFunctions,
                                                        void*, Slice<char>);

} // namespace hydra::horizon::loader::extensions::api

ENABLE_ENUM_FORMATTING(
    hydra::horizon::loader::extensions::api::CreateContextResult, Success,
    "success", AllocationFailed, "allocation failed", InvalidOptions,
    "invalid options")

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::extensions::api::QueryResult,
                       Success, "success", BufferTooSmall, "buffer too small")

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::extensions::api::LoadFileResult,
                       Success, "success", UnsupportedFile, "unsupported file")
