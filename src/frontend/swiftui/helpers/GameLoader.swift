func createGameFromFile(path: String) -> Game? {
    guard let loader = hydra_create_loader_from_file(path) else {
        return nil
    }

    // Get name and author
    var name = ""
    var author = ""
    let nacp = hydra_loader_load_nacp(loader)
    if let nacp = nacp {
        let title = hydra_nacp_get_title(nacp)
        name = String(cString: hydra_nacp_title_get_name(title))
        author = String(cString: hydra_nacp_title_get_author(title))
        hydra_nacp_destroy(nacp)
    } else {
        name = URL(fileURLWithPath: path).deletingPathExtension().lastPathComponent
        author = "Unknown"
    }

    return Game(loader: loader, name: name, author: author)
}
