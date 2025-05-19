import SwiftUI

struct GameListView: View {
    @State private var games: [Game] = []

    var body: some View {
        List {
            ForEach(self.games.indices, id: \.self) { index in
                GamePreview(game: games[index])
                    .padding(.vertical, 8)
            }
        }
        //.navigationTitle("")
        .onAppear {
            load()
        }
    }

    func load() {
        // Game directories
        let gameDirsOption = hydra_config_get_game_directories()
        for i in 0..<hydra_string_array_option_get_count(gameDirsOption) {
            if let gameDirRaw = hydra_string_array_option_get(gameDirsOption, UInt32(i)) {
                let gameDir = String(cString: gameDirRaw)

                // TODO: do this with URLs
                // Iterate recursively over the game directories
                let fileManager = FileManager.default
                guard let enumerator = fileManager.enumerator(atPath: gameDir) else {
                    // TODO: error popup
                    print("Invalid game directory \(gameDir)")
                    return
                }

                while let filename = enumerator.nextObject() as? String {
                    // TODO: ask the core for supported extensions
                    if !filename.hasSuffix("nro") && !filename.hasSuffix("nso")
                        && !filename.hasSuffix("nca")
                    {
                        continue
                    }

                    // HACK: get the full path (only works for the root directory)
                    let gamePath = "\(gameDir)/\(filename)"

                    self.games.append(Game(path: gamePath))
                }
            }
        }
    }
}
