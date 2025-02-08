#include "horizon/horizon.hpp"
#include "hypervisor/hypervisor.hpp"

/*
void PrintCodeAsCArray(const std::span<u32>& code, usize limit) {
    printf("std::vector<u32> test_code = {\n");
    for (int i = 0; i < code.size() && i < limit; i++) {
        printf("0x%08x, ", code[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
    }
    printf("};\n");
}
*/

/*
std::vector<u32> test_code = {
    0xf100001f, // cmp x0, #0x0
    0xba411824, // ccmn       x1, #0x1, #0x4
    0xD4000003, // smc #0
    // 0x01, 0x00, 0x00, 0xD4, // svc #0
    0xD4000002, // hvc #0
    0xD4200000, // brk #0
};
*/

/*
std::vector<u32> test_code = {
    0xf100001f,
    0xba411824,
    0x54000040,
    0x14000bc8,
    0xaa0003f9,
    0xaa0103fa,
    0xaa1e03fb,
    0x910003fc,
    0x10fffb00,
    0x10000261,
    0xD4200000, // 0x94000176
    0x900000a9,
    0xf9078d3c,
    0xaa1903e0,
    0xaa1a03e1,
    0xaa1b03e2,
    0x9400031c,
    0x900000a0,
    0xb94f3000,
    0x900000a1,
    0xf9479421,
    0xf000009e,
    0xf94267de,
    0x14000029,
};
*/

// HACK
const std::string path =
    "/Users/samuliak/Documents/deko3d_examples/build/0_hello_world.nro";

// HACK
// #define PRINT_ADDR_TO_INDEX(addr) \
//    printf("PRINT_ADDR_TO_INDEX: %u\n", (addr - 0x80) / 4)
// #define PRINT_INDEX_TO_ADDR(addr) \
//    printf("PRINT_INDEX_TO_ADDR: 0x%08x\n", addr * 4 + 0x80)
// #define PRINT_PC_TO_ADDR(pc) \
//    printf("PRINT_PC_TO_ADDR: 0x%016x\n", pc - 0x80000000 + 0x80)

// HACK
void SET_INSTRUCTION(u32* data, i64 addr /*, u32 old_instruction*/,
                     u32 new_instruction) {
    /*
    printf("ORIGINAL: 0x%08x\n", data[index]);
    for (i64 i = index; i < index + 256; i++) {
        if (data[i] == old_instruction) {
            printf("OFFSET: %lli\n", i - index);
            data[i] = new_instruction;
            return;
        }
    }

    throw;
    */
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
    auto ifs = OpenFile(path, size);
    BinaryReader reader(ifs, size);
    Rom* rom = ParseNRO(reader);
    ifs.close();

    // HACK
    /*
    std::vector<u8> test_code_bytes(test_code.size() * sizeof(u32));
    std::memcpy(test_code_bytes.data(), test_code.data(),
                test_code_bytes.size());
    for (int i = 0; i < test_code_bytes.size(); i++) {
        printf("%02X ", test_code_bytes[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    printf("\n");
    */

    // HACK
    u32* data = (u32*)(rom->GetRom().data() + rom->GetTextOffset());
    // data[10] = NOP; // __nx_dynamic
    //  data[16] = NOP; // __libnx_init
    // SET_INSTRUCTION(data, 3176, 0xa9be7bfd, RET); // mutexLock
    // SET_INSTRUCTION(data, 3208, 0xd53bd061, RET); // mutexUnlock
    //  SET_INSTRUCTION(data, 9148, 0x97ffff24, MOV_X0_XZR); // _smCmifCmdInPid
    SET_INSTRUCTION(data, 0x00000c28,
                    MOV_X0_XZR); // appletInitialize, exits with weird stack
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
    SET_INSTRUCTION(data, 0x0000161c,
                    NOP); // _fsdevUnmountDeviceStruct, crashes due to "str x5,
    // [x7, #0x18]" (0x18) at
    // 0x000bf80 in _free_r
    // SET_INSTRUCTION(data, 0x00000fb0,
    //                NOP); // setenv, crashes due to "str x23, [x25, #0x8]" at
    // 0x000b6e8 in malloc

    // HACK: for testing
    // SET_INSTRUCTION(data, 0xbf60, BRK);

    // PRINT_PC_TO_ADDR(0x80000030); // write to code memory

    // Horizon OS
    Hydra::Horizon::OS horizon;

    // Hypervisor
    Hydra::Hypervisor::Hypervisor hypervisor(horizon);
    hypervisor.LoadROM(rom);
    hypervisor.Run();

    /*
    std::vector<u32> print_code(rom->GetText().size() / sizeof(u32));
    std::memcpy(print_code.data(), rom->GetText().data(),
                rom->GetText().size());
    PrintCodeAsCArray(print_code, 24);
    */

    return 0;
}
