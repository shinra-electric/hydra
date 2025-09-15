import SwiftUI

struct DebuggerListView: View {
    @Binding var activeDebugger: UnsafeMutableRawPointer?

    @State private var debuggers: [UnsafeMutableRawPointer] = []

    var body: some View {
        List {
            ForEach(self.debuggers.indices, id: \.self) { index in
                let debugger = self.debuggers[index]
                ClickableListItem(onClick: {
                    self.activeDebugger = debugger
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
