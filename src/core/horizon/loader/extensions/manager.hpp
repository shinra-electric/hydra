#pragma once

#include "core/horizon/loader/extensions/extension.hpp"

namespace hydra::horizon::loader::extensions {

class Manager {
  public:
    static Manager& GetInstance() {
        static Manager instance;
        return instance;
    }

    Manager();

    Extension* FindExtensionForFormat(std::string_view format);

  private:
    std::vector<Extension> extensions;
};

} // namespace hydra::horizon::loader::extensions
