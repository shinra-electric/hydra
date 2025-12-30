#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::aocsrv {

class IAddOnContentManager : public IService {
  protected:
    result_t RequestImpl([[maybe_unused]] RequestContext& context,
                         u32 id) override;

  private:
    // Commands
    result_t CountAddOnContent(i64* out_count);
    // TODO: correct?
    result_t ListAddOnContent(u32 start_index, u32 buffer_size, u32* out_count,
                              OutBuffer<BufferAttr::MapAlias> out_buffer);
    STUB_REQUEST_COMMAND(CheckAddOnContentMountStatus);
};

} // namespace hydra::horizon::services::aocsrv
