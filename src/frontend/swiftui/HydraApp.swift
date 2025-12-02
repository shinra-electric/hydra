import SwiftUI

@Observable
class EmulationState {
    var activeGame: Game? = nil
    var emulationContext: HydraEmulationContext? = nil
}

@main
struct HydraApp: App {
    #if os(macOS)
        @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    #endif

    @State private var emulationState = EmulationState()

    var body: some Scene {
        #if os(macOS)
            Window("Hydra", id: "main") {
                ContentView(emulationState: $emulationState)
            }
            .defaultSize(width: 1280, height: 720)
            .windowResizability(.contentSize)
            .commands {
                MenuCommands(emulationState: $emulationState)
            }

            Window("Debugger", id: "debugger") {
                DebuggersView()
            }
            .defaultLaunchBehavior(.suppressed)

            Settings {
                SettingsView()
            }
        #else
            WindowGroup {
                ContentView(emulationState: $emulationState)
            }
        #endif
    }
}
