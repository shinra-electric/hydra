#pragma once

#include "core/horizon/services/pl/shared_font_manager.hpp"

namespace hydra::horizon::services::pl::sharedresource {

class IPlatformSharedResourceManager : public ISharedFontManager {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;
};

} // namespace hydra::horizon::services::pl::sharedresource
