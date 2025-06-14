import SwiftUI

struct ThreadDebuggerView: View {
    let thread: UnsafeMutableRawPointer

    var body: some View {
        Text(String(cString: hydra_debugger_thread_get_name(self.thread)))
    }
}
