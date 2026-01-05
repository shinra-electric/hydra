import SwiftUI

struct UserAvatarView: View {
    let userManager: HydraUserManager
    let avatarPath: String

    var body: some View {
        VStack {
            if let image = self.loadAvatar() {
                #if os(macOS)
                    Image(nsImage: image)
                        .resizable()
                        .aspectRatio(contentMode: .fit)
                #else
                    Image(uiImage: image)
                        .resizable()
                        .aspectRatio(contentMode: .fit)
                #endif
            }
        }
    }

    private func loadAvatarCG(dimensions: inout UInt32) -> CGImage? {
        guard
            let data = self.userManager.loadAvatarImage(
                path: self.avatarPath, dimensions: &dimensions)
        else {
            return nil
        }

        let dataProvider = CGDataProvider(
            dataInfo: nil,
            data: data,
            size: Int(dimensions) * Int(dimensions) * 4,
            releaseData: { _, _, _ in })!

        guard
            let cgImage = CGImage(
                width: Int(dimensions),
                height: Int(dimensions),
                bitsPerComponent: 8,
                bitsPerPixel: 32,
                bytesPerRow: Int(dimensions) * 4,
                space: CGColorSpaceCreateDeviceRGB(),
                bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue),
                provider: dataProvider,
                decode: nil,
                shouldInterpolate: false,
                intent: .defaultIntent
            )
        else {
            return nil
        }

        return cgImage
    }

    #if os(macOS)
        private func loadAvatar() -> NSImage? {
            var dimensions: UInt32 = 0
            guard let cgImage = self.loadAvatarCG(dimensions: &dimensions) else {
                return nil
            }

            return NSImage(
                cgImage: cgImage, size: NSSize(width: Int(dimensions), height: Int(dimensions)))
        }
    #else
        private func loadAvatar() -> UIImage? {
            var dimensions: UInt32 = 0
            guard let cgImage = self.loadAvatarCG(dimensions: &dimensions) else {
                return nil
            }

            return UIImage(
                cgImage: cgImage)
        }
    #endif
}
