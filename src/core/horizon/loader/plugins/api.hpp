#pragma once

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

namespace hydra::horizon::loader::plugins::api {

enum class Function {
    GetApiVersion,
    CreateContext,
    DestroyContext,
    Query,
    GetStreamInterface,
    GetFileInterface,
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

struct StreamInterface {
  public:
    void Destroy(void* handle) const { destroy(handle); }

    u64 GetSeek(void* handle) const { return get_seek(handle); }

    void SeekTo(void* handle, u64 offset) const { seek_to(handle, offset); }

    void SeekBy(void* handle, u64 offset) const { seek_by(handle, offset); }

    u64 GetSize(void* handle) const { return get_size(handle); }

    void ReadRaw(void* handle, std::span<u8> buffer) const {
        read_raw(handle, Slice(buffer));
    }

  private:
    void (*destroy)(void*);
    u64 (*get_seek)(void*);
    void (*seek_to)(void*, u64);
    void (*seek_by)(void*, u64);
    u64 (*get_size)(void*);
    void (*read_raw)(void*, Slice<u8>);
};

typedef StreamInterface (*GetStreamInterfaceFnT)(void*);

struct FileInterface {
  public:
    void* Open(void* handle) const { return open(handle); }

    u64 GetSize(void* handle) const { return get_size(handle); }

  private:
    void* (*open)(void*);
    u64 (*get_size)(void*);
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

typedef FileInterface (*GetFileInterfaceFnT)(void*);

typedef ReturnValue<QueryResult, u64> (*QueryFnT)(void*, QueryType, Slice<u8>);

typedef void (*add_file)(void*, filesystem::Directory*, Slice<const char>,
                         void*);

enum class CreateLoaderFromFileResult : u32 {
    Success = 0,
    AllocationFailed = 1,
    UnsupportedFile = 2,
};

typedef ReturnValue<CreateLoaderFromFileResult, void*> (
    *CreateLoaderFromFileFnT)(void*, void*, add_file, void*, Slice<const char>);

typedef u32 (*DestroyLoaderFnT)(void*, void*);

} // namespace hydra::horizon::loader::plugins::api

ENABLE_ENUM_FORMATTING(
    hydra::horizon::loader::plugins::api::CreateContextResult, Success,
    "success", AllocationFailed, "allocation failed", InvalidOptions,
    "invalid options")

ENABLE_ENUM_FORMATTING(hydra::horizon::loader::plugins::api::QueryResult,
                       Success, "success", BufferTooSmall, "buffer too small")

ENABLE_ENUM_FORMATTING(
    hydra::horizon::loader::plugins::api::CreateLoaderFromFileResult, Success,
    "success", AllocationFailed, "allocation failed", UnsupportedFile,
    "unsupported file")
