#pragma once

#include "core/input/device.hpp"

namespace hydra::input {

// Uses Nintendo Switch layout
enum class ControllerButton {
    Invalid = 0,

    A,
    B,
    X,
    Y,
    StickL,
    StickR,
    L,
    R,
    ZL,
    ZR,
    Plus,
    Minus,
    Left,
    Up,
    Right,
    Down,
    StickLLeft,
    StickLUp,
    StickLRight,
    StickLDown,
    StickRLeft,
    StickRUp,
    StickRRight,
    StickRDown,
    LeftSL,
    LeftSR,
    RightSL,
    RightSR,
};

class IController : public IDevice {
  public:
    bool ActsAsController() const override { return true; }

    // Controller
    bool IsPressed(const Code& code) override {
        if (code.GetDeviceType() != DeviceType::Controller)
            return false;

        const auto button = code.GetValue<ControllerButton>();
        return IsPressedImpl(button);
    }

    f32 GetAxisValue(const Code& code) override {
        if (code.GetDeviceType() != DeviceType::Controller)
            return 0.0f;

        const auto key = code.GetValue<ControllerButton>();
        return IsPressedImpl(key) ? 1.0f : 0.0f;
    }

  protected:
    virtual bool IsPressedImpl(ControllerButton button) = 0;
};

} // namespace hydra::input

ENABLE_ENUM_FORMATTING_AND_CASTING(
    hydra::input, Key, key, Q, "Q", W, "W", E, "E", R, "R", T, "T", Y, "Y", U,
    "U", I, "I", O, "O", P, "P", A, "A", S, "S", D, "D", F, "F", G, "G", H, "H",
    J, "J", K, "K", L, "L", Z, "Z", X, "X", C, "C", V, "V", B, "B", N, "N", M,
    "M", ArrowLeft, "Left", ArrowRight, "Right", ArrowUp, "Up", ArrowDown,
    "Down", Enter, "Enter", Tab, "Tab", Backspace, "Backspace", Space, "Space",
    LeftShift, "Left shift", RightShift, "Right shift", LeftControl,
    "Left control", RightControl, "Right control", LeftAlt, "Left alt",
    RightAlt, "Right alt", LeftSuper, "Left super", RightSuper, "Right super")
