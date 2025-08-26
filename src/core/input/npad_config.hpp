#pragma once

#include "core/input/const.hpp"

namespace hydra::input {

class NpadConfig {
  public:
    NpadConfig(horizon::hid::NpadIdType type_);

  private:
    horizon::hid::NpadIdType type;
    std::map<std::string, std::vector<CodeButtonMapping>> button_mappings;
    std::map<std::string, std::vector<CodeAnalogMapping>> analog_mappings;

  public:
    CONST_REF_GETTER(button_mappings, GetButtonMappings);
    CONST_REF_GETTER(analog_mappings, GetAnalogMappings);
};

} // namespace hydra::input
