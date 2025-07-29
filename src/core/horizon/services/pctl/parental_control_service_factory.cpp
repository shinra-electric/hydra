#include "core/horizon/services/pctl/parental_control_service_factory.hpp"

#include "core/horizon/services/pctl/parental_control_service.hpp"

namespace hydra::horizon::services::pctl {

DEFINE_SERVICE_COMMAND_TABLE(IParentalControlServiceFactory, 0, CreateService,
                             1, CreateServiceWithoutInitialize)

result_t IParentalControlServiceFactory::CreateService(RequestContext* ctx) {
    auto service = new IParentalControlService();
    AddService(*ctx, service);
    return service->Initialize();
}

result_t IParentalControlServiceFactory::CreateServiceWithoutInitialize(
    RequestContext* ctx) {
    AddService(*ctx, new IParentalControlService());
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::pctl
