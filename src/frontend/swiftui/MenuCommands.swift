import SwiftUI

struct MenuCommands: Commands {
    @Environment(\.openWindow) var openWindow

    var body: some Commands {
        CommandGroup(after: .newItem) {
            Button("Add to Library") {
                // TODO: Implement
            }
            .keyboardShortcut(KeyEquivalent("o"), modifiers: .command)

            Button("Boot from File") {
                // TODO: Implement
            }
            .keyboardShortcut(KeyEquivalent("b"), modifiers: .command)
        }

        // Remove some items
        CommandGroup(replacing: .help) {}
        CommandGroup(replacing: .pasteboard) {}
        CommandGroup(replacing: .undoRedo) {}
        // CommandGroup(replacing: .systemServices) {}
    }
}
