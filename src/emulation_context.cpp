#include "emulation_context.hpp"

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

    // Run
    // TODO: find out why running CPU in a separate thread alongside the display
    // sometimes causes a crash
    std::thread* t = new std::thread([&]() {
        // Main thread
        Hydra::HW::TegraX1::CPU::ThreadBase* main_thread = cpu->CreateThread();
        os->GetKernel().ConfigureMainThread(main_thread);

        // Run
        main_thread->Run();

        // Cleanup
        delete main_thread;
    });
}

EmulationContext::~EmulationContext() {
    for (auto t : threads) {
        // HACK
        delete t;
        // t.join();
    }
}

} // namespace Hydra
