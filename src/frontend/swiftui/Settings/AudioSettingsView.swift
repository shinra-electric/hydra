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
        let audioBackendOption = hydra_config_get_audio_backend()
        self.audioBackend.rawValue = hydra_u32_option_get(audioBackendOption)
    }

    func save() {
        let audioBackendOption = hydra_config_get_audio_backend()
        hydra_u32_option_set(audioBackendOption, self.audioBackend.rawValue)
    }
}
