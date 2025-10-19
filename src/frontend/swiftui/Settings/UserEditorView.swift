import SwiftUI

struct UserEditorView: View {
    let userManager: HydraUserManager
    var user: HydraUser

    @State private var nickname = ""
    @State private var avatarBgColor = hydra_uchar3(x: 0, y: 0, z: 0)
    @State private var avatarPath = HydraString.empty

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
                    userManager: self.userManager, avatarPath: self.user.avatarPath
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
        self.nickname = self.user.nickname.value
        self.avatarBgColor = self.user.avatarBgColor
        self.avatarPath = self.user.avatarPath
    }

    func save() {
        var user = self.user
        user.nickname.value = self.nickname
        user.avatarBgColor = self.avatarBgColor
        user.avatarPath = self.avatarPath
    }
}
