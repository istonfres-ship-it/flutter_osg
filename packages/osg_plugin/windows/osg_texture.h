#ifndef FLUTTER_PLUGIN_OSG_TEXTURE_H_
#define FLUTTER_PLUGIN_OSG_TEXTURE_H_

#include <flutter/texture_registrar.h>
#include <memory>
#include <mutex>
#include <vector>

namespace osg_plugin {

class OsgTexture {
 public:
  OsgTexture(int width, int height, flutter::TextureRegistrar* texture_registrar);
  ~OsgTexture();

  // Register texture and return texture ID
  int64_t Register();

  // Render a frame
  void Render();

  // Resize the texture
  void Resize(int width, int height);

  // Get current size
  std::pair<int, int> GetSize() const;

  // Input event handlers
  void OnMouseMove(int x, int y);
  void OnMouseButton(int button, bool pressed, int x, int y);
  void OnKeyEvent(int keyCode, bool pressed);
  void OnScroll(double deltaX, double deltaY);

 private:
  // Copy pixels from renderer to our buffer (with RGBA to BGRA conversion and flip)
  const FlutterDesktopPixelBuffer* CopyPixelBuffer(size_t width, size_t height);

  void* renderer_;
  int width_;
  int height_;
  int64_t texture_id_;
  flutter::TextureRegistrar* texture_registrar_;
  std::unique_ptr<flutter::TextureVariant> texture_;
  std::vector<uint8_t> pixel_buffer_;
  FlutterDesktopPixelBuffer flutter_pixel_buffer_;
  std::mutex mutex_;
};

}  // namespace osg_plugin

#endif  // FLUTTER_PLUGIN_OSG_TEXTURE_H_
