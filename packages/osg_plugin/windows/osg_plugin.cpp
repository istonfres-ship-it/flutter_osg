#include "osg_plugin.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

namespace osg_plugin {

// static
void OsgPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "com.example.osg/renderer",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<OsgPlugin>(registrar->texture_registrar());

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

OsgPlugin::OsgPlugin(flutter::TextureRegistrar* texture_registrar)
    : texture_registrar_(texture_registrar) {}

OsgPlugin::~OsgPlugin() {}

void OsgPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  
  const auto& method_name = method_call.method_name();

  if (method_name == "createRenderer") {
    int width = 800;
    int height = 600;
    
    // Parse arguments if provided
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments) {
      auto width_it = arguments->find(flutter::EncodableValue("width"));
      if (width_it != arguments->end()) {
        if (auto* w = std::get_if<int32_t>(&width_it->second)) {
          width = *w;
        }
      }
      auto height_it = arguments->find(flutter::EncodableValue("height"));
      if (height_it != arguments->end()) {
        if (auto* h = std::get_if<int32_t>(&height_it->second)) {
          height = *h;
        }
      }
    }

    osg_texture_ = std::make_unique<OsgTexture>(width, height, texture_registrar_);
    int64_t texture_id = osg_texture_->Register();
    result->Success(flutter::EncodableValue(texture_id));
  } else if (method_name == "renderFrame") {
    if (osg_texture_) {
      osg_texture_->Render();
    }
    result->Success();
  } else if (method_name == "resize") {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments && osg_texture_) {
      int width = 0, height = 0;
      auto width_it = arguments->find(flutter::EncodableValue("width"));
      if (width_it != arguments->end()) {
        if (auto* w = std::get_if<int32_t>(&width_it->second)) {
          width = *w;
        }
      }
      auto height_it = arguments->find(flutter::EncodableValue("height"));
      if (height_it != arguments->end()) {
        if (auto* h = std::get_if<int32_t>(&height_it->second)) {
          height = *h;
        }
      }
      if (width > 0 && height > 0) {
        osg_texture_->Resize(width, height);
      }
    }
    result->Success();
  } else if (method_name == "getSize") {
    if (osg_texture_) {
      auto size = osg_texture_->GetSize();
      flutter::EncodableMap response;
      response[flutter::EncodableValue("width")] = flutter::EncodableValue(size.first);
      response[flutter::EncodableValue("height")] = flutter::EncodableValue(size.second);
      result->Success(flutter::EncodableValue(response));
    } else {
      result->Success();
    }
  } else if (method_name == "mouseMove") {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments && osg_texture_) {
      int x = 0, y = 0;
      auto x_it = arguments->find(flutter::EncodableValue("x"));
      if (x_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&x_it->second)) x = *val;
      }
      auto y_it = arguments->find(flutter::EncodableValue("y"));
      if (y_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&y_it->second)) y = *val;
      }
      osg_texture_->OnMouseMove(x, y);
    }
    result->Success();
  } else if (method_name == "mouseButton") {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments && osg_texture_) {
      int button = 0, x = 0, y = 0;
      bool pressed = false;
      auto button_it = arguments->find(flutter::EncodableValue("button"));
      if (button_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&button_it->second)) button = *val;
      }
      auto pressed_it = arguments->find(flutter::EncodableValue("pressed"));
      if (pressed_it != arguments->end()) {
        if (auto* val = std::get_if<bool>(&pressed_it->second)) pressed = *val;
      }
      auto x_it = arguments->find(flutter::EncodableValue("x"));
      if (x_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&x_it->second)) x = *val;
      }
      auto y_it = arguments->find(flutter::EncodableValue("y"));
      if (y_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&y_it->second)) y = *val;
      }
      osg_texture_->OnMouseButton(button, pressed, x, y);
    }
    result->Success();
  } else if (method_name == "keyEvent") {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments && osg_texture_) {
      int keyCode = 0;
      bool pressed = false;
      auto key_it = arguments->find(flutter::EncodableValue("keyCode"));
      if (key_it != arguments->end()) {
        if (auto* val = std::get_if<int32_t>(&key_it->second)) keyCode = *val;
      }
      auto pressed_it = arguments->find(flutter::EncodableValue("pressed"));
      if (pressed_it != arguments->end()) {
        if (auto* val = std::get_if<bool>(&pressed_it->second)) pressed = *val;
      }
      osg_texture_->OnKeyEvent(keyCode, pressed);
    }
    result->Success();
  } else if (method_name == "scroll") {
    const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments && osg_texture_) {
      double deltaX = 0.0, deltaY = 0.0;
      auto dx_it = arguments->find(flutter::EncodableValue("deltaX"));
      if (dx_it != arguments->end()) {
        if (auto* val = std::get_if<double>(&dx_it->second)) deltaX = *val;
      }
      auto dy_it = arguments->find(flutter::EncodableValue("deltaY"));
      if (dy_it != arguments->end()) {
        if (auto* val = std::get_if<double>(&dy_it->second)) deltaY = *val;
      }
      osg_texture_->OnScroll(deltaX, deltaY);
    }
    result->Success();
  } else {
    result->NotImplemented();
  }
}

}  // namespace osg_plugin
