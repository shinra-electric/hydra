import SwiftUI

struct EmulationToolbarItems: ToolbarContent {
    @Binding var activeGame: Game?
    @Binding var emulationContext: HydraEmulationContext?

    private var isRunning: Bool {
        activeGame != nil ? true : false
    }

    var body: some ToolbarContent {
        #if os(macOS)
            ToolbarItemGroup(placement: .principal) {
                Button("Play", systemImage: "play") {}.disabled(isRunning)
                Button("Stop", systemImage: "stop") {
                    guard let emulationContext = self.emulationContext else { return }
                    emulationContext.requestStop()
                    // TODO: wait a bit?
                    emulationContext.forceStop()
                    self.activeGame = nil
                }
                .disabled(!isRunning)
                Button("Pause", systemImage: "pause") {}.disabled(!isRunning)
            }
        #endif
    }

}
