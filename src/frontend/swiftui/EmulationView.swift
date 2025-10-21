import SwiftUI

struct EmulationView: View {
    let game: Game

    @Binding var emulationContext: HydraEmulationContext?

    var body: some View {
        MetalView(emulationContext: self.$emulationContext)
            .onAppear {
                self.emulationContext = HydraEmulationContext()
                self.emulationContext!.loadAndStart(loader: game.loader)
            }
            .onDisappear {
                // TODO: stop?
                self.emulationContext = nil
            }
    }
}
