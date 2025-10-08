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
        let logOutputOption = hydra_config_get_log_output()
        self.logOutput.rawValue = hydra_u32_option_get(logOutputOption)

        let logFsAccessOption = hydra_config_get_log_fs_access()
        self.logFsAccess = hydra_bool_option_get(logFsAccessOption)

        let debugLoggingOption = hydra_config_get_debug_logging()
        self.debugLogging = hydra_bool_option_get(debugLoggingOption)
    }

    func save() {
        let logOutputOption = hydra_config_get_log_output()
        hydra_u32_option_set(logOutputOption, self.logOutput.rawValue)

        let logFsAccessOption = hydra_config_get_log_fs_access()
        hydra_bool_option_set(logFsAccessOption, self.logFsAccess)

        let debugLoggingOption = hydra_config_get_debug_logging()
        hydra_bool_option_set(debugLoggingOption, self.debugLogging)
    }
}
