#pragma once

#include "core/horizon/applets/applet_base.hpp"
#include "core/horizon/applets/controller/const.hpp"
#include "core/horizon/os.hpp"
#include "core/horizon/ui/handler_base.hpp"

namespace hydra::horizon::applets::controller {

class Applet : public AppletBase {
  public:
    using AppletBase::AppletBase;

  protected:
    result_t Run() override;

  private:
    template <usize max_supported_players>
    result_t RunControllerSupport() {
        LOG_STUBBED(Applets, "Controller support");

        const auto arg = PopInData<SupportArg<max_supported_players>>();

        i8 player_count_min = arg.player_count_min;
        i8 player_count_max = arg.player_count_max;
        if (arg.enable_single_mode)
            player_count_min = player_count_max = 1;

        // HACK
        PushOutData(ResultInfoInternal{
            .info =
                {
                    .player_count = player_count_min,
                    .selected_id = services::hid::NpadIdType::No1,
                },
            .result = RESULT_SUCCESS,
        });

        return RESULT_SUCCESS;
    }
};

} // namespace hydra::horizon::applets::controller
