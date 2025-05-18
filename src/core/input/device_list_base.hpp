#pragma once

#include "core/input/device_base.hpp"

namespace hydra::input {

class DeviceListBase {
  public:
    virtual ~DeviceListBase() {
        for (auto [name, device] : devices)
            delete device;
    }

    const std::map<std::string, DeviceBase*>& GetDevices() const {
        return devices;
    }

  protected:
    std::map<std::string, DeviceBase*> devices;
};

} // namespace hydra::input
