import SwiftUI

struct UserPreview: View {
    let userManager: UnsafeMutableRawPointer
    let user: UnsafeMutableRawPointer

    var body: some View {
        HStack {
            UserAvatarView(userManager: self.userManager, user: self.user)
            Text(toSwiftString(hydra_user_get_nickname(self.user))!)
        }
    }
}
