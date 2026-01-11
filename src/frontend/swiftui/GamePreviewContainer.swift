import SwiftUI

protocol GamePreview: View {
    init(game: Game)
}

struct GamePreviewContainer<T: GamePreview>: View {
    @EnvironmentObject var globalState: GlobalState

    let game: Game

    @State private var showAlert = false
    @State private var alertName = ""

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
                    alertName: self.$alertName)
                GameExtractButton(
                    game: self.game, content: .exefs, showAlert: self.$showAlert,
                    alertName: self.$alertName)
                GameExtractButton(
                    game: self.game, content: .romfs, showAlert: self.$showAlert,
                    alertName: self.$alertName)
            }
            .menuOrder(.fixed)
        }
        .alert("\(self.alertName) extracted successfully", isPresented: self.$showAlert) {
            Button("OK", role: .cancel) {}
        }
    }
}
