import SwiftUI

struct SettingsView: View {
    private enum Tabs: Hashable {
        case general, graphics, debug
    }

    var body: some View {
        TabView {
            GeneralSettingsView()
                .tabItem {
                    Label("General", systemImage: "gear")
                }
                .tag(Tabs.general)
            CpuSettingsView()
                .tabItem {
                    Label("CPU", systemImage: "cpu")
                }
                .tag(Tabs.graphics)
            GraphicsSettingsView()
                .tabItem {
                    Label("Graphics", systemImage: "star")
                }
                .tag(Tabs.graphics)
            AudioSettingsView()
                .tabItem {
                    Label("Audio", systemImage: "speaker.wave.2")
                }
                .tag(Tabs.graphics)
            UserSettingsView()
                .tabItem {
                    Label("User", systemImage: "person")
                }
                .tag(Tabs.graphics)
            SystemSettingsView()
                .tabItem {
                    Label("System", systemImage: "desktopcomputer")
                }
                .tag(Tabs.graphics)
            DebugSettingsView()
                .tabItem {
                    Label("Debug", systemImage: "memorychip")
                }
                .tag(Tabs.debug)
        }
        .padding(20)
        // TODO: don't hardcode the size
        .frame(width: 800, height: 600)
        .onDisappear {
            // TODO: why is this called before the individual tabs?
            hydra_config_serialize()
        }
    }
}
