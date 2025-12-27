import SwiftUI

struct GameListView: View {
    @Binding var emulationState: EmulationState

    @State private var games: [Game] = []

    var body: some View {
        List {
            ForEach(self.games.indices, id: \.self) { index in
                GamePreview(emulationState: $emulationState, game: self.games[index])
                    .padding(.vertical, 8)
            }
        }
        //.navigationTitle("")
        .onAppear {
            load()
        }
        .onDisappear {
            games.removeAll()
        }
    }

    func load() {
        // Game paths
        let gamePathsOption = hydraConfigGetGamePaths()
        for i in 0..<gamePathsOption.count {
            let gamePath = gamePathsOption.get(at: i)
            do {
                let url = try resolveUrl(URL(fileURLWithPath: gamePath))

                try processUrl(url: url)
            } catch {
                // TODO: error popup
                print("Failed to load game path \(gamePath): \(error)")
            }
        }
    }

    func processUrl(url: URL) throws {
        // Check if the URL is a game
        if url.pathExtension == "nro" || url.pathExtension == "nso" || url.pathExtension == "nca" || url.pathExtension == "nx" {
            tryAddGame(url: url)
            return
        }

        // Check if the URL is a directory
        var isDirectory: ObjCBool = false
        guard FileManager.default.fileExists(atPath: url.path, isDirectory: &isDirectory) else {
            // TODO: error popup
            print("Game path \"\(url)\" does not exist")
            return
        }

        if !isDirectory.boolValue {
            return
        }

        let urls = try FileManager.default.contentsOfDirectory(at: url, includingPropertiesForKeys: nil)
        for url in urls {
            try processUrl(url: url)
        }
    }

    func tryAddGame(url: URL) {
        // HACK
        if url.lastPathComponent == "Makefile.nx" {
            print("Ignoring Makefile.nx")
            return
        }

        guard let game = createGameFromPath(url: url) else {
            return
        }

        self.games.append(game)
    }
}
