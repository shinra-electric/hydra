import SwiftUI

struct LoaderPluginConfig: Equatable, Hashable {
    var path: String
    var options: [String: String]

    init(handle: HydraLoaderPluginConfig) {
        self.path = handle.path
        self.options = [:]

        let options = handle.options
        for i in 0..<options.count {
            let key = options.getKey(at: i)
            let value = options.getValue(at: i)
            self.options[key] = value
        }
    }

    func serialize(to handle: inout HydraLoaderPluginConfig) {
        handle.path = self.path
        handle.options.removeAll()
        for (key, value) in self.options {
            handle.options.set(byKey: key, value: value)
        }
    }
}

struct LoaderPluginView: View {
    var config: LoaderPluginConfig

    @State private var name = ""
    @State private var displayVersion = ""
    @State private var supportedFormats: [String] = []

    var body: some View {
        VStack(alignment: .leading) {
            Text(self.name)
                .bold()
            Text(self.displayVersion)
            Text("Supported formats: \(self.supportedFormats.joined(separator: ", "))")
        }
        .multilineTextAlignment(.leading)
        .onAppear {
            let plugin = HydraLoaderPlugin(path: config.path, options: config.options)
            self.name = plugin.name
            self.displayVersion = plugin.displayVersion
            self.supportedFormats = []
            for i in 0..<plugin.getSupportedFormatCount() {
                self.supportedFormats.append(plugin.getSupportedFormat(at: i))
            }
        }
    }
}
