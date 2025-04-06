#pragma once

#include "common/common.hpp"

namespace Hydra::HW::TegraX1::CPU {

class MMUBase;
class ThreadBase;

class CPUBase {
  public:
    static CPUBase& GetInstance();

    CPUBase();
    virtual ~CPUBase();

    virtual ThreadBase* CreateThread() = 0;

    virtual MMUBase* GetMMU() const = 0;
};

} // namespace Hydra::HW::TegraX1::CPU
