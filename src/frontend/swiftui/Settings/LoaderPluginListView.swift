import SwiftUI

struct LoaderPluginListView: View {
    @State private var configs: [LoaderPluginConfig] = []

    var body: some View {
        Section("Loader Plug-Ins") {
            ForEach(self.configs, id: \.self) { config in
                LoaderPluginView(config: config)
            }
            .onChange(of: self.configs) { _, newValue in
                let loaderPluginsOption = hydraConfigGetLoaderPlugins()
                loaderPluginsOption.resize(newCount: newValue.count)
                for i in 0..<newValue.count {
                    var newHandle = loaderPluginsOption.get(at: i)
                    newValue[i].serialize(to: &newHandle)
                }
            }
        }
        .onAppear {
            let loaderPluginsOption = hydraConfigGetLoaderPlugins()
            self.configs = []
            for i in 0..<loaderPluginsOption.count {
                self.configs.append(
                    LoaderPluginConfig(handle: loaderPluginsOption.get(at: i)))
            }
        }
    }
}
