#pragma once

#include "core/horizon/services/const.hpp"
#include "core/horizon/services/ssl/sf/const.hpp"

namespace hydra::horizon::services::ssl::sf {

class ISslContext : public ServiceBase {
  public:
    ISslContext(const SslVersion version_) : version{version_} {}

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    SslVersion version;

    // Commands
};

} // namespace hydra::horizon::services::ssl::sf
