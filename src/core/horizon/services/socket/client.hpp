#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::socket {

class IClient : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t RegisterClient(u64* out_unknown);
    STUB_REQUEST_COMMAND(StartMonitoring);
    STUB_REQUEST_COMMAND(Socket);
    STUB_REQUEST_COMMAND(Poll);
    STUB_REQUEST_COMMAND(Bind);
    STUB_REQUEST_COMMAND(Connect);
    STUB_REQUEST_COMMAND(Listen);
    STUB_REQUEST_COMMAND(Fcntl);
    STUB_REQUEST_COMMAND(SetSockOpt);
    STUB_REQUEST_COMMAND(Shutdown);
    STUB_REQUEST_COMMAND(Close);
};

} // namespace hydra::horizon::services::socket
