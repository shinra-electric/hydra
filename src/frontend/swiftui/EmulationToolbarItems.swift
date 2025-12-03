import SwiftUI

struct EmulationToolbarItems: ToolbarContent {
    @Binding var emulationState: EmulationState

    @State private var isRunning: Bool = false

    var body: some ToolbarContent {
        #if os(macOS)
            ToolbarItemGroup(placement: .principal) {
                if isRunning {
                    Button("Pause", systemImage: "pause") {
                        guard let emulationContext = emulationState.emulationContext else { return }
                        emulationContext.pause()
                        isRunning = false
                    }
                } else {
                    Button("Resume", systemImage: "play") {
                        guard let emulationContext = emulationState.emulationContext else { return }
                        emulationContext.resume()
                        isRunning = true
                    }
                }
                Button("Stop", systemImage: "stop") {
                    emulationState.isStopping = true
                }
                .onAppear {
                    isRunning = emulationState.emulationContext!.isRunning()
                }
            }
        #endif
    }

}
