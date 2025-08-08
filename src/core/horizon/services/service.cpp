#include "core/horizon/services/service.hpp"

#include "core/horizon/kernel/hipc/client_session.hpp"
#include "core/horizon/kernel/hipc/cmif.hpp"
#include "core/horizon/kernel/hipc/server_session.hpp"
#include "core/horizon/kernel/hipc/session.hpp"
#include "core/horizon/kernel/process.hpp"
#include "core/horizon/services/server.hpp"

namespace hydra::horizon::services {

IService::~IService() {
    if (subservice_pool)
        delete subservice_pool;
}

void IService::HandleRequest(kernel::Process* caller_process, uptr ptr) {
    // HIPC header
    auto hipc_in = kernel::hipc::parse_request((void*)ptr);
    auto command_type =
        static_cast<kernel::hipc::cmif::CommandType>(hipc_in.meta.type);
    const bool is_tipc =
        (command_type >= kernel::hipc::cmif::CommandType::TipcCommandRegion);
    if (!is_tipc)
        hipc_in.data.data_words =
            kernel::hipc::cmif::align_data_start(hipc_in.data.data_words);

    // Scratch memory
    u8 scratch_buffer[0x200];
    u8 scratch_buffer_objects[0x100];
    u8 scratch_buffer_copy_handles[0x100];
    u8 scratch_buffer_move_handles[0x100];

    // Request context
    kernel::hipc::Readers readers(caller_process->GetMmu(), hipc_in);
    kernel::hipc::Writers writers(caller_process->GetMmu(), hipc_in,
                                  scratch_buffer, scratch_buffer_objects,
                                  scratch_buffer_copy_handles,
                                  scratch_buffer_move_handles);
    RequestContext context{
        caller_process,
        readers,
        writers,
    };

    // Dispatch
    kernel::hipc::cmif::CommandType response_command_type{
        kernel::hipc::cmif::CommandType::Invalid};
    bool should_respond = true;
    switch (command_type) {
    case kernel::hipc::cmif::CommandType::Close:
    case kernel::hipc::cmif::CommandType::TipcClose: // TODO: is this the same
                                                     // as regular close?
        Close();
        should_respond = false;
        break;
    case kernel::hipc::cmif::CommandType::Request:
    case kernel::hipc::cmif::CommandType::RequestWithContext: {
        // TODO: how is RequestWithContext different?
        Request(context);
        // TODO: respond command type 0?
        break;
    }
    case kernel::hipc::cmif::CommandType::Control:
    case kernel::hipc::cmif::CommandType::ControlWithContext:
        // TODO: how is ControlWithContext different?
        Control(caller_process, readers, writers);
        break;
    default:
        if (command_type >=
            kernel::hipc::cmif::CommandType::TipcCommandRegion) {
            const auto command_id =
                (u32)command_type -
                (u32)kernel::hipc::cmif::CommandType::TipcCommandRegion;
            TipcRequest(context, command_id);
            response_command_type = command_type; // Same as input
            break;
        }

        LOG_WARN(Services, "Unknown command {}", command_type);
        break;
    }

    // Response
    if (should_respond) {
        // HIPC header
#define GET_ARRAY_SIZE(writer)                                                 \
    static_cast<u32>(align(writers.writer.GetWrittenSize(), (usize)4) /        \
                     sizeof(u32))

#define WRITE_ARRAY(writer, ptr)                                               \
    if (ptr) {                                                                 \
        memcpy(ptr, writers.writer.GetBase(),                                  \
               writers.writer.GetWrittenSize());                               \
    }

        kernel::hipc::Metadata meta{
            .type = (u32)response_command_type,
            .num_data_words =
                GET_ARRAY_SIZE(writer) + GET_ARRAY_SIZE(objects_writer),
            .num_copy_handles = GET_ARRAY_SIZE(copy_handles_writer),
            .num_move_handles = GET_ARRAY_SIZE(move_handles_writer)};
        auto response = kernel::hipc::make_request((void*)ptr, meta);
        if (!is_tipc)
            response.data_words =
                kernel::hipc::cmif::align_data_start(response.data_words);

        u8* data_start = reinterpret_cast<u8*>(response.data_words);
        if (command_type <
            kernel::hipc::cmif::CommandType::TipcCommandRegion) // TODO: is this
                                                                // really how it
                                                                // works?
            data_start = align_ptr(data_start, 0x10);
        WRITE_ARRAY(writer, data_start);
        if (writers.objects_writer.GetWrittenSize() != 0) {
            memcpy(data_start + GET_ARRAY_SIZE(writer) * sizeof(u32),
                   writers.objects_writer.GetBase(),
                   writers.objects_writer.GetWrittenSize());
        }
        WRITE_ARRAY(copy_handles_writer, response.copy_handles);
        WRITE_ARRAY(move_handles_writer, response.move_handles);

#undef GET_ARRAY_SIZE
#undef WRITE_ARRAY
    }
}

void IService::AddService(RequestContext& context, IService* service) {
    service->server = server;
    if (is_domain) {
        // Convert to domain
        service->is_domain = true;
        service->parent = parent;

        const auto handle_id = AddSubservice(service);
        context.writers.objects_writer.Write(handle_id);
    } else {
        // Create new session
        auto server_session = new kernel::hipc::ServerSession();
        auto client_session = new kernel::hipc::ClientSession();
        auto session =
            new kernel::hipc::Session(server_session, client_session);

        // Register server side
        server->RegisterSession(server_session, service);

        // Register client side
        const auto handle_id = context.process->AddHandle(client_session);
        context.writers.move_handles_writer.Write(handle_id);
    }
}

IService* IService::GetService(RequestContext& context, handle_id_t handle_id) {
    if (is_domain) {
        return GetSubservice(handle_id);
    } else {
        return server->GetServiceForSession(
            context.process->GetHandle<kernel::hipc::ClientSession>(handle_id)
                ->GetParent()
                ->GetServerSide());
    }
}

void IService::Close() {
    // TODO
    LOG_FUNC_NOT_IMPLEMENTED(Services);
}

void IService::Request(RequestContext& context) {
    if (is_domain) {
        // Domain in
        auto cmif_in =
            context.readers.reader.Read<kernel::hipc::cmif::DomainInHeader>();
        // LOG_DEBUG(Services, "Object ID: 0x{:08x}", cmif_in.object_id);
        auto subservice = GetSubservice(cmif_in.object_id);

        if (cmif_in.num_in_objects != 0) {
            auto objects = context.readers.reader.GetPtr() + cmif_in.data_size;
            context.readers.objects_reader = new Reader(
                objects, cmif_in.num_in_objects * sizeof(handle_id_t));
        }

        kernel::hipc::cmif::write_domain_out_header(context.writers.writer);

        switch (cmif_in.type) {
        case kernel::hipc::cmif::DomainCommandType::SendMessage: {
            subservice->CmifRequest(context);
            break;
        }
        case kernel::hipc::cmif::DomainCommandType::Close:
            // TODO: actually free the service
            FreeSubservice(cmif_in.object_id);
            LOG_DEBUG(Kernel, "Closed subservice");
            break;
        default:
            LOG_WARN(Kernel, "Unknown domain command type {}", cmif_in.type);
            break;
        }
    } else {
        CmifRequest(context);
    }
}

void IService::CmifRequest(RequestContext& context) {
    auto cmif_in = context.readers.reader.Read<kernel::hipc::cmif::InHeader>();
    ASSERT_DEBUG(cmif_in.magic == kernel::hipc::cmif::IN_HEADER_MAGIC, Services,
                 "Invalid CMIF in magic 0x{:08x}", cmif_in.magic);

    auto result = kernel::hipc::cmif::write_out_header(context.writers.writer);
    *result = RequestImpl(context, cmif_in.command_id);
}

void IService::Control(kernel::Process* caller_process,
                       kernel::hipc::Readers& readers,
                       kernel::hipc::Writers& writers) {
    auto cmif_in = readers.reader.Read<kernel::hipc::cmif::InHeader>();
    ASSERT_DEBUG(cmif_in.magic == kernel::hipc::cmif::IN_HEADER_MAGIC, Kernel,
                 "Invalid CMIF in magic 0x{:08x}", cmif_in.magic);

    result_t* result = kernel::hipc::cmif::write_out_header(writers.writer);

    const auto command =
        static_cast<kernel::hipc::cmif::ControlCommandType>(cmif_in.command_id);
    LOG_DEBUG(Services, "Control request {}", command);
    switch (command) {
    case kernel::hipc::cmif::ControlCommandType::ConvertCurrentObjectToDomain: {
        is_domain = true;
        subservice_pool = new DynamicPool<IService*>();
        const auto handle_id = AddSubservice(this);
        writers.writer.Write(handle_id);
        break;
    }
    case kernel::hipc::cmif::ControlCommandType::CloneCurrentObject:
        Clone(caller_process, writers);
        break;
    case kernel::hipc::cmif::ControlCommandType::QueryPointerBufferSize:
        // TODO: let the server specify this
        writers.writer.Write<u16>(0x8000); // The highest known pointer buffer
                                           // size (used by nvservices)
        break;
    case kernel::hipc::cmif::ControlCommandType::CloneCurrentObjectEx:
        // TODO: u32 tag
        Clone(caller_process, writers);
        break;
    default:
        LOG_ERROR(Services, "Unimplemented control request {}", command);
        break;
    }
}

void IService::Clone(kernel::Process* caller_process,
                     kernel::hipc::Writers& writers) {
    // Create new session
    auto server_session = new kernel::hipc::ServerSession();
    auto client_session = new kernel::hipc::ClientSession();
    auto session = new kernel::hipc::Session(server_session, client_session);

    // Register server side
    server->RegisterSession(server_session, this);

    // Register client side
    const auto handle_id = caller_process->AddHandle(client_session);
    writers.move_handles_writer.Write(handle_id);
}

void IService::TipcRequest(RequestContext& context, const u32 command_id) {
    ASSERT_DEBUG(!is_domain, Kernel,
                 "TIPC is not supported for domain services");
    auto result = context.writers.writer.WritePtr<result_t>();
    *result = RequestImpl(context, command_id);
}

} // namespace hydra::horizon::services
