#pragma once

#include "core/horizon/kernel/const.hpp"

namespace Hydra::Horizon {

struct FirmwareVersion {
    u8 major;
    u8 minor;
    u8 micro;
    u8 padding1;
    u8 revision_major;
    u8 revision_minor;
    u8 padding2;
    u8 padding3;
    char platform[0x20];
    char version_hash[0x40];
    char display_version[0x18];
    char display_title[0x80];
};

constexpr FirmwareVersion FIRMWARE_VERSION = {
    .major = 1,
    .minor = 0,
    .micro = 0,
    .revision_major = 0,
    .revision_minor = 0,
    .platform = "NX",
    .version_hash = "voyp5gq7m551zuqgspcgobbmo74rg6yydpalt72l",
    .display_version = "1.0.0",
    .display_title = "Hydra firmware 1.0.0",
};

enum class ConfigEntryType : u32 {
    EndOfList = 0,        ///< Entry list terminator.
    MainThreadHandle = 1, ///< Provides the handle to the main thread.
    NextLoadPath = 2,    ///< Provides a buffer containing information about the
                         ///< next homebrew application to load.
    OverrideHeap = 3,    ///< Provides heap override information.
    OverrideService = 4, ///< Provides service override information.
    Argv = 5,            ///< Provides argv.
    SyscallAvailableHint =
        6,          ///< Provides syscall availability hints (SVCs 0x00..0x7F).
    AppletType = 7, ///< Provides APT applet type.
    AppletWorkaround =
        8, ///< Indicates that APT is broken and should not be used.
    Reserved9 =
        9, ///< Unused/reserved entry type, formerly used by StdioSockets.
    ProcessHandle = 10,  ///< Provides the process handle.
    LastLoadResult = 11, ///< Provides the last load result.
    RandomSeed = 14, ///< Provides random data used to seed the pseudo-random
                     ///< number generator.
    UserIdStorage =
        15, ///< Provides persistent storage for the preselected user id.
    HosVersion = 16, ///< Provides the currently running Horizon OS version.
    SyscallAvailableHint2 =
        17, ///< Provides syscall availability hints (SVCs 0x80..0xBF).
};

enum class ConfigEntryFlag : u32 {
    None = 0,
    IsMandatory = BIT(0), ///< Specifies that the entry **must** be processed by
                          ///< the homebrew application.
};

struct ConfigEntry {
    ConfigEntryType type;  ///< Type of entry
    ConfigEntryFlag flags; ///< Entry flags
    u64 values[2];         ///< Entry arguments (type-specific)
};

enum class AppletMessage {
    None = 0,
    ChangeIntoForeground = 1,
    ChangeIntoBackground = 2,
    Exit = 4,
    ApplicationExited = 6,
    FocusStateChanged = 15,
    Resume = 16,
    DetectShortPressingHomeButton = 20,
    DetectLongPressingHomeButton = 21,
    DetectShortPressingPowerButton = 22,
    DetectMiddlePressingPowerButton = 23,
    DetectLongPressingPowerButton = 24,
    RequestToPrepareSleep = 25,
    FinishedSleepSequence = 26,
    SleepRequiredByHighTemperature = 27,
    SleepRequiredByLowBattery = 28,
    AutoPowerDown = 29,
    OperationModeChanged = 30,
    PerformanceModeChanged = 31,
    DetectReceivingCecSystemStandby = 32,
    SdCardRemoved = 33,
    LaunchApplicationRequested = 50,
    RequestToDisplay = 51,
    ShowApplicationLogo = 55,
    HideApplicationLogo = 56,
    ForceHideApplicationLogo = 57,
    FloatingApplicationDetected = 60,
    DetectShortPressingCaptureButton = 90,
    AlbumScreenShotTaken = 92,
    AlbumRecordingSaved = 93,
    DetectShortPressingCaptureButtonForApplet = 110,
    DetectLongPressingCaptureButtonForApplet = 111,
};

enum class AppletFocusState {
    InFocus = 1,    ///< Applet is focused.
    OutOfFocus = 2, ///< Out of focus - LibraryApplet open.
    Background = 3  ///< Out of focus - HOME menu open / console is sleeping.
};

enum class LaunchParameterKind : u32 {
    UserChannel = 1,
    PreselectedUser,
    Unknown0,
};

enum class LanguageCode : u64 {
    Japanese = str_to_u64("ja"),
    AmericanEnglish = str_to_u64("en-US"),
    French = str_to_u64("fr"),
    German = str_to_u64("de"),
    Italian = str_to_u64("it"),
    Spanish = str_to_u64("es"),
    Chinese = str_to_u64("zh-CN"),
    Korean = str_to_u64("ko"),
    Dutch = str_to_u64("nl"),
    Portuguese = str_to_u64("pt"),
    FuckRussia = str_to_u64("ru"), // TODO: should be "Russian"
    Russian = FuckRussia,
    Taiwanese = str_to_u64("zh-TW"),
    BritishEnglish = str_to_u64("en-GB"),
    CanadianFrench = str_to_u64("fr-CA"),
    LatinAmericanSpanish = str_to_u64("es-419"),
    SimplifiedChinese = str_to_u64("zh-Hans"),
    TraditionalChinese = str_to_u64("zh-Hant"),
    BrazilianPortugese = str_to_u64("pt-BR"),
};

constexpr LanguageCode available_languages[] = {
    LanguageCode::Japanese,
    LanguageCode::AmericanEnglish,
    LanguageCode::French,
    LanguageCode::German,
    LanguageCode::Italian,
    LanguageCode::Spanish,
    LanguageCode::Chinese,
    LanguageCode::Korean,
    LanguageCode::Dutch,
    LanguageCode::Portuguese,
    LanguageCode::Russian,
    LanguageCode::Taiwanese,
    LanguageCode::BritishEnglish,
    LanguageCode::CanadianFrench,
    LanguageCode::LatinAmericanSpanish,
    LanguageCode::SimplifiedChinese,
    LanguageCode::TraditionalChinese,
    LanguageCode::BrazilianPortugese,
};

} // namespace Hydra::Horizon

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::AppletMessage, None, "none", ChangeIntoForeground,
    "change into foreground", ChangeIntoBackground, "change into background",
    Exit, "exit", ApplicationExited, "application exited", FocusStateChanged,
    "focus state changed", Resume, "resume", DetectShortPressingHomeButton,
    "detect short pressing home button", DetectLongPressingHomeButton,
    "detect long pressing home button", DetectShortPressingPowerButton,
    "detect short pressing power button", DetectMiddlePressingPowerButton,
    "detect middle pressing power button", DetectLongPressingPowerButton,
    "detect long pressing power button", RequestToPrepareSleep,
    "request to prepare sleep", FinishedSleepSequence,
    "finished sleep sequence", SleepRequiredByHighTemperature,
    "sleep required by high temperature", SleepRequiredByLowBattery,
    "sleep required by low battery", AutoPowerDown, "auto power down",
    OperationModeChanged, "operation mode changed", PerformanceModeChanged,
    "performance mode changed", DetectReceivingCecSystemStandby,
    "detect receiving cec system standby", SdCardRemoved, "sd card removed",
    LaunchApplicationRequested, "launch application requested",
    RequestToDisplay, "request to display", ShowApplicationLogo,
    "show application logo", HideApplicationLogo, "hide application logo",
    ForceHideApplicationLogo, "force hide application logo",
    FloatingApplicationDetected, "floating application detected",
    DetectShortPressingCaptureButton, "detect short pressing capture button",
    AlbumScreenShotTaken, "album screenshot taken", AlbumRecordingSaved,
    "album recording saved", DetectShortPressingCaptureButtonForApplet,
    "detect short pressing capture button for applet",
    DetectLongPressingCaptureButtonForApplet,
    "detect long pressing capture button for applet")

ENABLE_ENUM_FORMATTING(Hydra::Horizon::LaunchParameterKind, UserChannel,
                       "user channel", PreselectedUser, "preselected user",
                       Unknown0, "unknown0")
