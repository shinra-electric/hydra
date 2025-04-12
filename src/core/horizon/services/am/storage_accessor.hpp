#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Am {

class IStorageAccessor : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStorageAccessor)

    IStorageAccessor(const sized_ptr data_) : data{data_} {}

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

  private:
    const sized_ptr data;

    // Commands
    void GetSize(REQUEST_COMMAND_PARAMS);
    void Write(REQUEST_COMMAND_PARAMS);
    void Read(REQUEST_COMMAND_PARAMS);
};

} // namespace Hydra::Horizon::Services::Am
