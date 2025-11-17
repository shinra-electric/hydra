import Foundation

extension Float {
    func rounded(toPlaces places: Int, rule: FloatingPointRoundingRule) -> Float {
        let divisor = pow(10.0, Float(places))
        return (self * divisor).rounded(rule) / divisor
    }
}

extension Double {
    func rounded(toPlaces places: Int, rule: FloatingPointRoundingRule) -> Double {
        let divisor = pow(10.0, Double(places))
        return (self * divisor).rounded(rule) / divisor
    }
}
