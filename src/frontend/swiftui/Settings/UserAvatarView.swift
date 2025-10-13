import SwiftUI

struct UserAvatarView: View {
    let userManager: UnsafeMutableRawPointer
    let userID: hydra_u128

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
        var data: UnsafeMutableRawPointer?
        hydra_user_manager_load_avatar_image(self.userManager, self.userID, &data, &dimensions)
        guard let data = data else {
            return nil
        }

        defer { free(data) }

        guard
            let context = CGContext(
                data: data,
                width: Int(dimensions),
                height: Int(dimensions),
                bitsPerComponent: 8,
                bytesPerRow: Int(dimensions) * 4,
                space: CGColorSpaceCreateDeviceRGB(),
                bitmapInfo: CGImageAlphaInfo.premultipliedLast.rawValue
            )
        else {
            return nil
        }

        return context.makeImage()
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
