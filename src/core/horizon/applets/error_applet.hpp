#pragma once

#include "core/horizon/applets/applet_base.hpp"

namespace hydra::horizon::applets {

class ErrorApplet : public AppletBase {
  public:
    ErrorApplet(const LibraryAppletMode mode) : AppletBase(mode) {}

  protected:
    result_t Run() override;
};

} // namespace hydra::horizon::applets
