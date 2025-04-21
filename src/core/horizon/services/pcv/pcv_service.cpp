#include "core/horizon/services/pcv/pcv_service.hpp"

#include "core/hw/tegra_x1/cpu/const.hpp"

namespace Hydra::Horizon::Services::Pcv {

namespace {

enum class ModuleId : u32 {
    Cpu = 0,
    Gpu = 1,
    I2s1 = 2,
    I2s2 = 3,
    I2s3 = 4,
    Pwm = 5,
    I2c1 = 6,
    I2c2 = 7,
    I2c3 = 8,
    I2c4 = 9,
    I2c5 = 10,
    I2c6 = 11,
    Spi1 = 12,
    Spi2 = 13,
    Spi3 = 14,
    Spi4 = 15,
    Disp1 = 16,
    Disp2 = 17,
    Isp = 18,
    Vi = 19,
    Sdmmc1 = 20,
    Sdmmc2 = 21,
    Sdmmc3 = 22,
    Sdmmc4 = 23,
    Owr = 24,
    Csite = 25,
    Tsec = 26,
    Mselect = 27,
    Hda2codec2x = 28,
    Actmon = 29,
    I2cSlow = 30,
    Sor1 = 31,
    Sata = 32,
    Hda = 33,
    XusbCoreHostSrc = 34,
    XusbFalconSrc = 35,
    XusbFsSrc = 36,
    XusbCoreDevSrc = 37,
    XusbSsSrc = 38,
    UartA = 39,
    UartB = 40,
    UartC = 41,
    UartD = 42,
    Host1x = 43,
    Entropy = 44,
    SocTherm = 45,
    Vic = 46,
    Nvenc = 47,
    Nvjpg = 48,
    Nvdec = 49,
    Qspi = 50,
    ViI2c = 51,
    Tsecb = 52,
    Ape = 53,
    AudioDsp = 54,
    AudioUart = 55,
    Emc = 56,
    Plle = 57,
    PlleHwSeq = 58,
    Dsi = 59,
    Maud = 60,
    Dpaux1 = 61,
    MipiCal = 62,
    UartFstMipiCal = 63,
    Osc = 64,
    SysBus = 65,
    SorSafe = 66,
    XusbSs = 67,
    XusbHost = 68,
    XusbDevice = 69,
    Extperiph1 = 70,
    Ahub = 71,
    Hda2hdmicodec = 72,
    Gpuaux = 73,
    UsbD = 74,
    Usb2 = 75,
    Pcie = 76,
    Afi = 77,
    PciExClk = 78,
    PExUsbPhy = 79,
    XUsbPadCtl = 80,
    Apbdma = 81,
    Usb2TrkClk = 82,
    XUsbIoPll = 83,
    XUsbIoPllHwSeq = 84,
    Cec = 85,
    Extperiph2 = 86,
    OscClk = invalid<u32>(), // TODO
};

}

} // namespace Hydra::Horizon::Services::Pcv

ENABLE_ENUM_FORMATTING(
    Hydra::Horizon::Services::Pcv::ModuleId, Cpu, "CPU", Gpu, "GPU", I2s1,
    "I2S1", I2s2, "I2S2", I2s3, "I2S3", Pwm, "PWM", I2c1, "I2C1", I2c2, "I2C2",
    I2c3, "I2C3", I2c4, "I2C4", I2c5, "I2C5", I2c6, "I2C6", Spi1, "SPI1", Spi2,
    "SPI2", Spi3, "SPI3", Spi4, "SPI4", Disp1, "DISP1", Disp2, "DISP2", Isp,
    "ISP", Vi, "VI", Sdmmc1, "SDMMC1", Sdmmc2, "SDMMC2", Sdmmc3, "SDMMC3",
    Sdmmc4, "SDMMC4", Owr, "OWR", Csite, "CSITE", Tsec, "TSEC", Mselect,
    "MSELECT", Hda2codec2x, "HDA2CODEC2X", Actmon, "ACTMON", I2cSlow, "I2CSLOW",
    Sor1, "SOR1", Sata, "SATA", Hda, "HDA", XusbCoreHostSrc,
    "XUSB_CORE_HOST_SRC", XusbFalconSrc, "XUSB_FALCON_SRC", XusbFsSrc,
    "XUSB_FS_SRC", XusbCoreDevSrc, "XUSB_CORE_DEV_SRC", XusbSsSrc,
    "XUSB_SS_SRC", UartA, "UARTA", UartB, "UARTB", UartC, "UARTC", UartD,
    "UARTD", Host1x, "HOST1X", Entropy, "ENTROPY", SocTherm, "SOC_THERM", Vic,
    "VIC", Nvenc, "NVENC", Nvjpg, "NVJPG", Nvdec, "NVDEC", Qspi, "QSPI", ViI2c,
    "VI_I2C", Tsecb, "TSECB", Ape, "APE", AudioDsp, "AUDIO_DSP", AudioUart,
    "AUDIO_UART", Emc, "EMC", Plle, "PLLE", PlleHwSeq, "PLLE_HW_SEQ", Dsi,
    "DSI", Maud, "MAUD", Dpaux1, "DPAUX1", MipiCal, "MIPI_CAL", UartFstMipiCal,
    "UART_FST_MIPI_CAL", Osc, "OSC", SysBus, "SYS_BUS", SorSafe, "SOR_SAFE",
    XusbSs, "XUSB_SS", XusbHost, "XUSB_HOST", XusbDevice, "XUSB_DEVICE",
    Extperiph1, "EXTPERIPH1", Ahub, "AHUB", Hda2hdmicodec, "HDA2HDMICODEC",
    Gpuaux, "GPUAUX", UsbD, "USBD", Usb2, "USB2", Pcie, "PCIE", Afi, "AFI",
    PciExClk, "PCI_EX_CLK", PExUsbPhy, "PEX_USB_PHY", XUsbPadCtl,
    "XUSB_PAD_CTL", Apbdma, "APBDMA", Usb2TrkClk, "USB2_TRK_CLK", XUsbIoPll,
    "XUSB_IO_PLL", XUsbIoPllHwSeq, "XUSB_IO_PLL_HW_SEQ", Cec, "CEC", Extperiph2,
    "EXTPERIPH2", OscClk, "OSC_CLK")

namespace Hydra::Horizon::Services::Pcv {

DEFINE_SERVICE_COMMAND_TABLE(IPcvService, 3, GetClockRate)

void IPcvService::GetClockRate(REQUEST_COMMAND_PARAMS) {
    const auto module_id = readers.reader.Read<ModuleId>();

    u32 clock_rate = 0;
    switch (module_id) {
    case ModuleId::Cpu:
        clock_rate = HW::TegraX1::CPU::CLOCK_RATE_HZ;
        break;
    default:
        LOG_NOT_IMPLEMENTED(HorizonServices, "Clock rate for {}", module_id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::Pcv
