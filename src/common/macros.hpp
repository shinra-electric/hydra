#pragma once

#define EXTRACT_BITS(value, high, low)                                         \
    (((value) >> (low)) & ((1 << ((high) - (low) + 1)) - 1))

#define BIT(n) (1 << (n))
#define ENABLE_ENUM_BITMASK_OPERATORS(e)                                       \
    inline e operator|(e a, e b) {                                             \
        return static_cast<e>(static_cast<u32>(a) | static_cast<u32>(b));      \
    }                                                                          \
    inline e& operator|=(e& a, e b) { return a = a | b; }                      \
    inline e operator&(e a, e b) {                                             \
        return static_cast<e>(static_cast<u32>(a) & static_cast<u32>(b));      \
    }                                                                          \
    inline e& operator&=(e& a, e b) { return a = a & b; }                      \
    inline e operator~(e a) { return static_cast<e>(~static_cast<u32>(a)); }   \
    inline bool any(e a) { return a != e::None; }

#define ENUM_CASE(e, value, n)                                                 \
    case e::value:                                                             \
        name = n;                                                              \
        break;

#define PARENS ()

#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define FOR_EACH(macro, e, ...)                                                \
    __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, e, __VA_ARGS__)))

#define FOR_EACH_HELPER(macro, e, a1, a2, ...)                                 \
    macro(e, a1, a2) __VA_OPT__(FOR_EACH_AGAIN PARENS(macro, e, __VA_ARGS__))

#define FOR_EACH_AGAIN() FOR_EACH_HELPER

// Main macro to enable to_string conversion
#define ENABLE_ENUM_FORMATTING(e, ...)                                         \
    template <> struct fmt::formatter<const e> : formatter<string_view> {      \
        template <typename FormatContext>                                      \
        auto format(e c, FormatContext& ctx) const {                           \
            string_view name;                                                  \
            switch (c) {                                                       \
            case e::Panic:                                                     \
                name = "panic";                                                \
                break;                                                         \
            default:                                                           \
                name = "unknown";                                              \
                break;                                                         \
            }                                                                  \
            return formatter<string_view>::format(name, ctx);                  \
        }                                                                      \
    };
