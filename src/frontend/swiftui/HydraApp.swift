import SwiftUI

@main
struct MyApp: App {
    init() {
        hydra_config_initialize()
    }

    var body: some Scene {
        Window("Hydra", id: "main") {
            ContentView()
                .frame(minWidth: 640, minHeight: 360)
        }
        .windowResizability(.contentSize)
        .commands {
            MenuCommands()
        }

        Window("Debugger", id: "debugger") {
            DebuggerView()
        }

        Settings {
            SettingsView()
        }
    }
}
