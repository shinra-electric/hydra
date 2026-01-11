import SwiftUI

protocol GamePreview: View {
    init(game: Game)
}

struct GamePreviewContainer<T: GamePreview>: View {
    @EnvironmentObject var globalState: GlobalState

    let game: Game

    var body: some View {
        ClickableListItem(onClick: {
            globalState.activeGame = self.game
        }) {
            T(game: self.game)
        }
        .contextMenu {
            Button {
                globalState.activeGame = self.game
            } label: {
                Label("Play", systemImage: "play")
            }

            Divider()

            Menu("Extract...") {
                GameExtractButton(game: self.game, content: .icon)
                GameExtractButton(game: self.game, content: .exefs)
                GameExtractButton(game: self.game, content: .romfs)
            }
            .menuOrder(.fixed)
        }
    }
}
