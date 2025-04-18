# TODO: make a precompiled header
add_library(hydra-common
    common.hpp
    types.hpp
    macros.hpp
    functions.hpp
    config.cpp
    config.hpp
    lz4.cpp
    lz4.hpp
    logging/log.cpp
    logging/log.hpp
    allocators/static_pool.hpp
    allocators/dynamic_pool.hpp
)

target_link_libraries(hydra-common PUBLIC fmt::fmt toml11::toml11)
