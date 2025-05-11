#pragma once

#include "core/horizon/kernel/service_base.hpp"

#define SERVICE_COMMAND_CASE(service, id, func)                                \
    case id:                                                                   \
        LOG_DEBUG(HorizonServices, #func);                                     \
        return invoke_command(context, *this, &service::func);

#define DEFINE_SERVICE_COMMAND_TABLE(service, ...)                             \
    result_t service::RequestImpl(RequestContext& context, u32 id) {           \
        switch (id) {                                                          \
            FOR_EACH_1_2(SERVICE_COMMAND_CASE, service, __VA_ARGS__)           \
        default:                                                               \
            LOG_WARN(HorizonServices, "Unknown request {}", id);               \
            return MAKE_RESULT(Svc, 0); /* TODO */                             \
        }                                                                      \
    }

#define STUB_REQUEST_COMMAND(name)                                             \
    result_t name() {                                                          \
        LOG_FUNC_STUBBED(HorizonServices);                                     \
        return RESULT_SUCCESS;                                                 \
    }

namespace Hydra::Horizon::Services {

using result_t = Kernel::result_t;
using add_service_fn_t = Kernel::add_service_fn_t;
using get_service_fn_t = Kernel::get_service_fn_t;
using RequestContext = Kernel::RequestContext;
using ServiceBase = Kernel::ServiceBase;

enum class BufferAttr {
    AutoSelect,
    MapAlias,
    HipcPointer,
};

template <BufferAttr attr_> class InBuffer {
  public:
    static constexpr BufferAttr attr = attr_;

    Reader* reader;

    InBuffer() : reader{nullptr} {}
    InBuffer(Reader& reader_) : reader{&reader_} {}
};

template <BufferAttr attr_> class OutBuffer {
  public:
    static constexpr BufferAttr attr = attr_;

    Writer* writer;

    OutBuffer() : writer{nullptr} {}
    OutBuffer(Writer& writer_) : writer{&writer_} {}
};

enum class HandleAttr {
    Copy,
    Move,
};

template <HandleAttr attr_> class InHandle {
  public:
    static constexpr HandleAttr attr = attr_;

    InHandle(handle_id_t handle_id_) : handle_id{handle_id_} {}

    operator handle_id_t() const { return handle_id; }

  private:
    handle_id_t handle_id;
};

template <HandleAttr attr_> class OutHandle {
  public:
    static constexpr HandleAttr attr = attr_;

    OutHandle() : handle_id{nullptr} {}
    OutHandle(handle_id_t& handle_id_) : handle_id{&handle_id_} {}

    operator handle_id_t&() { return *handle_id; }

    void operator=(handle_id_t other) { *handle_id = other; }

  private:
    handle_id_t* handle_id;
};

enum class ArgumentType {
    InData,
    OutData,
    InBuffer,
    OutBuffer,
    InHandle,
    OutHandle,
    AddServiceFn,
    Service,
};

template <typename T> struct arg_traits;

template <typename T> struct arg_traits {
    static constexpr ArgumentType type = ArgumentType::InData;
};

template <typename T> struct arg_traits<T*> {
    static constexpr ArgumentType type = ArgumentType::OutData;
    using BaseType = T;
};

template <BufferAttr attr> struct arg_traits<InBuffer<attr>> {
    static constexpr ArgumentType type = ArgumentType::InBuffer;
};

template <BufferAttr attr> struct arg_traits<OutBuffer<attr>> {
    static constexpr ArgumentType type = ArgumentType::OutBuffer;
};

template <HandleAttr attr> struct arg_traits<InHandle<attr>> {
    static constexpr ArgumentType type = ArgumentType::InHandle;
};

template <HandleAttr attr> struct arg_traits<OutHandle<attr>> {
    static constexpr ArgumentType type = ArgumentType::OutHandle;
};

template <> struct arg_traits<add_service_fn_t> {
    static constexpr ArgumentType type = ArgumentType::AddServiceFn;
};

template <> struct arg_traits<ServiceBase*> {
    static constexpr ArgumentType type = ArgumentType::Service;
};

template <typename CommandArguments, u32 in_buffer_index = 0,
          u32 out_buffer_index = 0, u32 arg_index = 0>
void read_arg(RequestContext& context, CommandArguments& args) {
    if constexpr (arg_index >= std::tuple_size_v<CommandArguments>) {
        return;
    } else {
        using Arg = std::tuple_element_t<arg_index, CommandArguments>;
        using traits = arg_traits<Arg>;

        auto& arg = std::get<arg_index>(args);

        if constexpr (traits::type == ArgumentType::InData) {
            arg = context.readers.reader.Read<Arg>();

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::OutData) {
            // TODO: check this
            arg = context.writers.writer.WritePtr<typename traits::BaseType>();

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::InBuffer) {
            std::vector<Reader>* reader_group;
            if constexpr (Arg::attr == BufferAttr::AutoSelect) {
                // TODO: auto select
                reader_group = &context.readers.send_buffers_readers;
            } else if constexpr (Arg::attr == BufferAttr::MapAlias) {
                reader_group = &context.readers.send_buffers_readers;
            } else /*if constexpr (Arg::attr == BufferAttr::HipcPointer)*/ {
                reader_group = &context.readers.send_statics_readers;
            }

            if (in_buffer_index < reader_group->size())
                arg = Arg((*reader_group)[in_buffer_index]);

            // Next
            read_arg<CommandArguments, in_buffer_index + 1, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::OutBuffer) {
            std::vector<Writer>* writer_group;
            if constexpr (Arg::attr == BufferAttr::AutoSelect) {
                // TODO: auto select
                writer_group = &context.writers.recv_buffers_writers;
            } else if constexpr (Arg::attr == BufferAttr::MapAlias) {
                writer_group = &context.writers.recv_buffers_writers;
            } else /*if constexpr (Arg::attr == BufferAttr::HipcPointer)*/ {
                writer_group = &context.writers.recv_list_writers;
            }

            if (out_buffer_index < writer_group->size())
                arg = Arg((*writer_group)[out_buffer_index]);

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index + 1,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::InHandle) {
            // TODO

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::OutHandle) {
            handle_id_t* handle_id;
            if constexpr (Arg::attr == HandleAttr::Copy) {
                handle_id =
                    context.writers.copy_handles_writer.WritePtr<handle_id_t>();
            } else /*if constexpr (Arg::attr == HandleAttr::Move)*/ {
                handle_id =
                    context.writers.move_handles_writer.WritePtr<handle_id_t>();
            }

            arg = Arg(*handle_id);

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else if constexpr (traits::type == ArgumentType::AddServiceFn) {
            arg = context.add_service;

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        } else /*if constexpr (traits::type == ArgumentType::Service)*/ {
            ASSERT_DEBUG(context.readers.objects_reader, HorizonServices,
                         "Objects reader is null");
            auto service_handle_id =
                context.readers.objects_reader->Read<handle_id_t>();
            arg = context.get_service(service_handle_id);

            // Next
            read_arg<CommandArguments, in_buffer_index, out_buffer_index,
                     arg_index + 1>(context, args);
            return;
        }
    }
}

template <typename T> struct function_traits;

template <typename R, typename... Args> struct function_traits<R(Args...)> {
    using return_type = R;
    static constexpr size_t arg_count = sizeof...(Args);

    template <size_t N> struct arg {
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };

    using args_tuple = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)> {};

template <typename C, typename R, typename... Args>
struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)> {
};

template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>>
    : function_traits<R(Args...)> {};

template <typename Class, typename... Args, size_t... Is>
result_t invoke_command_with_args(RequestContext& context, Class& instance,
                                  result_t (Class::*func)(Args...),
                                  std::index_sequence<Is...>) {
    using traits = function_traits<decltype(func)>;

    auto args = std::tuple<typename traits::template arg<Is>::type...>();
    read_arg(context, args);

    auto callable = [&]<typename... CallArgs>(CallArgs&... args) {
        return (instance.*func)(args...);
    };

    return std::apply(callable, args);
}

template <typename Class, typename... Args>
result_t invoke_command(RequestContext& context, Class& instance,
                        result_t (Class::*func)(Args...)) {
    using traits = function_traits<decltype(func)>;

    constexpr auto indices = std::make_index_sequence<traits::arg_count>{};

    return invoke_command_with_args(context, instance, func, indices);
}

} // namespace Hydra::Horizon::Services
