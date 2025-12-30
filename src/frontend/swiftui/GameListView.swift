import SwiftUI

enum ViewMode: Int {
    case list
    case grid
}

struct GameListView: View {
    @Binding var emulationState: EmulationState
    @Binding var viewMode: Int

    @State private var games: [Game] = []

    private let gridColumns = [
        GridItem(.adaptive(minimum: 180), spacing: 16)
    ]

    var body: some View {
        VStack {
            switch ViewMode(rawValue: viewMode) {
            case .list:
                List {
                    ForEach(self.games.indices, id: \.self) { index in
                        ClickableListItem(onClick: {
                            emulationState.activeGame = games[index]
                        }) {
                            ListGamePreview(
                                emulationState: $emulationState, game: games[index])
                        }
                        .padding(.vertical, 16)
                    }
                }
            case .grid:
                ScrollView {
                    LazyVGrid(columns: gridColumns, spacing: 4) {
                        ForEach(games.indices, id: \.self) { index in
                            ClickableListItem(onClick: {
                                emulationState.activeGame = games[index]
                            }) {
                                GridGamePreview(
                                    emulationState: $emulationState,
                                    game: games[index]
                                )
                            }
                            .frame(maxWidth: .infinity)
                            .frame(height: 180)
                            .padding()
                        }
                    }
                    .padding()
                    .frame(maxWidth: .infinity)
                }
            case .none:
                Text("ERROR")
            }
        }
        .onAppear {
            // Game paths

            // Get all games
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

            // Sort games by name
            games.sort { $0.name.caseInsensitiveCompare($1.name) == .orderedAscending }
        }
        .onDisappear {
            games.removeAll()
        }
    }

    func processUrl(url: URL) throws {
        // Check if the URL is a game
        if url.pathExtension == "nro" || url.pathExtension == "nso" || url.pathExtension == "nca"
            || url.pathExtension == "nx"
        {
            tryAddGame(url: url)
            return
        }

        // Check if the URL is a directory
        var isDirectory: ObjCBool = false
        guard
            FileManager.default.fileExists(
                atPath: url.path(percentEncoded: false), isDirectory: &isDirectory)
        else {
            // TODO: error popup
            print("Game path \"\(url)\" does not exist")
            return
        }

        if !isDirectory.boolValue {
            return
        }

        let urls = try FileManager.default.contentsOfDirectory(
            at: url, includingPropertiesForKeys: nil)
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
