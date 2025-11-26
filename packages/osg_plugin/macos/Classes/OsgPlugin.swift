import Cocoa
import FlutterMacOS

public class OsgPlugin: NSObject, FlutterPlugin {
  var osgTexture: OsgTexture?
  var registry: FlutterTextureRegistry?

  public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "com.example.osg/renderer", binaryMessenger: registrar.messenger)
    let instance = OsgPlugin()
    instance.registry = registrar.textures
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "createRenderer":
      if let registry = registry {
          // Fixed size for demo. In real app, pass size from Flutter arguments.
          let texture = OsgTexture(width: 800, height: 600, registry: registry)
          let textureId = texture.register()
          self.osgTexture = texture
          result(textureId)
      } else {
          result(FlutterError(code: "NO_REGISTRY", message: "Texture registry not available", details: nil))
      }
    case "renderFrame":
      self.osgTexture?.render()
      result(nil)
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}
