#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {

class StateManager {
  public:
    static StateManager& GetInstance();

    StateManager();
    ~StateManager();

    // Send
    void SendMessage(AppletMessage msg) {
        std::unique_lock<std::mutex> lock(mutex);
        msg_queue.push(msg);
    }

    void SetFocusState(AppletFocusState focus_state_) {
        std::unique_lock<std::mutex> lock(mutex);
        focus_state = focus_state_;
        msg_queue.push(AppletMessage::FocusStateChanged);
    }

    void LockExit() {
        std::unique_lock<std::mutex> lock(mutex);
        exit_locked = true;
    }

    void UnlockExit() {
        std::unique_lock<std::mutex> lock(mutex);
        exit_locked = false;
    }

    // Receive
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

    AppletFocusState GetFocusState() {
        std::unique_lock<std::mutex> lock(mutex);
        const auto f_state = focus_state;

        return f_state;
    }

    bool IsExitLocked() {
        std::unique_lock<std::mutex> lock(mutex);
        return exit_locked;
    }

  private:
    std::mutex mutex;
    std::queue<AppletMessage> msg_queue;
    AppletFocusState focus_state;
    bool exit_locked{false};
};

} // namespace Hydra::Horizon
