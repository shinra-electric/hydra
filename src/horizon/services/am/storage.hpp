#pragma once

#include "horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IStorage : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStorage)

    IStorage(const sized_ptr data_) : data{data_} {}
    ~IStorage() override { free(data.GetPtrU8()); }

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    const sized_ptr data;

    // Commands
    void Open(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
