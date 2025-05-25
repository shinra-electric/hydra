# TODO: make a precompiled header
add_library(hydra-common
    common.hpp
    macros.hpp
    type_aliases.hpp
    types.hpp
    traits.hpp
    functions.hpp
    literals.hpp
    memory.hpp
    time.hpp
    config.cpp
    config.hpp
    lz4.cpp
    lz4.hpp
    logging/log.cpp
    logging/log.hpp
    allocators/static_pool.hpp
    allocators/dynamic_pool.hpp
)
target_precompile_headers(hydra-common PUBLIC common.hpp)

target_link_libraries(hydra-common PUBLIC fmt::fmt)
target_link_libraries(hydra-common PRIVATE toml11::toml11)
