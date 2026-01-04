import SwiftUI

struct DeleteButton: View {
    let action: () -> Void

    var body: some View {
        Button(action: {
            self.action()
        }) {
            Image(systemName: "trash")
                .foregroundColor(.red)
        }
        .buttonStyle(BorderlessButtonStyle())
    }
}
