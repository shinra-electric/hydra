import SwiftUI

struct MessageView: View {
    let message: Message

    @State private var isExpanded = false

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Button(action: {
                withAnimation(.easeInOut(duration: 0.3)) {
                    isExpanded.toggle()
                }
            }) {
                HStack {
                    Image(systemName: isExpanded ? "chevron.down" : "chevron.right")
                        .foregroundColor(.primary)
                        .font(.system(size: 14, weight: .medium))

                    // TODO: simplify this?
                    HStack {
                        Text(message.function)
                            .foregroundStyle(.white)
                            .font(.system(size: 12))
                            .frame(maxWidth: 120, alignment: .trailing)
                        Text(message.str)
                            .foregroundStyle(message.style)
                            .font(.system(size: 12))
                            .frame(maxWidth: .infinity, alignment: .leading)
                    }
                }
            }
            .buttonStyle(PlainButtonStyle())

            if isExpanded {
                LazyVStack(spacing: 2) {
                    ForEach(
                        0..<Int(hydra_debugger_stack_trace_get_frame_count(message.stack_trace)),
                        id: \.self
                    ) {
                        i in
                        HStack {
                            Text(getStackFrame(index: UInt32(i)))
                                .font(.system(size: 12))
                            Spacer()
                        }
                    }
                }
            }
        }
        .onDisappear {
            hydra_debugger_stack_trace_destroy(self.message.stack_trace)
        }
    }

    func getStackFrame(index: UInt32) -> String {
        let stack_frame = hydra_debugger_stack_trace_get_frame(message.stack_trace, index)
        let resolved = hydra_debugger_stack_frame_resolve_unmanaged(stack_frame)
        let module = String(cString: hydra_debugger_resolved_stack_frame_get_module(resolved))
        let function = String(cString: hydra_debugger_resolved_stack_frame_get_function(resolved))
        let addr = hydra_debugger_resolved_stack_frame_get_address(resolved)
        hydra_debugger_resolved_stack_frame_destroy(resolved)

        // TODO: hex address
        return "0x\(String(addr, radix: 16)) (\(function) in \(module))"
    }
}
