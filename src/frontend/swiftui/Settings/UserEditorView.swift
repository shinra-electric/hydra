import SwiftUI

struct UserEditorView: View {
    let userManager: UnsafeMutableRawPointer
    let user: UnsafeMutableRawPointer

    @State private var nickname = ""
    @State private var avatarBgColor = hydra_uchar3(x: 0, y: 0, z: 0)
    @State private var avatarPath = ""

    var body: some View {
        HStack {
            UserAvatarView(userManager: self.userManager, user: self.user)
            Text(self.nickname)
            Rectangle()
                .fill(
                    Color(
                        red: Double(self.avatarBgColor.x) / 255.0,
                        green: Double(self.avatarBgColor.y) / 255.0,
                        blue: Double(self.avatarBgColor.z) / 255.0))
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        self.nickname = toSwiftString(hydra_user_get_nickname(self.user))!
        self.avatarBgColor = hydra_user_get_avatar_bg_color(self.user)
        self.avatarPath = toSwiftString(hydra_user_get_avatar_path(self.user))!
    }

    func save() {
        hydra_user_set_nickname(self.user, toHydraString(self.nickname)!)
        hydra_user_set_avatar_bg_color(self.user, self.avatarBgColor)
        hydra_user_set_avatar_path(self.user, toHydraString(self.avatarPath)!)
    }
}
