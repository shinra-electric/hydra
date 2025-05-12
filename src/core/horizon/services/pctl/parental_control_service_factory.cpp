#include "core/horizon/services/pctl/parental_control_service_factory.hpp"

#include "core/horizon/services/pctl/parental_control_service.hpp"

namespace Hydra::Horizon::Services::Pctl {

DEFINE_SERVICE_COMMAND_TABLE(IParentalControlServiceFactory, 0,
                             CreateParentalControlService)

result_t IParentalControlServiceFactory::CreateParentalControlService(
    add_service_fn_t add_service) {
    add_service(new IParentalControlService());
    return RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::Pctl
