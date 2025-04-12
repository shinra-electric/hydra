# TODO: if SDL3 frontend enabled
if (true)
    set(SOURCE_FILES
        sdl3/main.cpp
        sdl3/const.hpp
        sdl3/window.cpp
        sdl3/window.hpp
    )

    find_package(SDL3 CONFIG REQUIRED)
endif ()

add_executable(hydra
    ${SOURCE_FILES}
    window_base.cpp
    window_base.hpp
)

target_link_libraries(hydra PRIVATE hydra-common hydra-core)

# TODO: if SDL3 frontend enabled
if (true)
    target_link_libraries(hydra PRIVATE SDL3::SDL3)
endif ()

# Signing
set(ENTITLEMENTS_FILE "${PROJECT_SOURCE_DIR}/hydra.entitlements")

add_custom_command(TARGET hydra POST_BUILD VERBATIM
    COMMAND codesign --force --sign "${CODESIGN_IDENTITY}" --entitlements ${ENTITLEMENTS_FILE} --timestamp --options runtime "$<TARGET_FILE:hydra>"
    COMMENT "Signing hydra with entitlements"
)
