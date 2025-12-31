import SwiftUI

struct GameStopView: View {
    @EnvironmentObject var globalState: GlobalState

    @State var loadingPresented: Bool = false
    @State var dialogPresented: Bool = false
    @State private var timer: Timer? = nil

    var body: some View {
        VStack {
            if loadingPresented {
                ProgressView()
                    .progressViewStyle(CircularProgressViewStyle())
                    .alert("Game not responding", isPresented: $dialogPresented) {
                        Text("The game is not responding. You can wait or force quit.")

                        Button("Cancel", role: .cancel) {
                            timer!.invalidate()

                            globalState.isStopping = false

                            loadingPresented = false
                        }

                        Button("Force Quit", role: .destructive) {
                            timer!.invalidate()

                            globalState.emulationContext!.forceStop()
                            globalState.emulationContext = nil
                            globalState.activeGame = nil
                            globalState.isStopping = false

                            loadingPresented = false
                        }
                    }
            }
        }
        .onAppear {
            let startTime = Date()

            guard let emulationContext = globalState.emulationContext else { return }
            emulationContext.requestStop()

            timer = Timer.scheduledTimer(withTimeInterval: 0.2, repeats: true) { _ in
                if !emulationContext.isRunning() {
                    timer!.invalidate()

                    globalState.emulationContext = nil
                    globalState.activeGame = nil
                    globalState.isStopping = false
                } else if Date().timeIntervalSince(startTime) > 3 {
                    loadingPresented = true
                    if Date().timeIntervalSince(startTime) > 5 {
                        dialogPresented = true
                    }
                }
            }
        }
        .onDisappear {
            timer?.invalidate()
            timer = nil
        }
    }
}
