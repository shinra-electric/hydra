import SwiftUI

struct EmulationToolbarItems: ToolbarContent {
    @Binding var emulationState: EmulationState

    @State private var isRunning: Bool = false

    var body: some ToolbarContent {
        #if os(macOS)
            ToolbarItemGroup(placement: .principal) {
                Button("Resume", systemImage: "play") {
                    guard let emulationContext = emulationState.emulationContext else { return }
                    emulationContext.resume()
                    isRunning = true
                }.disabled(isRunning)
                Button("Pause", systemImage: "pause") {
                    guard let emulationContext = emulationState.emulationContext else { return }
                    emulationContext.pause()
                    isRunning = false
                }.disabled(!isRunning)
                Button("Stop", systemImage: "stop") {
                    guard let emulationContext = emulationState.emulationContext else { return }
                    emulationContext.requestStop()
                    // TODO: wait a bit?
                    emulationContext.forceStop()
                    emulationState.activeGame = nil
                }
                .onAppear {
                    isRunning = emulationState.emulationContext!.isRunning()
                }
            }
        #endif
    }

}
