if (FRONTEND STREQUAL "SDL3")
    message(STATUS "Using SDL3 frontend")

    set(SOURCE_FILES
        sdl3/main.cpp
        sdl3/const.hpp
        sdl3/window.cpp
        sdl3/window.hpp
    )

    find_package(SDL3 CONFIG REQUIRED)
    set(LINK_LIBS SDL3::SDL3)

    add_executable(hydra ${SOURCE_FILES})
elseif (FRONTEND STREQUAL "SwiftUI")
    message(STATUS "Using SwiftUI frontend")

    set(SOURCE_FILES
        swiftui/HydraApp.swift
        swiftui/ContentView.swift
        swiftui/MenuCommands.swift
        swiftui/ToolbarItems.swift
        swiftui/Settings/GeneralSettingsView.swift
        swiftui/Settings/GraphicsSettingsView.swift
        swiftui/Settings/DebugSettingsView.swift
        swiftui/Settings/SettingsView.swift
        swiftui/Sidebar/Game.swift
        swiftui/Sidebar/MetalView.swift
        swiftui/Sidebar/EmulationView.swift
        swiftui/Sidebar/GamelistView.swift
        swiftui/Sidebar/DetailView.swift
        swiftui/Sidebar/NowPlayingView.swift
        swiftui/Sidebar/SidebarSelectionState.swift
        swiftui/Sidebar/SidebarView.swift
    )

    add_executable(hydra MACOSX_BUNDLE ${SOURCE_FILES})

    target_compile_options(hydra PRIVATE -import-objc-header ${CMAKE_SOURCE_DIR}/src/frontend/swiftui/bridging_header.h)

    set(RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/hydra.icns)
    target_sources(hydra PRIVATE "${RESOURCE_FILES}")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE "${RESOURCE_FILES}"
        OUTPUT_NAME "Hydra"
        MACOSX_BUNDLE_GUI_IDENTIFIER com.github.${PROJECT_NAME}
        MACOSX_BUNDLE_ICON_FILE "hydra.icns"
        #MACOSX_BUNDLE_BUNDLE_VERSION "${BUILD_HASH}"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
    set(MACOSX_MINIMUM_SYSTEM_VERSION "12.0")
    set(MACOSX_BUNDLE_CATEGORY "public.app-category.games")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Hydra")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright© Hydra Project")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

else ()
    message(FATAL_ERROR "Unknown frontend \"${FRONTEND}\". Valid options are \"SDL3\" and \"SwiftUI\".")
endif ()

target_link_libraries(hydra PRIVATE hydra-common hydra-core ${LINK_LIBS})

# Signing
set(ENTITLEMENTS_FILE "${PROJECT_SOURCE_DIR}/hydra.entitlements")

# TODO: use --deep?
add_custom_command(TARGET hydra POST_BUILD VERBATIM
    COMMAND codesign --entitlements ${ENTITLEMENTS_FILE} --force --sign - --timestamp --options runtime "$<TARGET_FILE:hydra>"
    COMMENT "Signing hydra with entitlements"
)
