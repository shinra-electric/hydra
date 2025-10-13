func toSwiftString(_ str: hydra_string) -> String? {
    let data = Data(bytes: str.data, count: str.size)
    return String(data: data, encoding: String.Encoding.utf8)
}

func toHydraString(_ str: String) -> hydra_string? {
    guard let data = str.data(using: String.Encoding.utf8) else {
        return nil
    }

    return data.withUnsafeBytes { bytes in
        let cCharPointer = bytes.bindMemory(to: CChar.self).baseAddress  //let rawPtr = rawBufferPointer.baseAddress!
        return hydra_string(data: cCharPointer, size: data.count)
    }
}
