import SwiftUI

struct UserPreview: View {
    let userManager: UnsafeMutableRawPointer
    let userID: u128

    var body: some View {
        HStack {
            UserAvatarView(userManager: self.userManager, userID: self.userID)
            // TODO: nickname
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
    }

    func save() {
    }
}
