#pragma once

#include "horizon/const.hpp"

enum class IoctlResult : u32 {
    Success = 0,
};

#define IOCTL_PARAMS Reader &reader, Writer &writer, u32 nr, IoctlResult &result

#define IOCTL_CASE(nr, func)                                                   \
    case nr: {                                                                 \
        auto data = reader.Read<func##Data>();                                 \
        func(data, result);                                                    \
        writer.Write(data);                                                    \
        break;                                                                 \
    }

#define DEFINE_IOCTL_TABLE(c, ...)                                             \
    void c::Ioctl(IOCTL_PARAMS) {                                              \
        result = IoctlResult::Success;                                         \
        switch (nr) {                                                          \
            FOR_EACH_0_2(IOCTL_CASE, __VA_ARGS__)                              \
        default:                                                               \
            LOG_WARNING(HorizonServices, "Unknown ioctl {}", nr);              \
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
    };                                                                         \
    void ioctl(ioctl##Data& data, IoctlResult& result);

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class IoctlBase {
  public:
    virtual void Ioctl(IOCTL_PARAMS) = 0;
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
