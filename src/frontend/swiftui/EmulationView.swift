import SwiftUI

struct EmulationView: View {
    let game: Game

    @Binding var emulationContext: HydraEmulationContext?

    var body: some View {
        MetalView(emulationContext: self.$emulationContext)
            .onAppear {
                startEmulation()
            }
            .onDisappear {
                stopEmulation()
            }
    }

    func startEmulation() {
        self.emulationContext = HydraEmulationContext()
        self.emulationContext!.loadAndStart(loader: game.loader)
    }

    func stopEmulation() {
        // TODO: stop?
        self.emulationContext = nil
    }
}
