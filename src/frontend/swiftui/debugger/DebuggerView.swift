import SwiftUI

struct DebuggerView: View {
    @State var threads: [UnsafeMutableRawPointer] = []

    var body: some View {
        ForEach(self.threads, id: \.self) { thread in
            ThreadDebuggerView(thread: thread)
        }
        .onAppear {
            load()
        }
    }

    func load() {
        self.threads.removeAll()
        hydra_debugger_lock()
        for i in 0..<hydra_debugger_get_thread_count() {
            self.threads.append(hydra_debugger_get_thread(UInt32(i)))
        }
        hydra_debugger_unlock()
    }
}
