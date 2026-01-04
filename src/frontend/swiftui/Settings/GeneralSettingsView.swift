import SwiftUI
import UniformTypeIdentifiers

struct LoaderPlugin: Equatable, Hashable {
    var path: String
    var options: [String: String]

    init(handle: HydraLoaderPlugin) {
        self.path = handle.path
        self.options = [:]

        let options = handle.options
        for i in 0..<options.count {
            let key = options.getKey(at: i)
            let value = options.getValue(at: i)
            self.options[key] = value
        }
    }

    func serialize(to handle: inout HydraLoaderPlugin) {
        handle.path = self.path
        handle.options.removeAll()
        for (key, value) in self.options {
            handle.options.set(byKey: key, value: value)
        }
    }
}

struct GeneralSettingsView: View {
    @EnvironmentObject var globalState: GlobalState

    @State private var loaderPlugins: [LoaderPlugin] = []
    @State private var patchPaths: [String] = []

    private let switchType = UTType(exportedAs: "com.samoz256.switch-document", conformingTo: .data)
    private let hatchType = UTType(exportedAs: "com.samoz256.hatch-document", conformingTo: .data)

    var body: some View {
        Spacer()
        HStack {
            Spacer()
            Form {
                Section("Game Paths") {
                    EditablePathList(
                        allowedContentTypes: [.folder, self.switchType],
                        items: $globalState.gamePaths
                    )
                    .onChange(of: globalState.gamePaths) { _, newValue in
                        let gamePathsOption = hydraConfigGetGamePaths()
                        gamePathsOption.resize(newCount: newValue.count)
                        for i in 0..<newValue.count {
                            gamePathsOption.set(at: i, value: newValue[i])
                        }
                    }
                }

                Section("Loader Plug-Ins") {
                    ForEach(self.loaderPlugins, id: \.self) { loaderPlugin in
                        Text(loaderPlugin.path)
                    }
                    .onChange(of: self.loaderPlugins) { _, newValue in
                        let loaderPluginsOption = hydraConfigGetLoaderPlugins()
                        loaderPluginsOption.resize(newCount: newValue.count)
                        for i in 0..<newValue.count {
                            var newHandle = loaderPluginsOption.get(at: i)
                            newValue[i].serialize(to: &newHandle)
                        }
                    }
                }

                Section("Patch paths") {
                    EditablePathList(
                        allowedContentTypes: [.folder, self.hatchType], items: self.$patchPaths
                    )
                    .onChange(of: self.patchPaths) { _, newValue in
                        let patchPathsOption = hydraConfigGetPatchPaths()
                        patchPathsOption.resize(newCount: newValue.count)
                        for i in 0..<newValue.count {
                            patchPathsOption.set(at: i, value: newValue[i])
                        }
                    }
                }
            }
            .onAppear {
                let loaderPluginsOption = hydraConfigGetLoaderPlugins()
                self.loaderPlugins = []
                for i in 0..<loaderPluginsOption.count {
                    self.loaderPlugins.append(LoaderPlugin(handle: loaderPluginsOption.get(at: i)))
                }

                let patchPathsOption = hydraConfigGetPatchPaths()
                self.patchPaths = []
                for i in 0..<patchPathsOption.count {
                    self.patchPaths.append(patchPathsOption.get(at: i))
                }
            }
            .formStyle(.grouped)
            Spacer()
        }
        Spacer()
    }
}
