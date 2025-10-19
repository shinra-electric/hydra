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
        let firmwarePathOption = hydraConfigGetFirmwarePath()
        self.firmwarePath = firmwarePathOption.value.value

        let sdCardPathOption = hydraConfigGetSdCardPath()
        self.sdCardPath = sdCardPathOption.value.value

        let savePathOption = hydraConfigGetSavePath()
        self.savePath = savePathOption.value.value

        let sysmodulesPathOption = hydraConfigGetSysmodulesPath()
        self.sysmodulesPath = sysmodulesPathOption.value.value
    }

    func save() {
        let firmwarePathOption = hydraConfigGetFirmwarePath()
        firmwarePathOption.value.value = self.firmwarePath

        let sdCardPathOption = hydraConfigGetSdCardPath()
        sdCardPathOption.value.value = self.sdCardPath

        let savePathOption = hydraConfigGetSavePath()
        savePathOption.value.value = self.savePath

        let sysmodulesPathOption = hydraConfigGetSysmodulesPath()
        sysmodulesPathOption.value.value = self.sysmodulesPath
    }
}
