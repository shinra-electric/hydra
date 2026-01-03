#pragma once

#include <dlfcn.h>

#include "core/horizon/loader/extensions/api.hpp"

namespace hydra::horizon::filesystem {
class Directory;
}

namespace hydra::horizon::loader {
class NxLoader;
}

namespace hydra::horizon::loader::extensions {

class Extension {
    friend class Manager;

  public:
    enum class Error {
        LoadFailed,
        InvalidApiVersion,
        ContextCreationFailed,
    };

    Extension(const std::string& path);
    ~Extension();

    NxLoader* Load(std::string_view path);

    // API
    u64 GetApiVersion();
    void* CreateContext(std::span<std::string_view> options);
    void DestroyContext();
    std::string Query(api::QueryType what, std::span<u8> buffer);
    void* CreateLoaderFromFile(filesystem::Directory* root_dir,
                               std::string_view path);
    void DestroyLoader(void* loader);

  private:
    void* library;
    std::array<void*, static_cast<size_t>(api::Function::DestroyLoader) + 1>
        functions = {nullptr};

    // Context
    void* context;
    std::string name;
    std::string display_version;
    std::vector<std::string> supported_formats;

    // Helpers
    enum class GetFunctionError {
        SymbolNotFound,
    };

    template <api::Function api_func, typename T>
    T GetFunction() {
        auto& func = functions[static_cast<size_t>(api_func)];
        if (!func) {
            std::string_view symbol_name;
            switch (api_func) {
            case api::Function::GetApiVersion:
                symbol_name = "hydra_ext_get_api_version";
                break;
            case api::Function::CreateContext:
                symbol_name = "hydra_ext_create_context";
                break;
            case api::Function::DestroyContext:
                symbol_name = "hydra_ext_destroy_context";
                break;
            case api::Function::Query:
                symbol_name = "hydra_ext_query";
                break;
            case api::Function::CreateLoaderFromFile:
                symbol_name = "hydra_ext_create_loader_from_file";
                break;
            case api::Function::DestroyLoader:
                symbol_name = "hydra_ext_destroy_loader";
                break;
            }

            func = dlsym(library, symbol_name.data());
            ASSERT_THROWING(func != nullptr, Loader,
                            GetFunctionError::SymbolNotFound,
                            "Failed to load symbol \"{}\"", symbol_name);
        }

        return reinterpret_cast<T>(func);
    }
};

} // namespace hydra::horizon::loader::extensions
