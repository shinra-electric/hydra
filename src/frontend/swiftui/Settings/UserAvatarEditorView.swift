import SwiftUI

struct UserAvatarEditorView: View {
    let userManager: HydraUserManager

    @Binding var avatarBgColor: hydra_uchar3
    @Binding var avatarPath: HydraString

    @State private var avatarBgColorColor: Color = .clear

    var body: some View {
        HStack {
            // Avatars
            UserAvatarPickerView(userManager: self.userManager, avatarPath: self.$avatarPath)

            // Avatar preview
            VStack {
                ZStack {
                    Rectangle()
                        .fill(
                            self.avatarBgColorColor)
                    UserAvatarView(
                        userManager: self.userManager, avatarPath: self.avatarPath
                    )
                }
                .frame(maxWidth: 128, maxHeight: 128)  // TODO: don't hardcode?

                // Color picker
                ColorPicker("Choose background color", selection: self.$avatarBgColorColor)
                    .padding()
            }
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        // Avatar background color
        self.avatarBgColorColor = Color(
            red: Double(self.avatarBgColor.x) / 255.0,
            green: Double(self.avatarBgColor.y) / 255.0,
            blue: Double(self.avatarBgColor.z) / 255.0)
    }

    func save() {
        // Avatar background color
        if let cgColor = self.avatarBgColorColor.cgColor?.converted(
            to: CGColorSpace(name: CGColorSpace.sRGB)!,
            intent: .defaultIntent,
            options: nil
        ),
            let components = cgColor.components
        {
            self.avatarBgColor = hydra_uchar3(
                x: UInt8(components[0] * 255.0),
                y: UInt8(components[1] * 255.0),
                z: UInt8(components[2] * 255.0))
        }
    }
}
