import MetalKit
import SwiftUI

class MetalLayerCoordinator: NSObject {
    private var emulationContext: Binding<UnsafeMutableRawPointer?>

    private var layer: CAMetalLayer? = nil
    private var displayLink: CADisplayLink? = nil

    init(emulationContext: Binding<UnsafeMutableRawPointer?>) {
        self.emulationContext = emulationContext
        super.init()
    }

    deinit {
        displayLink?.invalidate()
    }

    func setView(_ view: NSView) {
        displayLink?.invalidate()

        var layer = view.layer as! CAMetalLayer
        self.layer = layer

        // Set surface
        if let emulationContext = self.emulationContext.wrappedValue {
            withUnsafeMutablePointer(to: &layer) { surface in
                hydra_emulation_context_set_surface(emulationContext, surface)
            }
        }

        // Display link
        self.displayLink = view.displayLink(target: self, selector: #selector(handleDisplayLink))
        self.displayLink?.add(to: .main, forMode: .common)
    }

    @objc func handleDisplayLink() {
        print("PRESENT")
    }
}

struct MetalView: NSViewRepresentable {
    @Binding var emulationContext: UnsafeMutableRawPointer?

    func makeNSView(context: Context) -> NSView {
        let view = NSView(frame: .zero)

        view.layer = CAMetalLayer()
        //view.wantsLayer = true

        context.coordinator.setView(view)

        return view
    }

    func updateNSView(_ view: NSView, context: Context) {
        guard let layer = view.layer as? CAMetalLayer else { return }
        layer.frame = view.bounds
        context.coordinator.setView(view)
    }

    func makeCoordinator() -> MetalLayerCoordinator {
        return MetalLayerCoordinator(emulationContext: self.$emulationContext)
    }
}
