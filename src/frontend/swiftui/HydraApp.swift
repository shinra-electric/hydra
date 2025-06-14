import SwiftUI

@main
struct MyApp: App {
    // TODO: if macOS
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate

    init() {
        hydra_config_initialize()
        // TODO: only enable if enabled in the config
        hydra_debugger_enable()
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
