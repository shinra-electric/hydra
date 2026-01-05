import SwiftUI

struct StringList: View {
    @Binding var items: [String]

    var body: some View {
        List {
            ForEach(items, id: \.self) { item in
                HStack {
                    Text(item)
                    Spacer()
                    DeleteButton(action: {
                        if let index = items.firstIndex(of: item) {
                            items.remove(at: index)
                        }
                    })
                }
            }
        }
    }
}
