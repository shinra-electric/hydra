#pragma once

#include "core/input/device_list.hpp"
#include "core/input/npad_config.hpp"

#define INPUT_DEVICE_MANAGER_INSTANCE input::DeviceManager::GetInstance()

namespace hydra::input {

constexpr u32 NPAD_COUNT = 8; // 8 players

class DeviceManager {
  public:
    static DeviceManager& GetInstance() {
        static DeviceManager s_instance;
        return s_instance;
    }

    DeviceManager();
    ~DeviceManager();

    void ConnectNpads();

    void Poll();

    // Touch screen devices
    void ConnectTouchScreenDevice(const std::string& name, IDevice* device) {
        ASSERT(device->ActsAsTouchScreen(), Input,
               "Device \"{}\" does not act as a touch screen", name);
        device_list->devices.insert({name, device});
    }

    IDevice* DisconnectTouchScreenDevice(const std::string& name) {
        auto it = device_list->devices.find(name);
        ASSERT(it != device_list->devices.end(), Input,
               "Touch screen \"{}\" not connected", name);
        device_list->devices.erase(it);
        return it->second;
    }

  private:
    IDeviceList* device_list;
    NpadConfig npad_configs[NPAD_COUNT];

    std::map<u64, u32> active_touches;

    // Helpers
    IDevice* GetDevice(const std::string& name) {
        auto it = device_list->devices.find(name);
        if (it == device_list->devices.end())
            return nullptr;

        return it->second;
    }

    void PollNpad(horizon::hid::NpadIdType type, u32 index);
};

} // namespace hydra::input
