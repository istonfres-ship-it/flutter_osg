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
                let bytesPerRow = CVPixelBufferGetBytesPerRow(buffer)
                let srcBytesPerRow = width * 4
                
                // Allocate temporary buffer to read OSG pixels (RGBA format)
                let tempBuffer = UnsafeMutableRawPointer.allocate(byteCount: width * height * 4, alignment: 1)
                defer { tempBuffer.deallocate() }
                
                // Read pixels from OSG (RGBA format)
                readOsgPixels(renderer, tempBuffer)
                
                // Convert RGBA to BGRA and flip vertically
                let srcPtr = tempBuffer.assumingMemoryBound(to: UInt8.self)
                let dstPtr = baseAddress.assumingMemoryBound(to: UInt8.self)
                
                for y in 0..<height {
                    let srcY = height - 1 - y  // Flip Y
                    for x in 0..<width {
                        let srcIdx = (srcY * width + x) * 4
                        let dstIdx = y * bytesPerRow + x * 4
                        
                        // RGBA -> BGRA: swap R and B
                        dstPtr[dstIdx + 0] = srcPtr[srcIdx + 2]  // B <- R
                        dstPtr[dstIdx + 1] = srcPtr[srcIdx + 1]  // G <- G
                        dstPtr[dstIdx + 2] = srcPtr[srcIdx + 0]  // R <- B
                        dstPtr[dstIdx + 3] = srcPtr[srcIdx + 3]  // A <- A
                    }
                }
            }
            CVPixelBufferUnlockBaseAddress(buffer, [])
            return Unmanaged.passRetained(buffer)
        }
        return nil
    }
}
