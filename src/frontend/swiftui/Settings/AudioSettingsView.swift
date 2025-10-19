import SwiftUI

struct AudioSettingsView: View {
    @State var audioBackend: HydraAudioBackend = HYDRA_AUDIO_BACKEND_INVALID

    var body: some View {
        VStack {
            Picker("Audio backend", selection: self.$audioBackend.rawValue) {
                Text("Null (recommended)").tag(
                    HYDRA_AUDIO_BACKEND_NULL.rawValue)
                Text("Cubeb (experimental)").tag(HYDRA_AUDIO_BACKEND_CUBEB.rawValue)
            }
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        let audioBackendOption = hydraConfigGetAudioBackend()
        self.audioBackend.rawValue = audioBackendOption.value
    }

    func save() {
        var audioBackendOption = hydraConfigGetAudioBackend()
        audioBackendOption.value = self.audioBackend.rawValue
    }
}
