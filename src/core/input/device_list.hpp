#pragma once

#include "core/input/device.hpp"

namespace hydra::input {

class IDeviceList {
    friend class DeviceManager;

  public:
    virtual ~IDeviceList() {
        for (auto [name, device] : devices)
            delete device;
    }

  protected:
    std::map<std::string, IDevice*> devices;
};

} // namespace hydra::input
