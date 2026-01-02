#pragma once

#include "core/horizon/loader/extension.hpp"

namespace hydra::horizon::loader {

class ExtensionManager {
  public:
    static ExtensionManager& GetInstance() {
        static ExtensionManager instance;
        return instance;
    }

    ExtensionManager();

    Extension* FindExtensionForFormat(std::string_view format);

  private:
    std::vector<Extension> extensions;
};

} // namespace hydra::horizon::loader
