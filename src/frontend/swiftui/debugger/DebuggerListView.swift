import SwiftUI

struct DebuggerListView: View {
    @Binding var activeDebugger: HydraDebugger?

    @State private var debuggers: [HydraDebugger] = []

    var body: some View {
        List {
            ForEach(self.debuggers.indices, id: \.self) { index in
                let debugger = self.debuggers[index]
                ClickableListItem(onClick: {
                    self.activeDebugger = debugger
                }) {
                    Text(debugger.name.value)
                }
            }
        }
        .onAppear {
            load()
        }
    }

    func load() {
        hydraDebuggerManagerLock()

        // Debuggers
        self.debuggers.removeAll()
        for i in 0..<hydraDebuggerManagerGetDebuggerCount() {
            // TODO: name
            self.debuggers.append(
                hydraDebuggerManagerGetDebugger(at: i))
        }

        hydraDebuggerManagerUnlock()
    }
}
