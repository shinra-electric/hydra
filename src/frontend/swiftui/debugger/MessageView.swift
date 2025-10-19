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
                        0..<message.stack_trace.frameCount,
                        id: \.self
                    ) {
                        i in
                        HStack {
                            Text(getStackFrame(index: i))
                                .font(.system(size: 12))
                                .textSelection(.enabled)
                                .focusable()
                            Spacer()
                        }
                    }
                }
            }
        }
    }

    func getStackFrame(index: Int) -> String {
        let stack_frame = message.stack_trace.getFrame(at: index)
        let resolved = stack_frame.resolve()
        let module = resolved.module.value
        let function = resolved.function.value
        let addr = resolved.address

        return "0x\(String(addr, radix: 16)) (\(function) in \(module))"
    }
}
