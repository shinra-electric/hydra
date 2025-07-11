#include "core/horizon/kernel/process_manager.hpp"

#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::kernel {

ProcessManager::~ProcessManager() { CleanUpFinishedProcesses(); }

Process* ProcessManager::CreateProcess(const std::string_view name) {
    Process* process = new Process(name);
    processes.push_back(process);
    return process;
}

void ProcessManager::CleanUpFinishedProcesses() {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto it = processes.cbegin(); it != processes.cend();) {
        if (!(*it)->IsRunning()) {
            delete *it;
            it = processes.erase(it);
        } else {
            it++;
        }
    }
}

bool ProcessManager::HasRunningProcesses() {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& process : processes) {
        if (process->IsRunning())
            return true;
    }
    return false;
}

} // namespace hydra::horizon::kernel
