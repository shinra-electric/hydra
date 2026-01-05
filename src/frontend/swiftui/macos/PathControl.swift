import AppKit
import SwiftUI

struct PathControl: NSViewRepresentable {
    @Binding var url: URL?

    var allowedTypes: [String]?
    var placeholderString: String?

    func makeNSView(context: Context) -> NSPathControl {
        let pathControl = NSPathControl()
        pathControl.pathStyle = .standard
        pathControl.url = self.url
        pathControl.isEditable = true
        pathControl.allowedTypes = self.allowedTypes
        pathControl.placeholderString = self.placeholderString
        pathControl.setContentCompressionResistancePriority(.defaultLow, for: .horizontal)
        return pathControl
    }

    func updateNSView(_ nsView: NSPathControl, context: Context) {
        nsView.url = url
    }
}
