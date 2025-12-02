import SwiftUI

struct EmulationToolbarItems: ToolbarContent {
    @Binding var emulationState: EmulationState

    private var isRunning: Bool {
        emulationState.activeGame != nil ? true : false
    }

    var body: some ToolbarContent {
        #if os(macOS)
            ToolbarItemGroup(placement: .principal) {
                Button("Play", systemImage: "play") {}.disabled(isRunning)
                Button("Stop", systemImage: "stop") {
                    guard let emulationContext = emulationState.emulationContext else { return }
                    emulationContext.requestStop()
                    // TODO: wait a bit?
                    emulationContext.forceStop()
                    emulationState.activeGame = nil
                }
                .disabled(!isRunning)
                Button("Pause", systemImage: "pause") {}.disabled(!isRunning)
            }
        #endif
    }

}
