#pragma once

namespace hydra::horizon::services::i2c {

enum class I2cDevice : u32 {
    ClassicController = 0,
    Ftm3bd56 = 1,
    Tmp451 = 2,
    Nct72 = 3,
    Alc5639 = 4,
    Max77620Rtc = 5,
    Max77620Pmic = 6,
    Max77621Cpu = 7,
    Max77621Gpu = 8,
    Bq24193 = 9,
    Max17050 = 10,
    Bm92t30mwv = 11,
    Ina226Vdd15v0Hb = 12,
    Ina226VsysCpuDs = 13,
    Ina226VsysGpuDs = 14,
    Ina226VsysDdrDs = 15,
    Ina226VsysAp = 16,
    Ina226VsysBlDs = 17,
    Bh1730 = 18,
    Ina226VsysCore = 19,
    Ina226Soc1V8 = 20,
    Ina226Lpddr1V8 = 21,
    Ina226Reg1V32 = 22,
    Ina226Vdd3V3Sys = 23,
    HdmiDdc = 24,
    HdmiScdc = 25,
    HdmiHdcp = 26,
    Fan53528 = 27,
    Max77812_3 = 28,
    Max77812_2 = 29,
    Ina226VddDdr0V6 = 30,
};

} // namespace hydra::horizon::services::i2c

ENABLE_ENUM_FORMATTING(
    hydra::horizon::services::i2c::I2cDevice, ClassicController,
    "classic controller", Ftm3bd56, "ftm3bd56", Tmp451, "tmp451", Nct72,
    "nct72", Alc5639, "alc5639", Max77620Rtc, "max77620 rtc", Max77620Pmic,
    "max77620 pmic", Max77621Cpu, "max77621 cpu", Max77621Gpu, "max77621 gpu",
    Bq24193, "bq24193", Max17050, "max17050", Bm92t30mwv, "bm92t30mwv",
    Ina226Vdd15v0Hb, "ina226 vdd 1.5v0 hb", Ina226VsysCpuDs,
    "ina226 vsys cpu ds", Ina226VsysGpuDs, "ina226 vsys gpu ds",
    Ina226VsysDdrDs, "ina226 vsys ddr ds", Ina226VsysAp, "ina226 vsys ap",
    Ina226VsysBlDs, "ina226 vsys bl ds", Bh1730, "bh1730", Ina226VsysCore,
    "ina226 vsys core", Ina226Soc1V8, "ina226 soc 1v8", Ina226Lpddr1V8,
    "ina226 lpddr 1v8", Ina226Reg1V32, "ina226 reg 1v32", Ina226Vdd3V3Sys,
    "ina226 vdd 3v3 sys", HdmiDdc, "hdmi ddc", HdmiScdc, "hdmi scdc", HdmiHdcp,
    "hdmi hdcp", Fan53528, "fan53528", Max77812_3, "max77812_3", Max77812_2,
    "max77812_2", Ina226VddDdr0V6, "ina226 vdd ddr 0v6")
