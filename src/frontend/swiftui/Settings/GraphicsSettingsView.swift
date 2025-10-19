import SwiftUI

struct GraphicsSettingsView: View {
    @State var gpuRenderer: HydraGpuRenderer = HYDRA_GPU_RENDERER_INVALID
    @State var shaderBackend: HydraShaderBackend = HYDRA_SHADER_BACKEND_INVALID
    @State var displayResolution: HydraResolution = HYDRA_RESOLUTION_INVALID
    @State var customDisplayResolution: hydra_uint2 = hydra_uint2(x: 0, y: 0)

    var body: some View {
        VStack {
            Picker("Gpu renderer", selection: self.$gpuRenderer.rawValue) {
                Text("Metal (recommended)").tag(HYDRA_GPU_RENDERER_METAL.rawValue)
            }
            Picker("Shader backend", selection: self.$shaderBackend.rawValue) {
                Text("MSL (recommended)").tag(HYDRA_SHADER_BACKEND_MSL.rawValue)
                Text("AIR (broken)").tag(HYDRA_SHADER_BACKEND_AIR.rawValue)
            }
            Picker("Display resolution", selection: self.$displayResolution.rawValue) {
                Text("Auto (recommended)").tag(HYDRA_RESOLUTION_AUTO.rawValue)
                Text("720p").tag(HYDRA_RESOLUTION_720P.rawValue)
                Text("1080p").tag(HYDRA_RESOLUTION_1080P.rawValue)
                Text("1440p").tag(HYDRA_RESOLUTION_1440P.rawValue)
                Text("2160p").tag(HYDRA_RESOLUTION_2160P.rawValue)
                Text("4320p").tag(HYDRA_RESOLUTION_4320P.rawValue)
                Text("Auto exact (not recommended)").tag(HYDRA_RESOLUTION_AUTO_EXACT.rawValue)
                Text("Custom (not recommended)").tag(HYDRA_RESOLUTION_CUSTOM.rawValue)
            }
            if self.displayResolution == HYDRA_RESOLUTION_CUSTOM {
                HStack {
                    Text("Custom resolution")
                    TextField(
                        "Width", value: self.$customDisplayResolution.x,
                        format: .number)
                    Text("x")
                    TextField(
                        "Height", value: self.$customDisplayResolution.y,
                        format: .number)
                }
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
        let gpuRendererOption = hydraConfigGetGpuRenderer()
        self.gpuRenderer.rawValue = gpuRendererOption.value

        let shaderBackendOption = hydraConfigGetShaderBackend()
        self.shaderBackend.rawValue = shaderBackendOption.value

        let displayResolutionOption = hydraConfigGetDisplayResolution()
        self.displayResolution.rawValue = displayResolutionOption.value

        let customDisplayResolutionOption = hydraConfigGetCustomDisplayResolution()
        self.customDisplayResolution = customDisplayResolutionOption.value
    }

    func save() {
        var gpuRendererOption = hydraConfigGetGpuRenderer()
        gpuRendererOption.value = self.gpuRenderer.rawValue

        var shaderBackendOption = hydraConfigGetShaderBackend()
        shaderBackendOption.value = self.shaderBackend.rawValue

        var displayResolutionOption = hydraConfigGetDisplayResolution()
        displayResolutionOption.value = self.displayResolution.rawValue

        var customDisplayResolutionOption = hydraConfigGetCustomDisplayResolution()
        customDisplayResolutionOption.value = self.customDisplayResolution
    }
}
