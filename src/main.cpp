#include "horizon/os.hpp"
#include "hw/bus.hpp"
#include "hw/display/display.hpp"
#include "hw/tegra_x1/cpu/cpu_base.hpp"
#include "hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "hw/tegra_x1/cpu/hypervisor/mmu.hpp"
#include "hw/tegra_x1/cpu/hypervisor/thread.hpp"
#include "hw/tegra_x1/gpu/gpu.hpp"

// HACK
const std::string path =
    "/Users/samuliak/Documents/deko3d_examples/build/0_hello_world.nro";

// HACK
// #define PRINT_ADDR_TO_INDEX(addr) \
//    Logging::log(Logging::Level::Debug, "PRINT_ADDR_TO_INDEX: %u", (addr -
//    0x80) / 4)
// #define PRINT_INDEX_TO_ADDR(addr) \
//    Logging::log(Logging::Level::Debug, "PRINT_INDEX_TO_ADDR: 0x%08x", addr
//    * 4 + 0x80)
// #define PRINT_PC_TO_ADDR(pc) \
//    Logging::log(Logging::Level::Debug, "PRINT_PC_TO_ADDR: 0x%016x", pc -
//    0x80000000 + 0x80)

// HACK
void SET_INSTRUCTION(u32* data, i64 addr, u32 new_instruction) {
    data[(addr - 0x80) / 4] = new_instruction;
}

// HACK
#define NOP 0xd503201fu
#define RET 0xd65f03c0u
#define MOV_X0_XZR 0xaa1f03e0u
#define BRK 0xd4200000u

int main(int argc, const char* argv[]) {
    // Parse file
    usize size;
    auto ifs = Hydra::open_file(path, size);
    // std::vector<u8> d(size);
    // ifs.read((char*)d.data(), size);
    // Logging::log(Logging::Level::Debug, "AAA: 0x%llx", *((u64*)(d.data() +
    // 0x17498))); return 0;
    BinaryReader reader(ifs, size);
    Rom* rom = ParseNRO(reader);
    ifs.close();

    // HACK
    u32* data = (u32*)(rom->GetRom().data() + rom->GetTextOffset());
    // data[10] = NOP; // __nx_dynamic
    // data[16] = NOP; // __libnx_init
    // SET_INSTRUCTION(data, 3176, 0xa9be7bfd, RET); // mutexLock
    // SET_INSTRUCTION(data, 3208, 0xd53bd061, RET); // mutexUnlock
    //  SET_INSTRUCTION(data, 9148, 0x97ffff24, MOV_X0_XZR); // _smCmifCmdInPid
    // q31, [x0, #0x20]"
    // SET_INSTRUCTION(data, 8710, 0x9400020e, MOV_X0_XZR); //
    // smGetServiceWrapper SET_INSTRUCTION(data, 8724, 0x97ffff60,
    //                 MOV_X0_XZR); // _hidCreateAppletResource.constprop.0
    // SET_INSTRUCTION(data, 8728, 0x97ffffa4,
    //                 MOV_X0_XZR); // _hidCmdGetHandle.constprop.0
    // SET_INSTRUCTION(data, 8739, 0x94000645, MOV_X0_XZR); // shmemMap
    // SET_INSTRUCTION(data, 755, 0x940022e1, MOV_X0_XZR);  // timeInitialize
    // SET_INSTRUCTION(
    //    data, 0x00000c54,
    //    MOV_X0_XZR); // __libnx_init_time, doesn't have to be skipped if
    // _smCmifCmdInPid is skipped, "mrs x4, cntpct_el0"
    // SET_INSTRUCTION(data, 758, 0x94001946, MOV_X0_XZR); // fsInitialize
    // SET_INSTRUCTION(data, 0x00007020,
    //                MOV_X0_XZR); // sessionmgrAttachClient, loops infinitely
    // waiting for __builtin_ffs to return >= 0
    // SET_INSTRUCTION(data, 830, 0x14002e82, MOV_X0_XZR); // __libc_init_array
    // SET_INSTRUCTION(data, 831, 0x00000000, RET); // __libnx_init
    // SET_INSTRUCTION(data, 839, 0x00000000, RET); // __libnx_exit
    // [x7, #0x18]" (0x18) at
    // 0x000bf80 in _free_r
    // SET_INSTRUCTION(data, 0x00000fb0,
    //                NOP); // setenv, crashes due to "str x23, [x25, #0x8]" at
    // 0x000b6e8 in malloc

    // Needed
    // SET_INSTRUCTION(data, 0x00000c28,
    //                MOV_X0_XZR); // appletInitialize, infinite sleep on exit
    // SET_INSTRUCTION(data, 0x0000161c,
    //                NOP); // _fsdevUnmountDeviceStruct, crashes due to "str
    //                x5,

    // HACK: for testing
    // SET_INSTRUCTION(data, 0x530c, BRK);
    // SET_INSTRUCTION(data, 0x991c, BRK);
    // SET_INSTRUCTION(data, 0x4960, BRK);

    // PRINT_PC_TO_ADDR(0x80000030); // write to code memory

    // CPU
    Hydra::HW::TegraX1::CPU::CPUBase* cpu;
    // TODO: choose based on CPU backend
    {
        cpu = new Hydra::HW::TegraX1::CPU::Hypervisor::CPU();
    }

    // GPU
    Hydra::HW::TegraX1::GPU::GPU gpu(cpu->GetMMU());

    // Display
    // TODO: instantiate a subclass instead
    Hydra::HW::Display::DisplayBase* builtin_display =
        new Hydra::HW::Display::DisplayBase();

    // Bus
    Hydra::HW::Bus bus;
    bus.ConnectDisplay(builtin_display, 0);

    // Horizon OS
    Hydra::Horizon::OS os(bus, cpu->GetMMU());

    // Main thread
    Hydra::HW::TegraX1::CPU::ThreadBase* thread = cpu->CreateThread();
    os.GetKernel().ConfigureThread(thread);

    // Load ROM
    os.LoadROM(rom, thread);

    // Run
    thread->Run();

    return 0;
}
