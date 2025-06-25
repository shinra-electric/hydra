#pragma once

#include "core/horizon/const.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

struct LibraryAppletInfo {
    AppletId id;
    LibraryAppletMode mode;
};

struct AppletIdentityInfo {
    AppletId id;
    u32 _padding_x4;
    u64 application_id;
};

class ILibraryAppletSelfAccessor : public ServiceBase {
  public:
    ILibraryAppletSelfAccessor();

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t PopInData(add_service_fn_t add_service);
    result_t PushOutData(ServiceBase* storage_);
    result_t PopInteractiveInData(add_service_fn_t add_service);
    result_t PushInteractiveOutData(ServiceBase* storage_);
    result_t GetLibraryAppletInfo(LibraryAppletInfo* out_info);
    result_t GetCallerAppletIdentityInfo(AppletIdentityInfo* out_info);
};

} // namespace hydra::horizon::services::am
