import SwiftUI

struct SystemSettingsView: View {
    @State var firmwarePath: String = ""
    @State var sdCardPath: String = ""
    @State var savePath: String = ""
    @State var sysmodulesPath: String = ""

    var body: some View {
        VStack {
            // TODO: use file importers
            HStack {
                Text("Firmware Path:")
                TextField("Firmware Path", text: $firmwarePath)
            }
            HStack {
                Text("SD Card Path:")
                TextField("SD Card Path", text: $sdCardPath)
            }
            HStack {
                Text("Save Path:")
                TextField("Save Path", text: $savePath)
            }
            HStack {
                Text("Sysmodules Path:")
                TextField("Sysmodules Path", text: $sysmodulesPath)
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
        let firmwarePathOption = hydra_config_get_firmware_path()
        self.firmwarePath = String(cString: hydra_string_option_get(firmwarePathOption))

        let sdCardPathOption = hydra_config_get_sd_card_path()
        self.sdCardPath = String(cString: hydra_string_option_get(sdCardPathOption))

        let savePathOption = hydra_config_get_save_path()
        self.savePath = String(cString: hydra_string_option_get(savePathOption))

        let sysmodulesPathOption = hydra_config_get_sysmodules_path()
        self.sysmodulesPath = String(cString: hydra_string_option_get(sysmodulesPathOption))
    }

    func save() {
        let firmwarePathOption = hydra_config_get_firmware_path()
        hydra_string_option_set(firmwarePathOption, self.firmwarePath.cString(using: .utf8))

        let sdCardPathOption = hydra_config_get_sd_card_path()
        hydra_string_option_set(sdCardPathOption, self.sdCardPath.cString(using: .utf8))

        let savePathOption = hydra_config_get_save_path()
        hydra_string_option_set(savePathOption, self.savePath.cString(using: .utf8))

        let sysmodulesPathOption = hydra_config_get_sysmodules_path()
        hydra_string_option_set(sysmodulesPathOption, self.sysmodulesPath.cString(using: .utf8))
    }
}
