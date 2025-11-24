import MetalKit
import SwiftUI

class MetalLayerCoordinator: NSObject {
    private var emulationContext: Binding<HydraEmulationContext?>
    private var fps: Binding<Int>
    private var layer: CAMetalLayer? = nil
    private var displayLink: CADisplayLink? = nil
    private var surfaceSet = false

    init(emulationContext: Binding<HydraEmulationContext?>, fps: Binding<Int>) {
        self.emulationContext = emulationContext
        self.fps = fps
        super.init()
    }

    deinit {
        self.displayLink?.invalidate()
    }

    #if os(macOS)
        func setView(_ view: NSView) {
            self.displayLink?.invalidate()
            self.layer = view.layer as? CAMetalLayer
            self.surfaceSet = false

            // Display link
            self.displayLink = view.displayLink(
                target: self, selector: #selector(handleDisplayLink))
            self.displayLink?.add(to: .main, forMode: .common)
        }
    #else
        func setView(_ view: UIView) {
            self.displayLink?.invalidate()
            self.layer = view.layer as? CAMetalLayer
            self.surfaceSet = false

            // Display link
            self.displayLink = CADisplayLink(target: self, selector: #selector(handleDisplayLink))
            self.displayLink?.add(to: .main, forMode: .commonModes)
        }
    #endif

    @objc func handleDisplayLink() {
        if let emulationContext = self.emulationContext.wrappedValue {
            // Set the surface if its not already set
            if !self.surfaceSet {
                guard let layer = self.layer else {
                    return
                }
                emulationContext.surface = Unmanaged.passUnretained(layer).toOpaque()
                self.surfaceSet = true
            }

            if emulationContext.isRunning() {
                // Present
                var dtAverageUpdated = false
                emulationContext.progressFrame(
                    width: UInt32(self.layer!.drawableSize.width),
                    height: UInt32(self.layer!.drawableSize.height),
                    dtAverageUpdated: &dtAverageUpdated)

                // Update
                if dtAverageUpdated {
                    let dt = emulationContext.getLastDeltaTimeAverage()
                    if dt != 0.0 {
                        fps.wrappedValue = Int((1.0 / dt).rounded())
                    } else {
                        fps.wrappedValue = 0
                    }
                }
            }
        }
    }
}

#if os(macOS)
    struct MetalView: NSViewRepresentable {
        @Binding var emulationContext: HydraEmulationContext?
        @Binding var fps: Int

        func makeNSView(context: Context) -> NSView {
            let view = NSView(frame: .zero)
            let layer = CAMetalLayer()

            layer.displaySyncEnabled = true
            view.layer = layer
            view.wantsLayer = true
            context.coordinator.setView(view)

            return view
        }

        func updateNSView(_ view: NSView, context: Context) {
            guard let layer = view.layer as? CAMetalLayer else { return }
            layer.frame = view.bounds
            context.coordinator.setView(view)
        }

        func makeCoordinator() -> MetalLayerCoordinator {
            return MetalLayerCoordinator(emulationContext: self.$emulationContext, fps: self.$fps)
        }
    }
#else
    class MetalBackedView: UIView {
        override class var layerClass: AnyClass {
            return CAMetalLayer.self
        }
    }

    struct MetalView: UIViewRepresentable {
        @Binding var emulationContext: HydraEmulationContext?
        @Binding var fps: Int

        func makeUIView(context: Context) -> UIView {
            let view = MetalBackedView(frame: .zero)
            context.coordinator.setView(view)
            return view
        }

        func updateUIView(_ view: UIView, context: Context) {
            if let layer = view.layer.sublayers?.first as? CAMetalLayer {
                layer.frame = view.bounds
            }
            context.coordinator.setView(view)
        }

        func makeCoordinator() -> MetalLayerCoordinator {
            return MetalLayerCoordinator(emulationContext: self.$emulationContext, fps: self.$fps)
        }
    }
#endif
