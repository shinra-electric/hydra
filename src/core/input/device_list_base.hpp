#pragma once

#include "core/input/device_base.hpp"

namespace hydra::input {

class DeviceListBase {
  public:
    virtual ~DeviceListBase() {
        for (auto device : devices)
            delete device;
    }

    const std::vector<DeviceBase*>& GetDevices() const { return devices; }

  protected:
    std::vector<DeviceBase*> devices;
};

} // namespace hydra::input
