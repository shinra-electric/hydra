#pragma once

#include "common/common.hpp"

namespace Hydra::Horizon::Services::Am {

enum class AppletMessage {
    ExitRequest = 4,             ///< Exit request.
    FocusStateChanged = 15,      ///< FocusState changed.
    Resume = 16,                 ///< Current applet execution was resumed.
    OperationModeChanged = 30,   ///< OperationMode changed.
    PerformanceModeChanged = 31, ///< PerformanceMode changed.
    RequestToDisplay =
        51, ///< Display requested, see \ref appletApproveToDisplay.
    CaptureButtonShortPressed = 90, ///< Capture button was short-pressed.
    AlbumScreenShotTaken = 92,      ///< Screenshot was taken.
    AlbumRecordingSaved = 93,       ///< AlbumRecordingSaved
};

enum class AppletFocusState {
    InFocus = 1,    ///< Applet is focused.
    OutOfFocus = 2, ///< Out of focus - LibraryApplet open.
    Background = 3  ///< Out of focus - HOME menu open / console is sleeping.
};

} // namespace Hydra::Horizon::Services::Am
