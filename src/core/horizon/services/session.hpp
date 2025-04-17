#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services {

class Session : public KernelHandle {
  public:
    Session(ServiceBase* service_) : service{service_} {}

    void Close();
    void Request(REQUEST_PARAMS);
    void Control(Readers& readers, Writers& writers);

    // Getters
    // Handle GetHandle() const { return handle; }

    // Setters
    void SetHandleId(handle_id_t handle_id_) { handle_id = handle_id_; }

  private:
    ServiceBase* service;

    handle_id_t handle_id{invalid<handle_id_t>()};
};

} // namespace Hydra::Horizon::Services
