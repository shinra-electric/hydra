#include "core/input/device_manager.hpp"

#include "core/input/apple_gc/device_list.hpp"

namespace hydra::input {

DeviceManager::DeviceManager() { device_list = new apple_gc::DeviceList(); }

DeviceManager::~DeviceManager() { delete device_list; }

void DeviceManager::ConnectDevices() { LOG_FUNC_NOT_IMPLEMENTED(Input); }

void DeviceManager::Poll() { ONCE(LOG_FUNC_NOT_IMPLEMENTED(Input)); }

} // namespace hydra::input
