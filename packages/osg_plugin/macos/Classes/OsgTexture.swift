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
    
    func resize(width: Int, height: Int) {
        guard width > 0, height > 0 else { return }
        if self.width == width && self.height == height { return }
        
        self.width = width
        self.height = height
        if let renderer = renderer {
            resizeOsgRenderer(renderer, Int32(width), Int32(height))
        }
    }
    
    func getSize() -> (Int, Int) {
        return (width, height)
    }

    func render() {
        if let renderer = renderer {
            renderOsgFrame(renderer)
            registry.textureFrameAvailable(textureId)
        }
    }
    
    // Input event handlers
    func onMouseMove(x: Int, y: Int) {
        if let renderer = renderer {
            osgMouseMove(renderer, Int32(x), Int32(y))
        }
    }
    
    func onMouseButton(button: Int, pressed: Bool, x: Int, y: Int) {
        if let renderer = renderer {
            osgMouseButton(renderer, Int32(button), pressed ? 1 : 0, Int32(x), Int32(y))
        }
    }
    
    func onKeyEvent(keyCode: Int, pressed: Bool) {
        if let renderer = renderer {
            osgKeyEvent(renderer, Int32(keyCode), pressed ? 1 : 0)
        }
    }
    
    func onScroll(deltaX: Double, deltaY: Double) {
        if let renderer = renderer {
            osgScroll(renderer, deltaX, deltaY)
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
                
                let tempBuffer = UnsafeMutableRawPointer.allocate(byteCount: width * height * 4, alignment: 1)
                defer { tempBuffer.deallocate() }
                
                readOsgPixels(renderer, tempBuffer)
                
                let srcPtr = tempBuffer.assumingMemoryBound(to: UInt8.self)
                let dstPtr = baseAddress.assumingMemoryBound(to: UInt8.self)
                
                for y in 0..<height {
                    let srcY = height - 1 - y
                    for x in 0..<width {
                        let srcIdx = (srcY * width + x) * 4
                        let dstIdx = y * bytesPerRow + x * 4
                        dstPtr[dstIdx + 0] = srcPtr[srcIdx + 2]
                        dstPtr[dstIdx + 1] = srcPtr[srcIdx + 1]
                        dstPtr[dstIdx + 2] = srcPtr[srcIdx + 0]
                        dstPtr[dstIdx + 3] = srcPtr[srcIdx + 3]
                    }
                }
            }
            CVPixelBufferUnlockBaseAddress(buffer, [])
            return Unmanaged.passRetained(buffer)
        }
        return nil
    }
}
