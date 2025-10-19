import SwiftUI

class UserSettingsState: ObservableObject {
    var filesystem = HydraFilesystem()
    var userManager = HydraUserManager()

    init() {
        self.filesystem.tryInstallFirmware()
        self.userManager.loadSystemAvatars(filesystem: self.filesystem)
    }
}

struct UserSettingsView: View {
    @StateObject private var state = UserSettingsState()

    @State private var userIDs: [hydra_u128] = []
    @State private var selectedUserIndex: Int? = nil

    var body: some View {
        VStack {
            NavigationSplitView {
                List(self.userIDs.indices, id: \.self, selection: self.$selectedUserIndex) {
                    index in
                    NavigationLink(value: index) {
                        let userID = self.userIDs[index]
                        UserPreview(
                            userManager: self.state.userManager,
                            user: self.state.userManager.getUser(id: userID)
                        )
                    }
                }
                .navigationTitle("Users")
            } detail: {
                if let index = self.selectedUserIndex {
                    let userID = self.userIDs[index]
                    UserEditorView(
                        userManager: self.state.userManager,
                        user: self.state.userManager.getUser(id: userID)
                    )
                } else {
                    Text("Select a user to edit")
                        .foregroundColor(.secondary)
                }
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
        // Users
        for index in 0..<self.state.userManager.userCount {
            let userID = self.state.userManager.getUserId(at: index)
            self.userIDs.append(userID)
        }
        self.selectedUserIndex = 0
    }

    func save() {
        self.userIDs.removeAll()
        self.selectedUserIndex = nil
    }
}
