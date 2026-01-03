#include "core/horizon/loader/extensions/extension.hpp"

#include "core/horizon/loader/nx_loader.hpp"

namespace hydra::horizon::loader::extensions {

namespace {

// TODO: the name...
class StreamInterfaceStream : public io::IStream {
  public:
    StreamInterfaceStream(Extension& extension_, void* handle_)
        : extension{extension_}, handle{handle_} {}
    ~StreamInterfaceStream() override {
        extension.GetStreamInterface().Destroy(handle);
    }

    u64 GetSeek() const override {
        return extension.GetStreamInterface().GetSeek(handle);
    }
    void SeekTo(u64 seek) override {
        extension.GetStreamInterface().SeekTo(handle, seek);
    }
    void SeekBy(u64 offset) override {
        extension.GetStreamInterface().SeekBy(handle, offset);
    }

    u64 GetSize() const override {
        return extension.GetStreamInterface().GetSize(handle);
    }

    void ReadRaw(std::span<u8> buffer) override {
        extension.GetStreamInterface().ReadRaw(handle, buffer);
    }

  private:
    Extension& extension;
    void* handle;
};

class StreamFile : public filesystem::IFile {
  public:
    StreamFile(Extension& extension_, void* handle_)
        : extension{extension_}, handle{handle_} {}

    io::IStream* Open(filesystem::FileOpenFlags flags) {
        (void)flags;
        return new StreamInterfaceStream(
            extension, extension.GetFileInterface().Open(handle));
    }

    usize GetSize() { return extension.GetFileInterface().GetSize(handle); }

  private:
    Extension& extension;
    void* handle;
};

class Loader : public NxLoader {
  public:
    Loader(Extension& extension_, void* handle_,
           const filesystem::Directory& dir)
        : NxLoader(dir), extension{extension_}, handle{handle_} {}
    ~Loader() {
        // HACK
        (void)extension;
        (void)handle;
        // extension.DestroyLoader(handle);
    }

  private:
    Extension& extension;
    void* handle;
};

void AddFile(void* extension, filesystem::Directory* dir,
             api::Slice<const char> path, void* handle) {
    const std::string_view path_str(path.data, path.size);
    const auto res = dir->AddEntry(
        path_str,
        new StreamFile(*reinterpret_cast<Extension*>(extension), handle), true);
    ASSERT(res == filesystem::FsResult::Success, Loader,
           "Failed to add file to \"{}\": {}", path_str, res);
}

} // namespace

Extension::Extension(const std::string& path) {
    library = dlopen(path.data(), RTLD_LAZY);
    ASSERT_THROWING(library, Loader, Error::LoadFailed,
                    "Failed to load extension at path {}: {}", path, dlerror());

    // API version
    ASSERT_THROWING(GetApiVersion() == 1, Loader, Error::InvalidApiVersion,
                    "Invalid API version");

    // Context
    context = CreateContext({}); // TODO: options
    ASSERT_THROWING(context, Loader, Error::ContextCreationFailed,
                    "Failed to create context");

    // Info
    std::array<u8, 1024> buffer;
    name = Query(api::QueryType::Name, buffer);
    display_version = Query(api::QueryType::DisplayVersion, buffer);
    supported_formats = split<std::string>(
        Query(api::QueryType::SupportedFormats, buffer), ',');

    // Interfaces
    stream_interface = GetStreamInterface();
    file_interface = GetFileInterface();

    LOG_INFO(
        Loader,
        "Loaded extension \"{}\" (version: {}, formats: {}) at path \"{}\"",
        name, display_version, supported_formats, path);
}

Extension::~Extension() {
    DestroyContext();
    dlclose(library);
}

NxLoader* Extension::Load(std::string_view path) {
    const auto root_dir = new filesystem::Directory();
    const auto handle = CreateLoaderFromFile(root_dir, path);
    return new Loader(*this, handle, *root_dir);
}

u64 Extension::GetApiVersion() {
    const auto get_api_version =
        GetFunction<api::Function::GetApiVersion, api::GetApiVersionFnT>();
    return get_api_version();
}

void* Extension::CreateContext(std::span<std::string_view> options) {
    const auto create_context =
        GetFunction<api::Function::CreateContext, api::CreateContextFnT>();
    std::vector<api::Slice<const char>> options_vec(options.size());
    for (size_t i = 0; i < options.size(); ++i) {
        options_vec[i] =
            api::Slice<const char>(options[i].data(), options[i].size());
    }
    const auto ret = create_context(api::Slice(std::span(options_vec)));
    if (ret.res != api::CreateContextResult::Success) {
        throw ret.res;
    }

    return ret.value;
}

void Extension::DestroyContext() {
    const auto destroy_context =
        GetFunction<api::Function::DestroyContext, api::DestroyContextFnT>();
    destroy_context(context);
}

std::string Extension::Query(api::QueryType what, std::span<u8> buffer) {
    const auto query = GetFunction<api::Function::Query, api::QueryFnT>();
    const auto ret = query(context, what, api::Slice(buffer));
    switch (ret.res) {
    case api::QueryResult::Success:
        break;
    case api::QueryResult::BufferTooSmall:
        LOG_FATAL(Loader, "Buffer too small");
    default:
        LOG_FATAL(Loader, "Unknown query result: {}", ret.res);
    }

    return std::string(buffer.begin(),
                       buffer.begin() + static_cast<i32>(ret.value));
}

api::StreamInterface Extension::GetStreamInterface() {
    const auto get_stream_interface =
        GetFunction<api::Function::GetStreamInterface,
                    api::GetStreamInterfaceFnT>();
    return get_stream_interface(context);
}

api::FileInterface Extension::GetFileInterface() {
    const auto get_file_interface = GetFunction<api::Function::GetFileInterface,
                                                api::GetFileInterfaceFnT>();
    return get_file_interface(context);
}

void* Extension::CreateLoaderFromFile(filesystem::Directory* root_dir,
                                      std::string_view path) {
    const auto create_loader_from_file =
        GetFunction<api::Function::CreateLoaderFromFile,
                    api::CreateLoaderFromFileFnT>();
    const auto ret = create_loader_from_file(context, this, AddFile, root_dir,
                                             api::Slice(std::span(path)));
    if (ret.res != api::CreateLoaderFromFileResult::Success) {
        throw ret.res;
    }

    return ret.value;
}

void Extension::DestroyLoader(void* loader) {
    const auto destroy_loader =
        GetFunction<api::Function::DestroyLoader, api::DestroyLoaderFnT>();
    destroy_loader(context, loader);
}

} // namespace hydra::horizon::loader::extensions
