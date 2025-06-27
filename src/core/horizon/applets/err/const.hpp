#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::applets::err {

enum class ErrorType : u8 {
    SystemData = 0,
    SystemError = 1,
    ApplicationError = 2,
    Eula = 3,
    ParentalControl = 4,
    RecordedSystemData = 5,
    RecordedSystemError = 6,
    RecordedApplicationError = 7,
    SystemUpdateEula = 8,
};

struct ParamCommon {
    ErrorType type;
    bool is_jump_enabled;
    u8 _reserved[2];
};

struct ParamForApplicationError {
    u8 version;
    u8 _reserved[3];
    result_t error_code_number;
    LanguageCode language_code;
    char dialog_message[0x800];
    char fullscreen_message[0x800];
};

enum class JumpDestination : u8 {
    Nowhere = 0,
    Set_SystemUpdate = 1,
    Set_DataManagement = 2,
    Set_InternetSetting = 3,
    Cabinet_NfpDataSetting = 4,
};

struct ReturnValue {
    u8 version{0};
    JumpDestination destination{JumpDestination::Nowhere};
};

} // namespace hydra::horizon::applets::err

ENABLE_ENUM_FORMATTING(hydra::horizon::applets::err::ErrorType, SystemData,
                       "system data", SystemError, "system error",
                       ApplicationError, "application error", Eula, "eula",
                       ParentalControl, "parental control", RecordedSystemData,
                       "recorded system data", RecordedSystemError,
                       "recorded system error", RecordedApplicationError,
                       "recorded application error", SystemUpdateEula,
                       "system update eula")
