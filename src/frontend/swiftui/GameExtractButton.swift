import SwiftUI
import UniformTypeIdentifiers

struct GameExtractButton: View {
    let game: Game
    let content: HydraLoaderContent

    @Binding var showAlert: Bool
    @Binding var alertName: String

    private var name: String {
        switch self.content {
        case .icon:
            return "Icon"
        case .exefs:
            return "ExeFS"
        case .romfs:
            return "RomFS"
        }
    }

    private var contentType: UTType {
        switch self.content {
        case .icon:
            return .jpeg
        case .exefs, .romfs:
            return .folder
        }
    }

    var body: some View {
        Button(self.name) {
            SavePanel.present(allowedContentTypes: [self.contentType], defaultFilename: nil) {
                url in
                guard let url = url else {
                    return
                }

                DispatchQueue.global(qos: .userInitiated).async {
                    hydraDebuggerManagerGetDebuggerForCurrentProcess().registerThisThread(
                        name: "Content extract")
                    self.game.loader.extractContent(
                        self.content, to: url.path(percentEncoded: false))
                    self.alertName = self.name
                    self.showAlert = true
                    hydraDebuggerManagerGetDebuggerForCurrentProcess().unregisterThisThread()
                }
            }
        }
        .disabled(!self.game.loader.hasContent(self.content))
    }
}
