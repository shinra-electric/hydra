add_executable(hydra
    main.cpp
    common/common.hpp
    common/types.hpp
    common/macros.hpp
    common/functions.hpp
    common/logging/log.cpp
    common/logging/log.hpp
    common/allocators/static_pool.hpp
    common/allocators/dynamic_pool.hpp
    horizon/horizon.cpp
    horizon/horizon.hpp
    horizon/const.hpp
    horizon/hipc.hpp
    horizon/cmif.hpp
    horizon/kernel.cpp
    horizon/kernel.hpp
    horizon/services/service.cpp
    horizon/services/service.hpp
    horizon/services/domain_service.cpp
    horizon/services/domain_service.hpp
    horizon/services/sm.cpp
    horizon/services/sm.hpp
    horizon/services/hid/hid_server.cpp
    horizon/services/hid/hid_server.hpp
    horizon/services/fssrv/filesystem_proxy.cpp
    horizon/services/fssrv/filesystem_proxy.hpp
    horizon/services/timesrv/static_service.cpp
    horizon/services/timesrv/static_service.hpp
    horizon/services/timesrv/system_clock.cpp
    horizon/services/timesrv/system_clock.hpp
    horizon/services/timesrv/steady_clock.cpp
    horizon/services/timesrv/steady_clock.hpp
    horizon/services/timesrv/time_zone_service.cpp
    horizon/services/timesrv/time_zone_service.hpp
    horizon/services/nvdrv/nvdrv_services.cpp
    horizon/services/nvdrv/nvdrv_services.hpp
    horizon/services/settings/system_settings_server.cpp
    horizon/services/settings/system_settings_server.hpp
    horizon/services/am/const.hpp
    horizon/services/am/apm_manager.cpp
    horizon/services/am/apm_manager.hpp
    horizon/services/am/common_state_getter.cpp
    horizon/services/am/common_state_getter.hpp
    horizon/services/am/application_proxy_service.cpp
    horizon/services/am/application_proxy_service.hpp
    horizon/services/am/application_proxy.cpp
    horizon/services/am/application_proxy.hpp
    horizon/services/visrv/application_display_service.cpp
    horizon/services/visrv/application_display_service.hpp
    horizon/services/visrv/manager_root_service.cpp
    horizon/services/visrv/manager_root_service.hpp
    hw/tegra_x1/cpu/cpu.hpp
    hw/tegra_x1/cpu/hypervisor/cpu.cpp
    hw/tegra_x1/cpu/hypervisor/cpu.hpp
    hw/tegra_x1/mmu/mmu.cpp
    hw/tegra_x1/mmu/mmu.hpp
    hw/tegra_x1/mmu/memory.cpp
    hw/tegra_x1/mmu/memory.hpp
    hw/tegra_x1/mmu/hypervisor/mmu.cpp
    hw/tegra_x1/mmu/hypervisor/mmu.hpp
    hypervisor/const.hpp
    hypervisor/hypervisor.cpp
    hypervisor/hypervisor.hpp
)

target_include_directories(hydra PRIVATE . /Users/samuliak/Documents/nx-parser/src) # TODO: dependencies directory
target_link_libraries(hydra PUBLIC "-framework Hypervisor" nx-parser fmt::fmt)

# Signing
set(CODESIGN_IDENTITY "Apple Development: samuliak@icloud.com (RMHRRGUDTQ)") # TODO: don't hardcode
set(ENTITLEMENTS_FILE "${PROJECT_SOURCE_DIR}/hydra.entitlements")

add_custom_command(TARGET hydra POST_BUILD VERBATIM
    COMMAND codesign --force --sign "${CODESIGN_IDENTITY}" --entitlements ${ENTITLEMENTS_FILE} --timestamp --options runtime "$<TARGET_FILE:hydra>"
    COMMENT "Signing hydra with entitlements"
)
