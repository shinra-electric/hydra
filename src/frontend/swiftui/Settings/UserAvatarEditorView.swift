import SwiftUI

struct UserAvatarEditorView: View {
    @Environment(\.self) var environment

    let userManager: UnsafeMutableRawPointer

    @Binding var avatarBgColor: hydra_uchar3
    @Binding var avatarPath: hydra_string

    @State private var avatarBgColorColor: Color = .clear
    @State private var avatarPaths: [hydra_string] = []

    var body: some View {
        HStack {
            // Avatars
            ScrollView(.vertical) {
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 64))], spacing: 10) {
                    ForEach(Array(self.avatarPaths.enumerated()), id: \.offset) {
                        index, avatarPath in
                        UserAvatarView(
                            userManager: self.userManager, avatarPath: avatarPath
                        )
                        .overlay(
                            RoundedRectangle(cornerRadius: 8)
                                .stroke(
                                    avatarPath.data == self.avatarPath.data
                                        ? Color.blue : Color.clear, lineWidth: 3)
                        )
                        .onTapGesture {
                            self.avatarPath = avatarPath
                        }
                    }
                }
            }

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

        // Avatar paths
        let avatarCount = Int(hydra_user_manager_get_avatar_count(self.userManager))
        self.avatarPaths = [hydra_string](
            repeating: hydra_string(data: nil, size: 0), count: avatarCount)
        self.avatarPaths.withUnsafeMutableBufferPointer { bufferPointer in
            let rawPointer: UnsafeMutablePointer<hydra_string> = bufferPointer.baseAddress!
            hydra_user_manager_get_avatar_paths(self.userManager, rawPointer)
        }
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
