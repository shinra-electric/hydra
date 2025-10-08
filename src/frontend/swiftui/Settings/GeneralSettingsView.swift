import SwiftUI
import UniformTypeIdentifiers

struct GeneralSettingsView: View {
    @State private var gamePaths: [String] = []
    @State private var patchPaths: [String] = []

    private let switchType = UTType(exportedAs: "com.samoz256.switch-document", conformingTo: .data)
    private let hatchType = UTType(exportedAs: "com.samoz256.hatch-document", conformingTo: .data)

    var body: some View {
        VStack {
            Text("Game paths")
            EditablePathList(
                allowedContentTypes: [.folder, self.switchType],
                items: self.$gamePaths)
            Text("Patch paths")
            EditablePathList(
                allowedContentTypes: [.folder, self.hatchType], items: self.$patchPaths)
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        let gamePathsOption = hydra_config_get_game_paths()
        self.gamePaths = []
        for i in 0..<hydra_string_array_option_get_count(gamePathsOption) {
            if let gamePathRaw = hydra_string_array_option_get(gamePathsOption, UInt32(i)) {
                let gamePath = String(cString: gamePathRaw)
                self.gamePaths.append(gamePath)
            }
        }

        let patchPathsOption = hydra_config_get_patch_paths()
        self.patchPaths = []
        for i in 0..<hydra_string_array_option_get_count(patchPathsOption) {
            if let patchPathRaw = hydra_string_array_option_get(patchPathsOption, UInt32(i)) {
                let patchPath = String(cString: patchPathRaw)
                self.patchPaths.append(patchPath)
            }
        }
    }

    func save() {
        // TODO: simplify this?
        let gamePathsOption = hydra_config_get_game_paths()
        hydra_string_array_option_resize(gamePathsOption, UInt64(self.gamePaths.count))
        for i in 0..<self.gamePaths.count {
            hydra_string_array_option_set(
                gamePathsOption, UInt32(i), self.gamePaths[i].cString(using: .utf8))
        }

        let patchPathsOption = hydra_config_get_patch_paths()
        hydra_string_array_option_resize(patchPathsOption, UInt64(self.patchPaths.count))
        for i in 0..<self.patchPaths.count {
            hydra_string_array_option_set(
                patchPathsOption, UInt32(i), self.patchPaths[i].cString(using: .utf8))
        }
    }
}
