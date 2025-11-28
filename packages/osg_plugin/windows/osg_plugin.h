#ifndef FLUTTER_PLUGIN_OSG_PLUGIN_H_
#define FLUTTER_PLUGIN_OSG_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/texture_registrar.h>

#include <memory>

#include "osg_texture.h"

namespace osg_plugin {

class OsgPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  OsgPlugin(flutter::TextureRegistrar* texture_registrar);

  virtual ~OsgPlugin();

  // Disallow copy and assign.
  OsgPlugin(const OsgPlugin&) = delete;
  OsgPlugin& operator=(const OsgPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  flutter::TextureRegistrar* texture_registrar_;
  std::unique_ptr<OsgTexture> osg_texture_;
};

}  // namespace osg_plugin

#endif  // FLUTTER_PLUGIN_OSG_PLUGIN_H_
