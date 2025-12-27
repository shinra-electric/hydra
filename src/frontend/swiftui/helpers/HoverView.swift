import SwiftUI

struct HoverView<Content: View>: View {
    private let content: Content

    @State private var isHovered = false

    init(
        @ViewBuilder content: () -> Content
    ) {
        self.content = content()
    }

    var body: some View {
        content
            .onHover {
                isHovered = $0
            }
            .scaleEffect(isHovered ? 1.05 : 1.0)
            .background(
                RoundedRectangle(cornerRadius: 8)
                    .fill(
                        isHovered
                            ? Color.white.opacity(0.12)
                            : Color.clear)
            )
            .shadow(radius: isHovered ? 10 : 2)
            .animation(.easeOut(duration: 0.15), value: isHovered)
    }
}
