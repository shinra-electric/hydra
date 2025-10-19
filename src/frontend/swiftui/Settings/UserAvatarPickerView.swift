import SwiftUI

struct UserAvatarPickerView: View {
    let userManager: HydraUserManager

    @Binding var avatarPath: HydraString

    @State private var avatarPaths: [HydraString] = []

    var body: some View {
        // Avatars
        ScrollView(.vertical) {
            LazyVGrid(columns: [GridItem(.adaptive(minimum: 64))], spacing: 10) {
                ForEach(self.avatarPaths) { avatarPath in
                    UserAvatarView(
                        userManager: self.userManager, avatarPath: avatarPath
                    )
                    .overlay(
                        RoundedRectangle(cornerRadius: 8)
                            .stroke(
                                avatarPath == self.avatarPath
                                    ? Color.blue : Color.clear, lineWidth: 3)
                    )
                    .onTapGesture {
                        self.avatarPath = avatarPath
                    }
                }
            }
        }
        .onAppear {
            load()
        }
    }

    func load() {
        self.avatarPaths = []
        for i in 0..<self.userManager.avatarCount {
            self.avatarPaths.append(self.userManager.getAvatarPath(at: i))
        }
    }
}
