#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IApplicationProxy : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IApplicationProxy)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    // Commands
    void GetCommonStateGetter(REQUEST_COMMAND_PARAMS);
    void GetSelfController(REQUEST_COMMAND_PARAMS);
    void GetWindowController(REQUEST_COMMAND_PARAMS);
    void GetAudioController(REQUEST_COMMAND_PARAMS);
    void GetDisplayController(REQUEST_COMMAND_PARAMS);
    void GetLibraryAppletCreator(REQUEST_COMMAND_PARAMS);
    void GetApplicationFunctions(REQUEST_COMMAND_PARAMS);
    void GetDebugFunctions(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
