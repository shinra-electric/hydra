#include "core/horizon/services/am/home_menu_functions.hpp"

namespace hydra::horizon::services::am {

DEFINE_SERVICE_COMMAND_TABLE(IHomeMenuFunctions, 10, RequestToGetForeground, 21,
                             GetPopFromGeneralChannelEvent)

result_t IHomeMenuFunctions::GetPopFromGeneralChannelEvent(
    OutHandle<HandleAttr::Copy> out_handle) {
    out_handle = pop_from_general_channel_event.id;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::am
