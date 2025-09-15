import SwiftUI

struct DebuggerProcessesView: View {
    @State private var debuggers: [UnsafeMutableRawPointer] = []

    var body: some View {
        ScrollView(.vertical) {
            ForEach(self.debuggers.indices, id: \.self) { index in
                let debugger = self.debuggers[index]
                ClickableListItem(onClick: {
                    abort()
                }) {
                    Text(String(cString: hydra_debugger_get_name(debugger)))
                }
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
            // TODO: name
            self.debuggers.append(
                hydra_debugger_manager_get_debugger(UInt32(i)))
        }

        hydra_debugger_manager_unlock()
    }
}
