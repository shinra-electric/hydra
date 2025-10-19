import SwiftUI

struct DebuggerView: View {
    var debugger: HydraDebugger

    @State private var threads: [HydraDebuggerThread] = []

    @State private var refreshID = 0

    var body: some View {
        ZStack {
            ScrollView(.vertical) {
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 500))], spacing: 10) {
                    ForEach(Array(self.threads.enumerated()), id: \.offset) { index, thread in
                        ThreadDebuggerView(thread: thread)
                            .id("\(refreshID)-\(index)")  // Unique ID per refresh
                    }
                }
            }

            // TODO: add an option to refresh at regular intervals or any time a change happens?
            HStack {
                VStack {
                    Spacer()

                    Button(action: {
                        load()
                        refreshID += 1
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
        self.debugger.lock()

        // Threads
        self.threads.removeAll()
        for i in 0..<self.debugger.threadCount {
            self.threads.append(self.debugger.getThread(at: i))
        }

        self.debugger.unlock()
    }
}
