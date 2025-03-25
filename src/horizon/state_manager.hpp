#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {

class StateManager {
  public:
    static StateManager& GetInstance();

    StateManager();
    ~StateManager();

    void SendMessage(AppletMessage msg) {
        std::unique_lock<std::mutex> lock(mutex);
        msg_queue.push(msg);
    }

    AppletMessage ReceiveMessage() {
        std::unique_lock<std::mutex> lock(mutex);
        if (msg_queue.empty()) {
            return AppletMessage::None;
        }

        AppletMessage msg = msg_queue.front();
        msg_queue.pop();
        LOG_DEBUG(Horizon, "Msg: {}", (u32)msg);

        return msg;
    }

    void SetFocusState(AppletFocusState focus_state_) {
        std::unique_lock<std::mutex> lock(mutex);
        focus_state = focus_state_;
        msg_queue.push(AppletMessage::FocusStateChanged);
    }

    AppletFocusState GetFocusState() {
        std::unique_lock<std::mutex> lock(mutex);
        const auto f_state = focus_state;

        return f_state;
    }

  private:
    std::mutex mutex;
    std::queue<AppletMessage> msg_queue;
    AppletFocusState focus_state;
};

} // namespace Hydra::Horizon
