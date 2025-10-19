import SwiftUI

@main
struct MyApp: App {
    // TODO: if macOS
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate

    @State private var activeGame: Game? = nil
    @State private var emulationContext: HydraEmulationContext? = nil

    var body: some Scene {
        Window("Hydra", id: "main") {
            ContentView(activeGame: self.$activeGame, emulationContext: self.$emulationContext)
                .frame(minWidth: 640, minHeight: 360)
        }
        .windowResizability(.contentSize)
        .commands {
            MenuCommands(activeGame: self.$activeGame, emulationContext: self.$emulationContext)
        }

        Window("Debugger", id: "debugger") {
            DebuggersView()
        }
        .defaultLaunchBehavior(.suppressed)

        Settings {
            SettingsView()
        }
    }
}
