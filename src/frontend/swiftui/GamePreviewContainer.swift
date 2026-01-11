import SwiftUI

protocol GamePreview: View {
    init(game: Game)
}

struct GamePreviewContainer<T: GamePreview>: View {
    @EnvironmentObject var globalState: GlobalState

    let game: Game

    @State private var showAlert = false
    @State private var alertMessage = ""

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
                GameExtractButton(
                    game: self.game, content: .icon, showAlert: self.$showAlert,
                    alertMessage: self.$alertMessage)
                GameExtractButton(
                    game: self.game, content: .exefs, showAlert: self.$showAlert,
                    alertMessage: self.$alertMessage)
                GameExtractButton(
                    game: self.game, content: .romfs, showAlert: self.$showAlert,
                    alertMessage: self.$alertMessage)
            }
            .menuOrder(.fixed)
        }
        .alert(self.alertMessage, isPresented: self.$showAlert) {
            Button("OK", role: .cancel) {}
        }
    }
}
