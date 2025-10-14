import SwiftUI

struct UserAvatarView: View {
    let userManager: UnsafeMutableRawPointer
    let user: UnsafeMutableRawPointer

    var body: some View {
        VStack {
            if let nsImage = self.loadAvatarNS() {
                Image(nsImage: nsImage)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(maxWidth: 128, maxHeight: 128)  // TODO: don't hardcode
            }
        }
    }

    private func loadAvatar(dimensions: inout UInt64) -> CGImage? {
        var data: UnsafeRawPointer?
        hydra_user_manager_load_avatar_image(
            self.userManager, hydra_user_get_avatar_path(self.user), &data, &dimensions)
        guard let data = data else {
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
