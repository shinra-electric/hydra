#pragma once

#include "core/horizon/kernel/service_base.hpp"

namespace Hydra::Horizon::Services::Lm {

namespace {

struct Packet {
    std::string message;
    u32 line{0};
    std::string filename;
    std::string function;
    std::string module;
    std::string thread;
    u64 drop_count{0};
    u64 time{0};
    std::string program_name;
};

} // namespace

class ILogger : public Kernel::ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(ILogger)

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    Packet packet{};

    // Commands
    void Log(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Lm
