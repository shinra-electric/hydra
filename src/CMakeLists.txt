add_executable(hydra
    main.cpp
    common/common.hpp
    common/types.hpp
    common/functions.hpp
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
    horizon/services/time/static_service.cpp
    horizon/services/time/static_service.hpp
    horizon/services/time/system_clock.cpp
    horizon/services/time/system_clock.hpp
    horizon/services/time/steady_clock.cpp
    horizon/services/time/steady_clock.hpp
    horizon/services/time/time_zone_service.cpp
    horizon/services/time/time_zone_service.hpp
    horizon/services/nvdrv/nvdrv_services.cpp
    horizon/services/nvdrv/nvdrv_services.hpp
    hw/cpu/cpu.hpp
    hw/cpu/hypervisor/cpu.cpp
    hw/cpu/hypervisor/cpu.hpp
    hw/mmu/mmu.cpp
    hw/mmu/mmu.hpp
    hw/mmu/memory.cpp
    hw/mmu/memory.hpp
    hw/mmu/hypervisor/mmu.cpp
    hw/mmu/hypervisor/mmu.hpp
    hypervisor/const.hpp
    hypervisor/hypervisor.cpp
    hypervisor/hypervisor.hpp
)

target_include_directories(hydra PRIVATE . /Users/samuliak/Documents/nx-parser/src) # TODO: dependencies directory
target_link_libraries(hydra PRIVATE "-framework Hypervisor" nx-parser)

# Signing
set(CODESIGN_IDENTITY "Apple Development: samuliak@icloud.com (RMHRRGUDTQ)") # TODO: don't hardcode
set(ENTITLEMENTS_FILE "${PROJECT_SOURCE_DIR}/hydra.entitlements")

add_custom_command(TARGET hydra POST_BUILD VERBATIM
    COMMAND codesign --force --sign "${CODESIGN_IDENTITY}" --entitlements ${ENTITLEMENTS_FILE} --timestamp --options runtime "$<TARGET_FILE:hydra>"
    COMMENT "Signing hydra with entitlements"
)
