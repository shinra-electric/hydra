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
    horizon/os.cpp
    horizon/os.hpp
    horizon/const.hpp
    horizon/hipc.hpp
    horizon/cmif.hpp
    horizon/kernel.cpp
    horizon/kernel.hpp
    horizon/services/service_base.cpp
    horizon/services/service_base.hpp
    horizon/services/domain_service.cpp
    horizon/services/domain_service.hpp
    horizon/services/sm/user_interface.cpp
    horizon/services/sm/user_interface.hpp
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
    horizon/services/nvdrv/ioctl/fd_base.hpp
    horizon/services/nvdrv/ioctl/nvhost_ctrl.cpp
    horizon/services/nvdrv/ioctl/nvhost_ctrl.hpp
    horizon/services/nvdrv/ioctl/nvmap.cpp
    horizon/services/nvdrv/ioctl/nvmap.hpp
    horizon/services/nvdrv/const.hpp
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
    horizon/services/visrv/display_service_base.cpp
    horizon/services/visrv/display_service_base.hpp
    horizon/services/visrv/system_display_service.cpp
    horizon/services/visrv/system_display_service.hpp
    horizon/services/visrv/manager_display_service.cpp
    horizon/services/visrv/manager_display_service.hpp
    horizon/services/visrv/application_display_service.cpp
    horizon/services/visrv/application_display_service.hpp
    horizon/services/visrv/manager_root_service.cpp
    horizon/services/visrv/manager_root_service.hpp
    horizon/services/hosbinder/hos_binder_driver.cpp
    horizon/services/hosbinder/hos_binder_driver.hpp
    hw/tegra_x1/cpu/cpu_base.hpp
    hw/tegra_x1/cpu/hypervisor/cpu.cpp
    hw/tegra_x1/cpu/hypervisor/cpu.hpp
    hw/tegra_x1/mmu/mmu_base.cpp
    hw/tegra_x1/mmu/mmu_base.hpp
    hw/tegra_x1/mmu/memory.cpp
    hw/tegra_x1/mmu/memory.hpp
    hw/tegra_x1/mmu/hypervisor/mmu.cpp
    hw/tegra_x1/mmu/hypervisor/mmu.hpp
    hw/tegra_x1/gpu/gpu.cpp
    hw/tegra_x1/gpu/gpu.hpp
    hw/display/display.hpp
    hw/bus.hpp
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
