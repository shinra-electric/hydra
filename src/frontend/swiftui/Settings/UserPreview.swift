import SwiftUI

struct UserPreview: View {
    let userManager: UnsafeMutableRawPointer
    let userID: hydra_u128

    var body: some View {
        HStack {
            UserAvatarView(userManager: self.userManager, userID: self.userID)
            let user = hydra_user_manager_get_user(self.userManager, self.userID)
            Text(toSwiftString(hydra_user_get_nickname(user))!)
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
