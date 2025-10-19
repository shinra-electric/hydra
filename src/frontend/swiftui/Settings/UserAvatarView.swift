import SwiftUI

struct UserAvatarView: View {
    let userManager: HydraUserManager
    let avatarPath: HydraString

    var body: some View {
        VStack {
            if let nsImage = self.loadAvatarNS() {
                Image(nsImage: nsImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
            }
        }
    }

    private func loadAvatar(dimensions: inout UInt64) -> CGImage? {
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

    private func loadAvatarNS() -> NSImage? {
        var dimensions: UInt64 = 0
        guard let cgImage = self.loadAvatar(dimensions: &dimensions) else {
            return nil
        }

        return NSImage(
            cgImage: cgImage, size: NSSize(width: Int(dimensions), height: Int(dimensions)))
    }
}
