#pragma once

#include <dlfcn.h>

#include "core/horizon/loader/extension_api.hpp"

namespace hydra::horizon::loader {

class Extension {
  public:
    Extension(std::string_view path);
    ~Extension();

    // API
    std::string Query(api::QueryType what, std::span<u8> buffer) {
        const auto query = GetFunction<api::Function::Query, api::QueryFnT>();
        const auto ret = query(what, buffer.data(), buffer.size());
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

  private:
    void* library;
    std::array<void*, static_cast<size_t>(api::Function::Query) + 1> functions =
        {nullptr};

    // Info
    std::string name;
    std::string display_version;
    std::vector<std::string> supported_formats;

    // Helpers
    template <api::Function api_func, typename T>
    T GetFunction() {
        auto& func = functions[static_cast<size_t>(api_func)];
        if (!func) {
            switch (api_func) {
            case api::Function::Query:
                func = dlsym(library, "hydra_query");
                break;
            }
        }

        return reinterpret_cast<T>(func);
    }
};

} // namespace hydra::horizon::loader
