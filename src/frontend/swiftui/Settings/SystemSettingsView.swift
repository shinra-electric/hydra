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
        self.firmwarePath = firmwarePathOption.value

        let sdCardPathOption = hydraConfigGetSdCardPath()
        self.sdCardPath = sdCardPathOption.value

        let savePathOption = hydraConfigGetSavePath()
        self.savePath = savePathOption.value

        let sysmodulesPathOption = hydraConfigGetSysmodulesPath()
        self.sysmodulesPath = sysmodulesPathOption.value
    }

    func save() {
        var firmwarePathOption = hydraConfigGetFirmwarePath()
        firmwarePathOption.value = self.firmwarePath

        var sdCardPathOption = hydraConfigGetSdCardPath()
        sdCardPathOption.value = self.sdCardPath

        var savePathOption = hydraConfigGetSavePath()
        savePathOption.value = self.savePath

        var sysmodulesPathOption = hydraConfigGetSysmodulesPath()
        sysmodulesPathOption.value = self.sysmodulesPath
    }
}
