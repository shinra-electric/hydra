import SwiftUI

struct SettingsView: View {
    var body: some View {
        #if os(macOS)
            TabView {
                Tab("General", systemImage: "gear") {
                    GeneralSettingsView()
                }

                Tab("CPU", systemImage: "cpu") {
                    CpuSettingsView()
                }

                Tab("Graphics", systemImage: "star") {
                    GraphicsSettingsView()
                }

                Tab("Audio", systemImage: "speaker.wave.2") {
                    AudioSettingsView()
                }

                Tab("User", systemImage: "person") {
                    UserSettingsView()
                }

                Tab("System", systemImage: "desktopcomputer") {
                    SystemSettingsView()
                }

                Tab("Debug", systemImage: "memorychip") {
                    DebugSettingsView()
                }
            }
            .scenePadding()
            .frame(maxWidth: 500, minHeight: 100)
            .onDisappear {
                hydraConfigSerialize()
            }
        #else
            NavigationView {
                List {
                    // No general settings

                    NavigationLink(destination: CpuSettingsView()) {
                        Label("CPU", systemImage: "cpu")
                    }

                    NavigationLink(destination: GraphicsSettingsView()) {
                        Label("Graphics", systemImage: "star")
                    }

                    NavigationLink(destination: AudioSettingsView()) {
                        Label("Audio", systemImage: "speaker.wave.2")
                    }

                    NavigationLink(destination: UserSettingsView()) {
                        Label("User", systemImage: "person")
                    }

                    NavigationLink(destination: SystemSettingsView()) {
                        Label("System", systemImage: "desktopcomputer")
                    }

                    NavigationLink(destination: DebugSettingsView()) {
                        Label("Debug", systemImage: "memorychip")
                    }
                }
                .navigationTitle("Settings")
            }
            .onDisappear {
                hydraConfigSerialize()
            }
        #endif
    }
}
