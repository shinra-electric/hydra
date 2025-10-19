import SwiftUI

struct DebuggersView: View {
    @State private var activeDebugger: HydraDebugger? = nil

    var body: some View {
        NavigationStack {
            DebuggerListView(activeDebugger: self.$activeDebugger)
                .navigationDestination(item: self.$activeDebugger) { activeDebugger in
                    DebuggerView(debugger: activeDebugger)
                }
        }
    }
}
