import SwiftUI

struct ListGamePreview: View {
    let game: Game

    var body: some View {
        HStack {
            GameIconView(game: game)
                .frame(maxWidth: 80, maxHeight: 80)
            VStack(alignment: .leading) {
                Text(game.name)
                    .bold()
                    .frame(alignment: .leading)
                    .padding(4)
                Text(game.author)
                    .frame(alignment: .leading)
                    .padding(4)
                Text(game.version)
                    .frame(alignment: .leading)
                    .padding(4)
            }
        }
        .multilineTextAlignment(.leading)
    }
}
