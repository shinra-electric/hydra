import AppKit
import UniformTypeIdentifiers

struct SavePanel {
    static func present(
        allowedContentTypes: [UTType] = [],
        defaultFilename: String? = nil,
        completion: @escaping (URL?) -> Void
    ) {
        let panel = NSSavePanel()
        panel.canCreateDirectories = true
        panel.allowedContentTypes = allowedContentTypes
        panel.nameFieldStringValue = defaultFilename ?? ""

        panel.begin { response in
            completion(response == .OK ? panel.url : nil)
        }
    }
}
