import SwiftUI

struct GridGamePreview: View {
    @Binding var emulationState: EmulationState
    let game: Game

    var body: some View {
        ClickableListItem(onClick: {
            emulationState.activeGame = self.game
        }) {
            VStack {
                GameIconView(game: game)
                    .frame(maxWidth: 120, maxHeight: 120)
                Text(game.name)
                    .multilineTextAlignment(.center)
                    .padding()
            }
        }
    }
}
