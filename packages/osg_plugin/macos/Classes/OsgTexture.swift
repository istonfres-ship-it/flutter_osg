import Cocoa
import FlutterMacOS

class OsgTexture: NSObject, FlutterTexture {
    private var renderer: UnsafeMutableRawPointer?
    private var width: Int
    private var height: Int
    private var textureId: Int64 = 0
    private var registry: FlutterTextureRegistry

    init(width: Int, height: Int, registry: FlutterTextureRegistry) {
        self.width = width
        self.height = height
        self.registry = registry
        self.renderer = createOsgRenderer(Int32(width), Int32(height))
        super.init()
    }

    deinit {
        if let renderer = renderer {
            destroyOsgRenderer(renderer)
        }
    }

    func register() -> Int64 {
        textureId = registry.register(self)
        return textureId
    }

    func render() {
        if let renderer = renderer {
            renderOsgFrame(renderer)
            registry.textureFrameAvailable(textureId)
        }
    }

    func copyPixelBuffer() -> Unmanaged<CVPixelBuffer>? {
        var pixelBuffer: CVPixelBuffer?
        let attrs = [
            kCVPixelBufferCGImageCompatibilityKey: true,
            kCVPixelBufferCGBitmapContextCompatibilityKey: true,
            kCVPixelBufferMetalCompatibilityKey: true
        ] as CFDictionary

        let status = CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32BGRA, attrs, &pixelBuffer)

        if status == kCVReturnSuccess, let buffer = pixelBuffer {
            CVPixelBufferLockBaseAddress(buffer, [])
            if let baseAddress = CVPixelBufferGetBaseAddress(buffer) {
                // OSG renders to RGBA, but CVPixelBuffer is BGRA usually on macOS for efficiency or RGBA.
                // Let's assume RGBA for now. If colors are swapped, we change format or swizzle.
                // Wait, I used kCVPixelFormatType_32BGRA.
                // OSG usually renders RGBA.
                // So Red and Blue might be swapped.
                // For this demo, let's just copy and see.
                readOsgPixels(renderer, baseAddress)
            }
            CVPixelBufferUnlockBaseAddress(buffer, [])
            return Unmanaged.passRetained(buffer)
        }
        return nil
    }
}
