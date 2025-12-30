import SwiftUI
import UniformTypeIdentifiers

struct UserAvatarPickerView: View {
    let userManager: HydraUserManager

    @Binding var avatarPath: String

    @State private var avatarPaths: [String] = []

    @State private var isFilePickerPresented: Bool = false

    var body: some View {
        // Avatars
        ScrollView(.vertical) {
            LazyVGrid(columns: [GridItem(.adaptive(minimum: 64))], spacing: 10) {
                ForEach(self.avatarPaths, id: \.self) { avatarPath in
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
                Button("Import") {
                    self.isFilePickerPresented = true
                }
                .fileImporter(
                    isPresented: self.$isFilePickerPresented,
                    allowedContentTypes: [.png, .jpeg],
                    allowsMultipleSelection: false
                ) { result in
                    switch result {
                    case .success(let fileURLs):
                        let path = fileURLs.first?.path(percentEncoded: false) ?? ""
                        self.avatarPath = path
                        if !self.avatarPaths.contains(path) {
                            self.avatarPaths.append(path)
                        }
                    case .failure(let error):
                        print(error)
                    }
                }
            }
        }
        .onAppear {
            for i in 0..<self.userManager.avatarCount {
                self.avatarPaths.append(self.userManager.getAvatarPath(at: i))
            }
        }
        .onDisappear {
            self.avatarPaths.removeAll()
        }
    }
}
