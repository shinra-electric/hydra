add_executable(hydra
    main.cpp
    common.hpp
    horizon/horizon.cpp
    horizon/horizon.hpp
    horizon/const.hpp
    horizon/kernel.cpp
    horizon/kernel.hpp
    horizon/services/service.hpp
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
