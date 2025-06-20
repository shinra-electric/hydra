#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

struct LibraryAppletInfo {
    AppletId id;
    LibraryAppletMode mode;
};

class ILibraryAppletSelfAccessor : public ServiceBase {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t GetLibraryAppletInfo(LibraryAppletInfo* out_info);
};

} // namespace hydra::horizon::services::am
