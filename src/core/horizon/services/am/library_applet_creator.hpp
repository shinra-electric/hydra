#pragma once

#include "core/horizon/services/am/library_applet_accessor.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::am {

class ILibraryAppletCreator : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t CreateLibraryApplet(RequestContext* ctx, AppletId id,
                                 LibraryAppletMode mode);
    result_t CreateStorage(RequestContext* ctx, i64 size);
    result_t CreateTransferMemoryStorage(kernel::Process* process,
                                         RequestContext* ctx,
                                         InHandle<HandleAttr::Copy> tmem_handle,
                                         bool writable, i64 size);
};

} // namespace hydra::horizon::services::am
