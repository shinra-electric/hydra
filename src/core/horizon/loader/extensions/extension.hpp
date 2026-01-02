#pragma once

#include <dlfcn.h>

#include "core/horizon/loader/extensions/api.hpp"

namespace hydra::horizon::filesystem {
class Directory;
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

    // API
    u64 GetApiVersion() {
        const auto get_api_version =
            GetFunction<api::Function::GetApiVersion, api::GetApiVersionFnT>();
        return get_api_version();
    }

    void* CreateContext(std::span<std::string_view> options) {
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

    std::string Query(void* ctx, api::QueryType what, std::span<u8> buffer) {
        const auto query = GetFunction<api::Function::Query, api::QueryFnT>();
        const auto ret = query(ctx, what, api::Slice(buffer));
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

    // TODO: implement
    /*
    void LoadFile(void* ctx, filesystem::Directory* root_dir, std::string_view
    path) { const auto load_file = GetFunction<api::Function::LoadFile,
    api::LoadFileFnT>();

        // Functions
        DirectoryFunctions dir_funcs{
            // TODO
        };

        const auto ret = load_file(ctx, dir_funcs, root_dir, path);
        if (ret.res != api::LoadFileResult::Success) {
            throw ret.res;
        }
    }
    */

  private:
    void* library;
    std::array<void*, static_cast<size_t>(api::Function::LoadFile) + 1>
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
            case api::Function::Query:
                symbol_name = "hydra_ext_query";
                break;
            case api::Function::LoadFile:
                symbol_name = "hydra_ext_load_file";
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
