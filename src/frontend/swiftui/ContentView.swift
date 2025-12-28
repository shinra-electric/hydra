import SwiftUI

#if os(iOS)
    import GameController
#endif

struct ContentView: View {
    @Binding var emulationState: EmulationState

    @AppStorage("viewMode") private var viewMode: Int = ViewMode.list.rawValue
    @State private var fps: Int = 0

    #if os(iOS)
        @State private var virtualController: GCVirtualController? = nil
    #endif

    private var appVersion: String {
        Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "?"
    }

    private var gitVersion: String {
        Bundle.main.infoDictionary?["CFBundleVersion"] as? String ?? "?"
    }

    private var navigationTitle: String {
        let gameTitle = emulationState.activeGame?.name ?? ""
        var titleInsert = ""
        if emulationState.activeGame?.name != nil { titleInsert = "  |  \(gameTitle)" }
        var gitInsert = ""
        if gitVersion != "" { gitInsert = " (\(gitVersion))" }
        var fpsInsert = ""
        if emulationState.emulationContext != nil { fpsInsert = "  |  \(fps) FPS" }
        return "Hydra v\(appVersion)\(gitInsert)\(titleInsert)\(fpsInsert)"
    }

    var body: some View {
        NavigationStack {
            GameListView(emulationState: $emulationState, viewMode: $viewMode)
                .navigationDestination(item: $emulationState.activeGame) { activeGame in
                    EmulationView(emulationState: $emulationState, fps: $fps)
                        .aspectRatio(CGSize(width: 16, height: 9), contentMode: .fit)
                        #if os(iOS)
                            .onAppear {
                                // TODO: if virtual controller enabled
                                if true {
                                    let config = GCVirtualController.Configuration()
                                    config.elements = [
                                        GCInputButtonA, GCInputButtonB, GCInputButtonX,
                                        GCInputButtonY,
                                        GCInputLeftThumbstick, GCInputRightThumbstick,
                                        GCInputLeftShoulder,
                                        GCInputRightShoulder, GCInputLeftTrigger,
                                        GCInputRightTrigger,
                                    ]

                                    let virtualController = GCVirtualController(
                                        configuration: config)
                                    virtualController.connect()

                                    self.virtualController = virtualController
                                }
                            }
                            .onDisappear {
                                virtualController?.disconnect()
                                virtualController = nil
                            }
                        #endif
                        .toolbar {
                            EmulationToolbarItems(emulationState: $emulationState)
                        }
                }
                .toolbar {
                    GameListToolbarItems(viewMode: $viewMode)
                }
                .navigationBarBackButtonHidden()
        }
        #if os(macOS)
            .windowToolbarFullScreenVisibility(.onHover)
        #endif
        .navigationTitle(navigationTitle)
    }
}
