import SwiftUI

struct UserAvatarEditorView: View {
    let userManager: UnsafeMutableRawPointer

    @Binding var avatarBgColor: hydra_uchar3
    @Binding var avatarPath: hydra_string

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
                    }
                }
            }

            // Avatar preview
            ZStack {
                Rectangle()
                    .fill(
                        Color(
                            red: Double(self.avatarBgColor.x) / 255.0,
                            green: Double(self.avatarBgColor.y) / 255.0,
                            blue: Double(self.avatarBgColor.z) / 255.0))
                UserAvatarView(
                    userManager: self.userManager, avatarPath: self.avatarPath
                )
            }
            .frame(maxWidth: 128, maxHeight: 128)  // TODO: don't hardcode?
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        let avatarCount = Int(hydra_user_manager_get_avatar_count(self.userManager))
        self.avatarPaths = [hydra_string](
            repeating: hydra_string(data: nil, size: 0), count: avatarCount)
        self.avatarPaths.withUnsafeMutableBufferPointer { bufferPointer in
            let rawPointer: UnsafeMutablePointer<hydra_string> = bufferPointer.baseAddress!
            hydra_user_manager_get_avatar_paths(self.userManager, rawPointer)
        }
    }

    func save() {
    }
}
