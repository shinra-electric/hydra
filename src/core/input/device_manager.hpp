#pragma once

#include "core/input/device_list_base.hpp"

#define INPUT_DEVICE_MANAGER_INSTANCE input::DeviceManager::GetInstance()

namespace hydra::input {

class DeviceManager {
  public:
    static DeviceManager& GetInstance() {
        static DeviceManager s_instance;
        return s_instance;
    }

    DeviceManager();
    ~DeviceManager();

    void ConnectDevices();

    void Poll();

  private:
    DeviceListBase* device_list;
};

} // namespace hydra::input
