import AppKit
import SwiftUI

struct KeyCatcherView: NSViewRepresentable {
    class KeyView: NSView {
        override var acceptsFirstResponder: Bool { true }

        override func keyDown(with event: NSEvent) {}

        override func keyUp(with event: NSEvent) {}
    }

    func makeNSView(context: Context) -> NSView {
        let view = KeyView()
        DispatchQueue.main.async {
            view.window?.makeFirstResponder(view)
        }

        return view
    }

    func updateNSView(_ nsView: NSView, context: Context) {}
}
