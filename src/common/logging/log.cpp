#include "common/logging/log.hpp"

namespace Hydra::Logging {

Output g_output = Output::Stdout;
std::mutex g_log_mutex;

} // namespace Hydra::Logging
