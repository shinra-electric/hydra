import SwiftUI

struct GameIconView: View {
    let game: Game

    var body: some View {
        RawImage(
            loadData: { width, height in
                return self.game.loader.loadIcon(width: &width, height: &height)
            },
            freeData: { data in
                free(data)
            }
        ) {
            VStack {
                ZStack {
                    Image(systemName: "photo.fill")
                        .resizable()
                        .aspectRatio(contentMode: .fit)
                    Image(systemName: "line.diagonal")
                        .resizable()
                        .aspectRatio(contentMode: .fill)
                }
            }
        }
        .cornerRadius(8)
    }
}
