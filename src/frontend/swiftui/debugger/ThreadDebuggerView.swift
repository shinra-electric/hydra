import SwiftUI

struct Message: Hashable {
    let log_level: HydraLogLevel
    let str: String

    var style: any ShapeStyle {
        switch self.log_level {
        case HYDRA_LOG_LEVEL_DEBUG:
            return .cyan
        case HYDRA_LOG_LEVEL_INFO:
            return .white
        case HYDRA_LOG_LEVEL_STUB:
            return .purple
        case HYDRA_LOG_LEVEL_WARNING:
            return .yellow
        case HYDRA_LOG_LEVEL_ERROR:
            return .red
        case HYDRA_LOG_LEVEL_FATAL:
            return .red  // TODO: dark red
        default:
            return .white
        }
    }
}

// TODO: don't store the messages in a separate array?
struct ThreadDebuggerView: View {
    let thread: UnsafeMutableRawPointer

    @State var messages: [Message] = []

    var body: some View {
        VStack {
            Text(String(cString: hydra_debugger_thread_get_name(self.thread)))
            ZStack {
                Color.black
                    .ignoresSafeArea()

                ScrollView(.vertical) {
                    // TODO: lazy
                    /*Lazy*/VStack {
                        ForEach(self.messages, id: \.self) { msg in
                            Text(msg.str)
                                .foregroundStyle(msg.style)
                        }
                    }
                }
            }
        }
        .frame(minWidth: 300)
        .frame(height: 400)
        .onAppear {
            load()
        }
    }

    func load() {
        self.messages.removeAll()
        hydra_debugger_thread_lock(self.thread)
        for i in 0..<hydra_debugger_thread_get_message_count(self.thread) {
            let message = hydra_debugger_thread_get_message(self.thread, UInt32(i))
            let log_level = hydra_debugger_message_get_log_level(message)
            let function = String(cString: hydra_debugger_message_get_function(message))
            let str = String(cString: hydra_debugger_message_get_string(message))
            let msg = Message(log_level: log_level, str: "\(function): \(str)")
            self.messages.append(msg)
        }
        hydra_debugger_thread_unlock(self.thread)
    }
}
