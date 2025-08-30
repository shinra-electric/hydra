#pragma once

namespace hydra::horizon::services::usb {

enum class UsbPowerRole : u32 {
    Unknown = 0,
    Slink = 1,
    Source = 2,
};

enum class UsbChargerType : u32 {
    Unknown = 0,
    Pd = 1,
    TypeC15 = 2,
    TypeC30 = 3,
    Dcp = 4,
    Cdp = 5,
    Sdp = 6,
    Apple500 = 7,
    Apple1000 = 8,
    Apple2000 = 9,
};

} // namespace hydra::horizon::services::usb
