#pragma once

#include "horizon/services/nvdrv/const.hpp"

#define IOCTL_PARAMS Reader *reader, Writer *writer, u32 nr, NvResult &result

#define IOCTL_CASE(nr, func)                                                   \
    case nr: {                                                                 \
        func##Data* data = nullptr;                                            \
        if (reader)                                                            \
            data = reader->ReadPtr<func##Data>();                              \
        func(*data, result);                                                   \
        if (writer)                                                            \
            writer->Write(*data);                                              \
        break;                                                                 \
    }

#define DEFINE_IOCTL_TABLE(fd, ...)                                            \
    void fd::Ioctl(IOCTL_PARAMS) {                                             \
        switch (nr) {                                                          \
            FOR_EACH_0_2(IOCTL_CASE, __VA_ARGS__)                              \
        default:                                                               \
            LOG_WARNING(HorizonServices, "Unknown ioctl 0x{:08x}", nr);        \
            break;                                                             \
        }                                                                      \
    }

#define IOCTL_OUT_MEMBER_COPY(member) member = o.member.Get();

#define DECLARE_IOCTL(ioctl, args, ...)                                        \
    struct ioctl##Data {                                                       \
        args;                                                                  \
        void operator=(const ioctl##Data& o) {                                 \
            FOR_EACH_0_1(IOCTL_OUT_MEMBER_COPY, __VA_ARGS__)                   \
        }                                                                      \
    } __attribute__((packed));                                                 \
    void ioctl(ioctl##Data& data, NvResult& result);

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class FdBase {
  public:
    virtual void Ioctl(IOCTL_PARAMS) = 0;
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
