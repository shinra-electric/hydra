#pragma once

#include "core/horizon/applets/applet_base.hpp"
#include "core/horizon/applets/swkbd/const.hpp"

namespace hydra::horizon::applets::swkbd {

class SoftwareKeyboard : public AppletBase {
  public:
    using AppletBase::AppletBase;

  protected:
    result_t Run() override;
};

} // namespace hydra::horizon::applets::swkbd
