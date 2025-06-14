import SwiftUI

@main
struct MyApp: App {
    init() {
        hydra_config_initialize()
        hydra_debugger_try_install_callback()
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
