#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Settings {

class ISettingsServer : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ISettingsServer)

    usize GetPointerBufferSize() override { return 0x1000; }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetLanguageCode(REQUEST_COMMAND_PARAMS);
    void GetAvailableLanguageCodes(REQUEST_COMMAND_PARAMS);
    void GetAvailableLanguageCodeCount(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Settings
