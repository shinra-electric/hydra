#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"

#include "core/horizon/os.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/mmu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/thread.hpp"

namespace hydra::hw::tegra_x1::cpu::hypervisor {

Cpu::Cpu() {
    // Create VM
    hv_vm_config_t vm_config = hv_vm_config_create();
    // hv_vm_config_set_el2_enabled(config, true);
    HV_ASSERT_SUCCESS(hv_vm_create(vm_config));

    // Create GIC
    // hv_gic_config_t gic_config = hv_gic_config_create();
    // hv_gic_config_set_msi_region_base(&gic_config, TODO);
    // hv_gic_config_set_msi_interrupt_range(&gic_config, TODO);
    // HYP_ASSERT_SUCCESS(hv_gic_create(gic_config));
}

Cpu::~Cpu() { hv_vm_destroy(); }

IMmu* Cpu::CreateMmu() { return new Mmu(); }

IThread* Cpu::CreateThread(IMmu* mmu, IMemory* tls_mem) {
    Thread* thread = new Thread(mmu, tls_mem);
    return thread;
}

IMemory* Cpu::AllocateMemory(usize size) { return new Memory(pa_mapper, size); }

} // namespace hydra::hw::tegra_x1::cpu::hypervisor
