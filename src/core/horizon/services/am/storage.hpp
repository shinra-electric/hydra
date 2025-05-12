#pragma once

#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Am {

class IStorage : public ServiceBase {
  public:
    IStorage(const sized_ptr data_) : data{data_} {}
    ~IStorage() override { free(data.GetPtrU8()); }

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    const sized_ptr data;

    // Commands
    result_t Open(add_service_fn_t add_service);
};

} // namespace Hydra::Horizon::Services::Am
