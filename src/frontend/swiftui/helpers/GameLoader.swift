import Foundation

func createGameFromFile(path: String) -> Game? {
    let loader = HydraLoader(path: HydraString(path))

    // Get name and author
    var name = ""
    var author = ""
    let nacp = loader.loadNacp()
    if let nacp = nacp {
        let title = nacp.title
        name = title.name.value
        author = title.author.value
    } else {
        name = URL(fileURLWithPath: path).deletingPathExtension().lastPathComponent
        author = "Unknown"
    }

    return Game(loader: loader, name: name, author: author)
}
