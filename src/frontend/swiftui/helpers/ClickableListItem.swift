import SwiftUI

struct ClickableListItem<Content: View>: View {
    private let content: Content
    private let onClick: () -> Void

    @State private var lastClickTime: Date? = nil
    @State private var navigate = false

    init(onClick: @escaping () -> Void, @ViewBuilder content: () -> Content) {
        self.onClick = onClick
        self.content = content()
    }

    var body: some View {
        HoverView {
            content
        }
        .onTapGesture {
            if let lastTime = lastClickTime, Date().timeIntervalSince(lastTime) < 0.3 {
                self.onClick()
                lastClickTime = nil
            } else {
                lastClickTime = Date()
            }
        }
    }
}
