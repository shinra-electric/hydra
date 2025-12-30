import SwiftUI

struct RawImage<Content: View>: View {
    private let loadData: (inout UInt32, inout UInt32) -> UnsafeMutableRawPointer?
    private let freeData: (UnsafeMutableRawPointer) -> Void
    private let content: Content

    init(
        loadData: @escaping (inout UInt32, inout UInt32) -> UnsafeMutableRawPointer?,
        freeData: @escaping (UnsafeMutableRawPointer) -> Void,
        @ViewBuilder content: () -> Content
    ) {
        self.loadData = loadData
        self.freeData = freeData
        self.content = content()
    }

    var body: some View {
        if let image = self.loadIcon() {
            #if os(macOS)
                Image(nsImage: image)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
            #else
                Image(uiImage: image)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
            #endif
        } else {
            content
                .padding()
        }
    }

    private func loadIconCG(width: inout UInt32, height: inout UInt32) -> CGImage? {
        guard let data = loadData(&width, &height) else {
            return nil
        }

        defer { freeData(data) }

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
        private func loadIcon() -> NSImage? {
            var width: UInt32 = 0
            var height: UInt32 = 0
            guard let cgImage = self.loadIconCG(width: &width, height: &height) else {
                return nil
            }

            return NSImage(cgImage: cgImage, size: NSSize(width: Int(width), height: Int(height)))
        }
    #else
        private func loadIcon() -> UIImage? {
            var width: UInt32 = 0
            var height: UInt32 = 0
            guard let cgImage = self.loadIconCG(width: &width, height: &height) else {
                return nil
            }

            return UIImage(cgImage: cgImage)
        }
    #endif
}
