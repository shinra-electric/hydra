import Foundation

func createGameFromFile(path: String) -> Game? {
    let loader = HydraLoader(path: path)

    // Get name and author
    var name = ""
    var author = ""
    let nacp = loader.loadNacp()
    if let nacp = nacp {
        let title = nacp.title
        name = title.name
        author = title.author
    } else {
        name = URL(fileURLWithPath: path).deletingPathExtension().lastPathComponent
        author = "Unknown"
    }

    return Game(loader: loader, name: name, author: author)
}
