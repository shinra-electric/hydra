#pragma once

#include "core/horizon/applets/applet_base.hpp"

namespace hydra::horizon::applets::err {

class ErrorApplet : public AppletBase {
  public:
    using AppletBase::AppletBase;

  protected:
    result_t Run() override;
};

} // namespace hydra::horizon::applets::err
