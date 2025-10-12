import SwiftUI

struct UserSettingsView: View {
    // TODO: don't use @State?
    @State private var filesystem: UnsafeMutableRawPointer? = nil
    @State private var userManager: UnsafeMutableRawPointer? = nil

    var body: some View {
        VStack {
            if let userManager = self.userManager {
                ForEach(0..<Int(hydra_user_manager_get_user_count(userManager)), id: \.self) {
                    index in
                    UserPreview(
                        userManager: userManager,
                        userID: hydra_user_manager_get_user_id(userManager, UInt32(index))
                    )
                    .padding(.vertical, 8)
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
    }

    func save() {
        hydra_user_manager_destroy(self.userManager)
        self.userManager = nil
        hydra_filesystem_destroy(self.filesystem)
        self.filesystem = nil
    }
}
