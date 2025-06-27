import SwiftUI

struct MenuCommands: Commands {
    @Environment(\.openWindow) var openWindow

    @Binding var activeGame: Game?

    @State var firmwareLoaders: [UnsafeMutableRawPointer] = []

    var body: some Commands {
        CommandGroup(after: .newItem) {
            Button("Load from file") {
                // TODO: Implement
                print("NOT IMPLEMENTED")
            }
            .keyboardShortcut(KeyEquivalent("l"), modifiers: .command)

            Menu("Load from firmware") {
                ForEach(self.firmwareLoaders, id: \.self) { firmwareLoader in
                    let name = String(cString: hydra_nca_loader_get_name(firmwareLoader))
                    Button(name) {
                        self.activeGame = Game(loader: firmwareLoader, name: name)
                    }
                }
            }
            .disabled(self.firmwareLoaders.count == 0)
            .onAppear {
                self.loadFirmware()
            }
            .onDisappear {
                for firmwareLoader in self.firmwareLoaders {
                    hydra_loader_destroy(firmwareLoader)
                }
            }
        }

        // Remove some items
        // CommandGroup(replacing: .help) {}
        // CommandGroup(replacing: .pasteboard) {}
        CommandGroup(replacing: .undoRedo) {}
        // CommandGroup(replacing: .systemServices) {}
    }

    func loadFirmware() {
        let firmwarePathOption = hydra_config_get_firmware_path()
        let firmwarePath = String(cString: hydra_string_option_get(firmwarePathOption))
        if firmwarePath == "" {
            return
        }

        // TODO: do all of this with URLs
        let fileManager = FileManager.default

        // TODO: don't iterate recursively?
        guard let enumerator = fileManager.enumerator(atPath: firmwarePath) else {
            // TODO: error popup
            print("Invalid firmware directory \(firmwarePath)")
            return
        }

        while let filename = enumerator.nextObject() as? String {
            let path = "\(firmwarePath)/\(filename)"
            self.addFirmwareApplet(path: path)
        }
    }

    func addFirmwareApplet(path: String) {
        guard let file = hydra_open_file(path) else {
            return
        }
        //defer { hydra_file_close(file) }

        guard let contentArchive = hydra_create_content_archive(file) else {
            return
        }
        defer { hydra_content_archive_destroy(contentArchive) }

        if hydra_content_archive_get_content_type(contentArchive)
            != HYDRA_CONTENT_ARCHIVE_CONTENT_TYPE_PROGRAM
        {
            return
        }

        guard let loader = hydra_create_nca_loader_from_content_archive(contentArchive) else {
            return
        }

        self.firmwareLoaders.append(loader)
    }
}
