import SwiftUI

struct DebuggerProcessesView: View {
    @State private var debuggers: [UnsafeMutableRawPointer] = []

    @State private var activeDebugger: UnsafeMutableRawPointer? = nil

    var body: some View {
        NavigationStack {
            DebuggerListView(activeDebugger: self.$activeDebugger)
                .navigationDestination(item: self.$activeDebugger) { activeDebugger in
                    DebuggerView(debugger: activeDebugger)
                }
        }
    }
}
