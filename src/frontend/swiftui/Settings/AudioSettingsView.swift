import SwiftUI

struct AudioSettingsView: View {
    @State private var audioBackend: HydraAudioBackend = HYDRA_AUDIO_BACKEND_INVALID

    var body: some View {
        List {
            Picker("Audio backend", selection: self.$audioBackend.rawValue) {
                Text("Null").tag(
                    HYDRA_AUDIO_BACKEND_NULL.rawValue)
                Text("Cubeb (recommended)").tag(HYDRA_AUDIO_BACKEND_CUBEB.rawValue)
            }
            .onChange(of: self.audioBackend.rawValue) { _, newValue in
                var audioBackendOption = hydraConfigGetAudioBackend()
                audioBackendOption.value = newValue
            }
        }
        .onAppear {
            let audioBackendOption = hydraConfigGetAudioBackend()
            self.audioBackend.rawValue = audioBackendOption.value
        }
    }
}
