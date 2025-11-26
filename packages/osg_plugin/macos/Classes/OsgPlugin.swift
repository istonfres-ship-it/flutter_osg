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
      if let registry = registry, let args = call.arguments as? [String: Any] {
          let width = args["width"] as? Int ?? 800
          let height = args["height"] as? Int ?? 600
          let texture = OsgTexture(width: width, height: height, registry: registry)
          let textureId = texture.register()
          self.osgTexture = texture
          result(textureId)
      } else if let registry = registry {
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
    case "resize":
      if let args = call.arguments as? [String: Any],
         let width = args["width"] as? Int,
         let height = args["height"] as? Int {
          self.osgTexture?.resize(width: width, height: height)
      }
      result(nil)
    case "getSize":
      if let texture = self.osgTexture {
          let size = texture.getSize()
          result(["width": size.0, "height": size.1])
      } else {
          result(nil)
      }
    case "mouseMove":
      if let args = call.arguments as? [String: Any],
         let x = args["x"] as? Int,
         let y = args["y"] as? Int {
          self.osgTexture?.onMouseMove(x: x, y: y)
      }
      result(nil)
    case "mouseButton":
      if let args = call.arguments as? [String: Any],
         let button = args["button"] as? Int,
         let pressed = args["pressed"] as? Bool,
         let x = args["x"] as? Int,
         let y = args["y"] as? Int {
          self.osgTexture?.onMouseButton(button: button, pressed: pressed, x: x, y: y)
      }
      result(nil)
    case "keyEvent":
      if let args = call.arguments as? [String: Any],
         let keyCode = args["keyCode"] as? Int,
         let pressed = args["pressed"] as? Bool {
          self.osgTexture?.onKeyEvent(keyCode: keyCode, pressed: pressed)
      }
      result(nil)
    case "scroll":
      if let args = call.arguments as? [String: Any],
         let deltaX = args["deltaX"] as? Double,
         let deltaY = args["deltaY"] as? Double {
          self.osgTexture?.onScroll(deltaX: deltaX, deltaY: deltaY)
      }
      result(nil)
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}
