#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon {

class UserManager {
  public:
    static UserManager& GetInstance();

    UserManager();
    ~UserManager();

  private:
};

} // namespace hydra::horizon
