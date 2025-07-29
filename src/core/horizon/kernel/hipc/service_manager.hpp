#pragma once

#include "core/horizon/kernel/hipc/client_session.hpp"

namespace hydra::horizon::kernel::hipc {

class ClientSession;

template <typename Key>
class ServiceManager {
  public:
    ~ServiceManager() {
        std::lock_guard lock(mutex);
        for (auto& [_, port] : ports)
            port->Release();
    }

    void RegisterPort(const Key& port_name, ClientSession* client_session) {
        std::lock_guard lock(mutex);
        client_session->Retain();
        ports[port_name] = client_session;
    }

    void UnregisterPort(const Key& port_name) {
        std::lock_guard lock(mutex);
        ports.erase(port_name);
    }

    ClientSession* GetPort(const Key& port_name) {
        std::lock_guard lock(mutex);
        auto it = ports.find(port_name);
        if (it == ports.end())
            return nullptr;

        return it->second;
    }

  private:
    std::mutex mutex;
    std::map<Key, ClientSession*> ports;
};

} // namespace hydra::horizon::kernel::hipc
