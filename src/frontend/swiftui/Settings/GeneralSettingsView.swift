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
        let gamePathsOption = hydraConfigGetGamePaths()
        self.gamePaths = []
        for i in 0..<gamePathsOption.count {
            self.gamePaths.append(gamePathsOption.get(at: i))
        }

        let patchPathsOption = hydraConfigGetPatchPaths()
        self.patchPaths = []
        for i in 0..<patchPathsOption.count {
            self.patchPaths.append(patchPathsOption.get(at: i))
        }
    }

    func save() {
        // TODO: simplify this?
        let gamePathsOption = hydraConfigGetGamePaths()
        gamePathsOption.resize(newCount: self.gamePaths.count)
        for i in 0..<self.gamePaths.count {
            gamePathsOption.set(at: i, value: self.gamePaths[i])
        }

        let patchPathsOption = hydraConfigGetPatchPaths()
        patchPathsOption.resize(newCount: self.patchPaths.count)
        for i in 0..<self.patchPaths.count {
            patchPathsOption.set(at: i, value: self.patchPaths[i])
        }
    }
}
