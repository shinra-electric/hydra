#include "emulation_context.hpp"

// HACK
void SET_INSTRUCTION(u32* data, i64 addr, u32 new_instruction) {
    data[addr / 4] = new_instruction;
}

// HACK
#define NOP 0xd503201fu
#define RET 0xd65f03c0u
#define MOV_X0_XZR 0xaa1f03e0u
#define BRK 0xd4200000u

namespace Hydra {

EmulationContext::EmulationContext(const std::string& rom_filename) {
    // Parse file
    usize size;
    auto ifs = Hydra::open_file(rom_filename, size);
    // std::vector<u8> d(size);
    // ifs.read((char*)d.data(), size);
    // Logging::log(Logging::Level::Debug, "AAA: 0x%llx", *((u64*)(d.data() +
    // 0x17498))); return 0;
    BinaryReader reader(ifs, size);
    Rom* rom = ParseNRO(reader);
    ifs.close();

    // HACK
    u32* data = (u32*)(rom->GetRom().data());
    // SET_INSTRUCTION(data, 0x00000c28,
    //                MOV_X0_XZR); // appletInitialize, infinite sleep on exit
    // SET_INSTRUCTION(data, 0x0000161c,
    //                NOP); // _fsdevUnmountDeviceStruct, crashes due to "str
    //                x5,

    // HACK: for testing
    // SET_INSTRUCTION(data, 0x6af50, BRK);

    // Emulation
    // TODO: choose based on CPU backend
    {
        cpu = new Hydra::HW::TegraX1::CPU::Hypervisor::CPU();
    }

    gpu = new Hydra::HW::TegraX1::GPU::GPU(cpu->GetMMU());

    builtin_display = new Hydra::HW::Display::Display();

    bus = new Hydra::HW::Bus();
    bus->ConnectDisplay(builtin_display, 0);

    os = new Hydra::Horizon::OS(*bus, cpu->GetMMU());

    // Load ROM
    os->LoadROM(rom);
}

EmulationContext::~EmulationContext() {
    for (auto t : threads) {
        // Force the thead to exit
        delete t;
    }
}

void EmulationContext::Start() {
    std::thread* t = new std::thread([&]() {
        // Main thread
        Hydra::HW::TegraX1::CPU::ThreadBase* main_thread = cpu->CreateThread();
        os->GetKernel().ConfigureMainThread(main_thread);

        // Run
        main_thread->Run();

        // Cleanup
        delete main_thread;

        // Notify that emulation has ended
        is_running = false;
    });

    is_running = true;
}

} // namespace Hydra
