#pragma once

namespace hydra::horizon::services::ssl::sf {

enum class SslVersion : u32 {
    Auto = 0,
    TlsV10 = 3,
    TlsV11 = 4,
    TlsV12 = 5,
    TlsV13 = 6,           // 11.0.0+
    ApiVersionStart = 24, // 11.0.0+
};

enum class SystemVersion : u32 {
    _3_0_0 = 1,
    _5_0_0 = 2,
    _6_0_0 = 3,
    _20_0_0 = 4,
};

} // namespace hydra::horizon::services::ssl::sf

ENABLE_ENUM_FORMATTING(hydra::horizon::services::ssl::sf::SslVersion, Auto,
                       "auto", TlsV10, "TLS v10", TlsV11, "TLS v11", TlsV12,
                       "TLS v12", TlsV13, "TLS v13")

ENABLE_ENUM_FORMATTING(hydra::horizon::services::ssl::sf::SystemVersion, _3_0_0,
                       "3.0.0+", _5_0_0, "5.0.0+", _6_0_0, "6.0.0+", _20_0_0,
                       "20.0.0+")
