#include "common/log.hpp"

#include "common/config.hpp"

namespace hydra {

// TODO: will the destructor ever get called?
Logger::~Logger() {
    if (ofs) {
        ofs->close();
        delete ofs;
    }
}

void Logger::EnsureOutputStream() {
    if (ofs)
        return;

    // TODO: include log time in the filename
    const auto path = fmt::format("{}/log.log", CONFIG_INSTANCE.GetLogsPath());
    ofs = new std::ofstream(path);
}

} // namespace hydra
