#include "core/horizon/services/friends/service_creator.hpp"

#include "core/horizon/services/friends/friend_service.hpp"
#include "core/horizon/services/friends/notification_service.hpp"

namespace hydra::horizon::services::friends {

DEFINE_SERVICE_COMMAND_TABLE(IServiceCreator, 0, CreateFriendService, 1,
                             CreateNotificationService)

result_t IServiceCreator::CreateFriendService(RequestContext* ctx) {
    AddService(*ctx, new IFriendService());
    return RESULT_SUCCESS;
}

result_t IServiceCreator::CreateNotificationService(RequestContext* ctx,
                                                    uuid_t user_id) {
    AddService(*ctx, new INotificationService(user_id));
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::friends
