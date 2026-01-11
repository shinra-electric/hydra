import SwiftUI
import UniformTypeIdentifiers

struct GameExtractButton: View {
    let game: Game
    let content: HydraLoaderContent

    private var name: String {
        switch self.content {
        case .exefs:
            return "ExeFS"
        case .romfs:
            return "RomFS"
        case .icon:
            return "Icon"
        }
    }

    private var contentType: UTType {
        switch self.content {
        case .exefs, .romfs:
            return .folder
        case .icon:
            return .jpeg
        }
    }

    @State private var isAlertPresented = false

    var body: some View {
        Button(self.name) {
            SavePanel.present(allowedContentTypes: [self.contentType], defaultFilename: nil) {
                url in
                guard let url = url else {
                    return
                }
                self.game.loader.extractContent(self.content, to: url.path(percentEncoded: false))
                self.isAlertPresented = true
            }
        }
        .disabled(!self.game.loader.hasContent(self.content))
        .alert("\(self.name) successfully extracted", isPresented: self.$isAlertPresented) {
            Button("OK", role: .cancel) {}
        }
    }
}
