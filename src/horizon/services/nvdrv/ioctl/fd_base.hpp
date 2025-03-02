#pragma once

#include "horizon/services/nvdrv/const.hpp"

#define IOCTL_PARAMS                                                           \
    Reader *reader, Writer *writer, u32 type, u32 nr, NvResult &result

#define IOCTL_CASE(nr, func)                                                   \
    case nr: {                                                                 \
        /* HACK: for some reason, why need to do it this weird way, otherwise  \
         * the data will be corrupt */                                         \
        func##Data data_dummy;                                                 \
        func##Data* data = &data_dummy;                                        \
        if (reader)                                                            \
            data = reader->ReadPtr<func##Data>();                              \
        func(*data, result);                                                   \
        if (writer)                                                            \
            writer->Write(*data);                                              \
        break;                                                                 \
    }

#define DEFINE_IOCTL_TABLE_ENTRY(type, ...)                                    \
    case type:                                                                 \
        switch (nr) {                                                          \
            FOR_EACH_0_2(IOCTL_CASE, __VA_ARGS__)                              \
        default:                                                               \
            LOG_WARNING(HorizonServices,                                       \
                        "Unknown ioctl nr 0x{:02x} for type 0x{:02x}", nr,     \
                        type);                                                 \
            break;                                                             \
        }                                                                      \
        break;

#define DEFINE_IOCTL_TABLE(fd, ...)                                            \
    void fd::Ioctl(IOCTL_PARAMS) {                                             \
        switch (type) {                                                        \
            __VA_ARGS__                                                        \
        default:                                                               \
            LOG_WARNING(HorizonServices, "Unknown ioctl type 0x{:02x}", type); \
            break;                                                             \
        }                                                                      \
    }

#define IOCTL_OUT_MEMBER_COPY(member) member = o.member.Get();

#define DECLARE_IOCTL_IMPL(ioctl, attr, args, ...)                             \
    struct ioctl##Data {                                                       \
        args;                                                                  \
        ioctl##Data() {}                                                       \
        void operator=(const ioctl##Data& o) {                                 \
            FOR_EACH_0_1(IOCTL_OUT_MEMBER_COPY, __VA_ARGS__)                   \
        }                                                                      \
    } __attribute__((packed));                                                 \
    attr void ioctl(ioctl##Data& data, NvResult& out_result)

#define DECLARE_IOCTL(ioctl, args, ...)                                        \
    DECLARE_IOCTL_IMPL(ioctl, , args, __VA_ARGS__)

#define DECLARE_VIRTUAL_IOCTL(ioctl, args, ...)                                \
    DECLARE_IOCTL_IMPL(ioctl, virtual, args, __VA_ARGS__) {                    \
        LOG_ERROR(HorizonServices, "Not implemented for this fb");             \
    }

namespace Hydra::Horizon::Services::NvDrv::Ioctl {

class FdBase {
  public:
    virtual void Ioctl(IOCTL_PARAMS) = 0;
    virtual void QueryEvent(u32 event_id_u32, HandleId& out_handle_id,
                            NvResult& out_result) {
        LOG_WARNING(HorizonServices, "Unknown event id {}", event_id_u32);

        out_result = NvResult::NotSupported;
    }
};

} // namespace Hydra::Horizon::Services::NvDrv::Ioctl
