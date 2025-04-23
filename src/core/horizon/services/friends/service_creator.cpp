#include "core/horizon/services/friends/service_creator.hpp"

#include "core/horizon/services/friends/friend_service.hpp"

namespace Hydra::Horizon::Services::Friends {

DEFINE_SERVICE_COMMAND_TABLE(IServiceCreator, 0, CreateFriendService)

void IServiceCreator::CreateFriendService(REQUEST_COMMAND_PARAMS) {
    add_service(new IFriendService());
}

} // namespace Hydra::Horizon::Services::Friends
