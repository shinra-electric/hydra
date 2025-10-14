import SwiftUI

struct UserSettingsView: View {
    // TODO: don't use @State?
    @State private var filesystem: UnsafeMutableRawPointer? = nil
    @State private var userManager: UnsafeMutableRawPointer? = nil

    @State private var userIDs: [hydra_u128] = []
    @State private var selectedUserIndex: Int? = nil

    var body: some View {
        VStack {
            if let userManager = self.userManager {
                NavigationSplitView {
                    List(self.userIDs.indices, id: \.self, selection: self.$selectedUserIndex) {
                        index in
                        NavigationLink(value: index) {
                            let userID = self.userIDs[index]
                            UserPreview(
                                userManager: userManager,
                                user: hydra_user_manager_get_user(
                                    userManager,
                                    userID)
                            )
                        }
                    }
                    .navigationTitle("Users")
                } detail: {
                    if let index = self.selectedUserIndex {
                        let userID = self.userIDs[index]
                        UserEditorView(
                            userManager: userManager,
                            user: hydra_user_manager_get_user(
                                userManager,
                                userID)
                        )
                    } else {
                        Text("Select a user to edit")
                            .foregroundColor(.secondary)
                    }
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
        // Filesystem
        self.filesystem = hydra_create_filesystem()
        hydra_try_install_firmware_to_filesystem(self.filesystem)

        // User manager
        self.userManager = hydra_create_user_manager()
        hydra_user_manager_load_system_avatars(self.userManager, self.filesystem)

        // Users
        for index in 0..<Int(hydra_user_manager_get_user_count(userManager)) {
            let userID = hydra_user_manager_get_user_id(userManager, UInt32(index))
            self.userIDs.append(userID)
        }
        self.selectedUserIndex = 0
    }

    func save() {
        self.userIDs.removeAll()
        hydra_user_manager_destroy(self.userManager)
        self.userManager = nil
        hydra_filesystem_destroy(self.filesystem)
        self.filesystem = nil
    }
}
