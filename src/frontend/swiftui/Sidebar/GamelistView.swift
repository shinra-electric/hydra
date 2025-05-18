import SwiftUI

struct GameListView: View {
    @State private var game_directories: [String] = []

    var body: some View {
        List {
            ForEach(game_directories.indices, id: \.self) { index in
                Text(game_directories[index])
                    .padding(.vertical, 8)
            }
        }
        //.navigationTitle("C String Array")
        .onAppear {
            loadGamePreviews()
        }
    }

    func loadGamePreviews() {
        // Game directories
        let game_directories_option = hydra_config_get_game_directories()
        for i in 0..<hydra_string_array_option_get_count(game_directories_option) {
            if let game_dir_raw = hydra_string_array_option_get(game_directories_option, UInt32(i))
            {
                let game_dir = String(cString: game_dir_raw)
                game_directories.append(game_dir)
            }
        }
    }
}
