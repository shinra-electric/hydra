#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

// TODO: does IStorage always need to be backed by a file?
class IStorage : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStorage)

    IStorage(const std::string& path_) : path{path_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

    // Helpers
    void ReadImpl(u8* ptr, u64 offset, u64 size);

  private:
    std::string path;

    // Commands
    virtual void Read(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Fssrv
