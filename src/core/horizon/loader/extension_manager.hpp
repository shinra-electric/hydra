#pragma once

namespace hydra::horizon::loader {

class ExtensionManager {
  public:
    ExtensionManager();
    ~ExtensionManager();

  private:
    std::vector<Extension> extensions;
};

} // namespace hydra::horizon::loader
