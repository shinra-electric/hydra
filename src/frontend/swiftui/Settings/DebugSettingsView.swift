import SwiftUI

struct DebugSettingsView: View {
    @State var logOutput: HydraLogOutput = HYDRA_LOG_OUTPUT_INVALID
    @State var logFsAccess = false
    @State var debugLogging = false
    // TODO: process args

    var body: some View {
        VStack {
            Picker("Log output", selection: self.$logOutput.rawValue) {
                Text("none (not recommended)").tag(
                    HYDRA_LOG_OUTPUT_NONE.rawValue)
                Text("stdout").tag(
                    HYDRA_LOG_OUTPUT_STD_OUT.rawValue)
                Text("file (default)").tag(HYDRA_LOG_OUTPUT_FILE.rawValue)
            }
            Toggle("Log filesystem access", isOn: self.$logFsAccess)
            Toggle("Debug logging", isOn: self.$debugLogging)
            // TODO: process arguments
        }
        .onAppear {
            load()
        }
        .onDisappear {
            save()
        }
    }

    func load() {
        let logOutputOption = hydraConfigGetLogOutput()
        self.logOutput.rawValue = logOutputOption.value

        let logFsAccessOption = hydraConfigGetLogFsAccess()
        self.logFsAccess = logFsAccessOption.value

        let debugLoggingOption = hydraConfigGetDebugLogging()
        self.debugLogging = debugLoggingOption.value
    }

    func save() {
        var logOutputOption = hydraConfigGetLogOutput()
        logOutputOption.value = self.logOutput.rawValue

        var logFsAccessOption = hydraConfigGetLogFsAccess()
        logFsAccessOption.value = self.logFsAccess

        var debugLoggingOption = hydraConfigGetDebugLogging()
        debugLoggingOption.value = self.debugLogging
    }
}
