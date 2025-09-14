import SwiftUI

struct DebuggerProcessesView: View {
    @State private var debuggers: [UnsafeMutableRawPointer] = []

    var body: some View {
        ScrollView(.vertical) {
            ForEach(self.debuggers.indices, id: \.self) { index in
                // TODO
            }
        }
        .onAppear {
            load()
        }
    }

    func load() {
        hydra_debugger_manager_lock()

        // Debuggers
        self.debuggers.removeAll()
        for i in 0..<hydra_debugger_manager_get_debugger_count() {
            self.debuggers.append(hydra_debugger_manager_get_debugger(UInt32(i)))
        }

        hydra_debugger_manager_unlock()
    }
}
