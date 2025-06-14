import SwiftUI

struct DebuggerView: View {
    @State private var threads: [UnsafeMutableRawPointer] = []

    @State private var refreshCount = 0

    var body: some View {
        ZStack {
            ScrollView(.vertical) {
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 500))], spacing: 10) {
                    ForEach(self.threads, id: \.self) { thread in
                        ThreadDebuggerView(thread: thread)
                    }
                }
            }

            // TODO: add an option to refresh at regular intervals or any time a change happens
            HStack {
                VStack {
                    Spacer()

                    Button(action: {
                        load()
                        refreshCount += 1
                    }) {
                        Image(systemName: "arrow.clockwise")
                            .font(.title2)
                            .foregroundColor(.blue)
                            .padding()
                    }
                    .padding()
                }

                Spacer()
            }
        }
        .onAppear {
            load()
        }
    }

    func load() {
        self.threads.removeAll()
        hydra_debugger_lock()
        for i in 0..<hydra_debugger_get_thread_count() {
            self.threads.append(hydra_debugger_get_thread(UInt32(i)))
        }
        hydra_debugger_unlock()
    }
}
