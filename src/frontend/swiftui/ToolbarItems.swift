import SwiftUI
import UniformTypeIdentifiers

struct ToolbarItems: ToolbarContent {
    @State private var isFilePickerPresented: Bool = false

    private let switchType = UTType(exportedAs: "com.samoz256.switch-document", conformingTo: .data)

    var body: some ToolbarContent {
        ToolbarItemGroup(placement: .automatic) {
            Button("Add Game Path", systemImage: "plus") {
                isFilePickerPresented.toggle()
            }
            // TODO: change allowedContentTypes to nsp etc
            .fileImporter(
                isPresented: $isFilePickerPresented,
                allowedContentTypes: [.folder, self.switchType],
                allowsMultipleSelection: false
            ) { result in
                switch result {
                case .success(let fileURLs):
                    for fileURL in fileURLs {
                        let gamePathsOption = hydraConfigGetGamePaths()
                        gamePathsOption.append(
                            value: HydraString(fileURL.path))

                        hydraConfigSerialize()
                    }

                case .failure(let error):
                    print(error)
                }
            }
        }
    }

}
