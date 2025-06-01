#pragma once

#include "core/horizon/applets/const.hpp"
#include "core/horizon/ui/const.hpp"

namespace hydra::horizon::ui {

// TODO: support 16-bit chars
class HandlerBase {
  public:
    // TODO: detail
    virtual void ShowMessageDialog(const MessageDialogType type,
                                   const std::string& title,
                                   const std::string& message) = 0;
    // TODO: more texts
    // TODO: text verification callback
    virtual applets::SoftwareKeyboardResult
    ShowSoftwareKeyboard(const std::string& header_text,
                         std::string& out_text) = 0;
};

} // namespace hydra::horizon::ui
