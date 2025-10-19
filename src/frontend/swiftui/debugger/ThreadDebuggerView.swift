import SwiftUI

// TODO: don't store the messages in a separate array?
struct ThreadDebuggerView: View {
    let thread: HydraDebuggerThread

    @State private var status = HYDRA_DEBUGGER_THREAD_STATUS_RUNNING
    @State private var breakReason = ""
    @State private var messages: [Message] = []

    var body: some View {
        VStack {
            Text(self.thread.name.value)
                .bold()
            ZStack {
                Color.black
                    .ignoresSafeArea()

                ScrollView(.vertical) {
                    LazyVStack {
                        ForEach(self.messages, id: \.self) { msg in
                            MessageView(message: msg)
                        }
                        if self.status == HYDRA_DEBUGGER_THREAD_STATUS_BREAK {
                            Text("BREAK (reason: \(self.breakReason))")
                                .foregroundStyle(.red)
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
            self.messages.removeAll()
        }
    }

    func load() {
        self.thread.lock()

        // Status
        self.status = self.thread.status
        self.breakReason = self.thread.breakReason.value

        // Messages
        self.messages.removeAll()  // TODO: is this necessary?
        for i in 0..<self.thread.messageCount {
            let message = self.thread.getMessage(at: i)
            let log_level = message.logLevel
            let function = message.function.value
            let str = message.str.value
            let stack_trace = message.stackTrace

            let msg = Message(
                log_level: log_level, function: function, str: str, stack_trace: stack_trace)
            self.messages.append(msg)
        }

        self.thread.unlock()
    }
}
