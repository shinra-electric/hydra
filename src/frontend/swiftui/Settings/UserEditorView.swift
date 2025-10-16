import SwiftUI

struct UserEditorView: View {
    let userManager: UnsafeMutableRawPointer
    let user: UnsafeMutableRawPointer

    @State private var nickname = ""
    @State private var avatarBgColor = hydra_uchar3(x: 0, y: 0, z: 0)
    @State private var avatarPath = hydra_string(data: nil, size: 0)

    @State private var showAvatarEditor = false

    var body: some View {
        HStack {
            ZStack {
                Rectangle()
                    .fill(
                        Color(
                            red: Double(self.avatarBgColor.x) / 255.0,
                            green: Double(self.avatarBgColor.y) / 255.0,
                            blue: Double(self.avatarBgColor.z) / 255.0))
                UserAvatarView(
                    userManager: self.userManager, avatarPath: hydra_user_get_avatar_path(self.user)
                )

                // Edit button
                Button(action: {
                    self.showAvatarEditor = true
                }) {
                    Image(systemName: "pencil")
                        .foregroundColor(.white)
                        .padding()
                }
            }
            .frame(maxWidth: 128, maxHeight: 128)  // TODO: don't hardcode?
            Text(self.nickname)
        }
        .sheet(isPresented: self.$showAvatarEditor) {
            // Title bar
            HStack {
                Text("Select an Image")
                    .font(.headline)
                Spacer()
                Button("Cancel") {
                    self.showAvatarEditor = false
                }
                .keyboardShortcut(.cancelAction)
            }
            .padding()
            .background(Color(nsColor: .controlBackgroundColor))

            Divider()

            // Editor view
            UserAvatarEditorView(
                userManager: self.userManager, avatarBgColor: self.$avatarBgColor,
                avatarPath: self.$avatarPath
            )
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
        self.avatarPath = hydra_user_get_avatar_path(self.user)
    }

    func save() {
        hydra_user_set_nickname(self.user, toHydraString(self.nickname)!)
        hydra_user_set_avatar_bg_color(self.user, self.avatarBgColor)
        hydra_user_set_avatar_path(self.user, self.avatarPath)
    }
}
