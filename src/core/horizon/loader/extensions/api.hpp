#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::loader::extensions::api {

enum class Function {
    GetApiVersion,
    CreateContext,
    DestroyContext,
    Query,
    CreateLoaderFromFile,
    DestroyLoader,
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

typedef u32 (*DestroyContextFnT)(void*);

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

struct StreamInterface {
  public:
    u64 GetSeek() const { return get_seek(handle); }

    void SeekTo(u64 offset) const { seek_to(handle, offset); }

    void SeekBy(u64 offset) const { seek_by(handle, offset); }

    u64 GetSize() const { return get_size(handle); }

    void ReadRaw(std::span<u8> buffer) const {
        read_raw(handle, Slice(buffer));
    }

  private:
    void* handle;
    u64 (*get_seek)(void*);
    void (*seek_to)(void*, u64);
    void (*seek_by)(void*, u64);
    u64 (*get_size)(void*);
    void (*read_raw)(void*, Slice<u8>);
};

typedef void (*add_file)(filesystem::Directory*, Slice<const char>,
                         StreamInterface);

enum class CreateLoaderFromFileResult : u32 {
    Success = 0,
    AllocationFailed = 1,
    UnsupportedFile = 2,
};

typedef ReturnValue<CreateLoaderFromFileResult, void*> (
    *CreateLoaderFromFileFnT)(void*, add_file, void*, Slice<const char>);

typedef u32 (*DestroyLoaderFnT)(void*, void*);

} // namespace hydra::horizon::loader::extensions::api

ENABLE_ENUM_FORMATTING(
    hydra::horizon::loader::extensions::api::CreateContextResult, Success,
    "success", AllocationFailed, "allocation failed", InvalidOptions,
    "invalid options")

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::extensions::api::QueryResult,
                       Success, "success", BufferTooSmall, "buffer too small")

ENABLE_ENUM_FORMATTING(
    hydra::horizon::loader::extensions::api::CreateLoaderFromFileResult,
    Success, "success", AllocationFailed, "allocation failed", UnsupportedFile,
    "unsupported file")
