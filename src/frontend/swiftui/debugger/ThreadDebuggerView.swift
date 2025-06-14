import SwiftUI

// TODO: don't store the messages in a separate array?
struct ThreadDebuggerView: View {
    let thread: UnsafeMutableRawPointer

    @State var messages: [Message] = []

    var body: some View {
        VStack {
            Text(String(cString: hydra_debugger_thread_get_name(self.thread)))
                .bold()
            ZStack {
                Color.black
                    .ignoresSafeArea()

                ScrollView(.vertical) {
                    LazyVStack {
                        ForEach(self.messages, id: \.self) { msg in
                            MessageView(message: msg)
                        }
                    }
                }
            }
        }
        .frame(height: 500)
        .onAppear {
            load()
        }
        .onDisappear {
            for msg in self.messages {
                hydra_debugger_stack_trace_destroy(msg.stack_trace)
            }
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
            let stack_trace = hydra_debugger_stack_trace_copy(
                hydra_debugger_message_get_stack_trace(message))!

            let msg = Message(
                log_level: log_level, function: function, str: str, stack_trace: stack_trace)
            self.messages.append(msg)
        }
        hydra_debugger_thread_unlock(self.thread)
    }
}
