import SwiftUI

struct EmulationView: View {
    @Binding var emulationState: EmulationState
    @Binding var fps: Int

    var body: some View {
        ZStack {
            MetalView(emulationState: $emulationState, fps: $fps)
                .onAppear {
                    emulationState.emulationContext = HydraEmulationContext()
                    emulationState.emulationContext!.loadAndStart(
                        loader: emulationState.activeGame!.loader)
                }
                .onDisappear {
                    fps = 0
                }

            #if os(macOS)
                KeyCatcherView()
            #endif

            if emulationState.isStopping {
                GameStopView(emulationState: $emulationState)
            }
        }
    }
}
