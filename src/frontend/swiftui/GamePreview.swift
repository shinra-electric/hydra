import SwiftUI

struct GamePreview: View {
    let game: Game
    @Binding var activeGame: Game?

    @State private var lastClickTime: Date? = nil
    @State private var navigate = false

    var body: some View {
        // TODO: display logo as well
        Text(game.name)
            .padding()
            .contentShape(Rectangle())
            .onTapGesture {
                if let lastTime = lastClickTime, Date().timeIntervalSince(lastTime) < 0.3 {
                    activeGame = self.game
                    lastClickTime = nil
                } else {
                    lastClickTime = Date()
                }
            }
    }
}
