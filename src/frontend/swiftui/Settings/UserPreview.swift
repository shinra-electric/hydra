import SwiftUI

struct UserPreview: View {
    let userManager: UnsafeMutableRawPointer
    let user: UnsafeMutableRawPointer

    var body: some View {
        HStack {
            ZStack {
                let avatarBgColor = hydra_user_get_avatar_bg_color(self.user)
                Rectangle()
                    .fill(
                        Color(
                            red: Double(avatarBgColor.x) / 255.0,
                            green: Double(avatarBgColor.y) / 255.0,
                            blue: Double(avatarBgColor.z) / 255.0))
                UserAvatarView(
                    userManager: self.userManager, avatarPath: hydra_user_get_avatar_path(self.user)
                )
            }
            .frame(maxWidth: 128, maxHeight: 128)  // TODO: don't hardcode?
            Text(toSwiftString(hydra_user_get_nickname(self.user))!)
        }
    }
}
