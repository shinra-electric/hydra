import SwiftUI

struct ListGamePreview: View {
    @Binding var emulationState: EmulationState
    let game: Game

    var body: some View {
        ClickableListItem(onClick: {
            emulationState.activeGame = self.game
        }) {
            HStack {
                GameIconView(game: game)
                    .frame(maxWidth: 64, maxHeight: 64)
                VStack {
                    Text(game.name)
                        .padding()
                    Text(game.author)
                        .padding()
                }
            }
        }
    }
}
