import SwiftUI

struct GamePreview: View {
    let game: Game
    @Binding var activeGame: Game?

    var body: some View {
        ClickableListItem(onClick: {
            self.activeGame = self.game
        }) {
            HStack {
                #if os(macOS)
                    if let nsImage = self.loadIconNS() {
                        Image(nsImage: nsImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                            .frame(maxWidth: 64, maxHeight: 64)  // TODO: don't hardcode
                    }
                #else
                    if let uiImage = self.loadIconUI() {
                        Image(uiImage: uiImage)
                            .resizable()
                            .aspectRatio(contentMode: .fit)
                            .frame(maxWidth: 64, maxHeight: 64)  // TODO: don't hardcode
                    }
                #endif
                Text(game.name)
                    .padding()
                // TODO: author?
            }
        }
    }

    private func loadIcon(width: inout UInt64, height: inout UInt64) -> CGImage? {
        guard let data = self.game.loader.loadIcon(width: &width, height: &height) else {
            return nil
        }

        defer { free(data) }

        guard
            let context = CGContext(
                data: data,
                width: Int(width),
                height: Int(height),
                bitsPerComponent: 8,
                bytesPerRow: Int(width) * 4,
                space: CGColorSpaceCreateDeviceRGB(),
                bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue
            )
        else {
            return nil
        }

        return context.makeImage()
    }

    #if os(macOS)
        private func loadIconNS() -> NSImage? {
            var width: UInt64 = 0
            var height: UInt64 = 0
            guard let cgImage = self.loadIcon(width: &width, height: &height) else {
                return nil
            }

            return NSImage(cgImage: cgImage, size: NSSize(width: Int(width), height: Int(height)))
        }
    #else
        private func loadIconUI() -> UIImage? {
            var width: UInt64 = 0
            var height: UInt64 = 0
            guard let cgImage = self.loadIcon(width: &width, height: &height) else {
                return nil
            }

            return UIImage(cgImage: cgImage)
        }
    #endif
}
