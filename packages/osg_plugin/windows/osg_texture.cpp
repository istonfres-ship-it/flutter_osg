#include "osg_texture.h"
#include "OsgBridge.h"

#include <cstring>

namespace osg_plugin {

OsgTexture::OsgTexture(int width, int height, flutter::TextureRegistrar* texture_registrar)
    : width_(width),
      height_(height),
      texture_id_(-1),
      texture_registrar_(texture_registrar),
      renderer_(nullptr) {
  renderer_ = createOsgRenderer(width, height);
  pixel_buffer_.resize(width * height * 4);
  
  // Initialize flutter pixel buffer
  flutter_pixel_buffer_.buffer = nullptr;
  flutter_pixel_buffer_.width = 0;
  flutter_pixel_buffer_.height = 0;
}

OsgTexture::~OsgTexture() {
  if (texture_id_ >= 0 && texture_registrar_) {
    texture_registrar_->UnregisterTexture(texture_id_);
  }
  if (renderer_) {
    destroyOsgRenderer(renderer_);
    renderer_ = nullptr;
  }
}

int64_t OsgTexture::Register() {
  // Create a PixelBufferTexture
  texture_ = std::make_unique<flutter::TextureVariant>(
      flutter::PixelBufferTexture(
          [this](size_t width, size_t height) -> const FlutterDesktopPixelBuffer* {
            return CopyPixelBuffer(width, height);
          }));
  
  texture_id_ = texture_registrar_->RegisterTexture(texture_.get());
  return texture_id_;
}

const FlutterDesktopPixelBuffer* OsgTexture::CopyPixelBuffer(size_t width, size_t height) {
  std::lock_guard<std::mutex> lock(mutex_);
  
  if (!renderer_ || width_ <= 0 || height_ <= 0) {
    return nullptr;
  }

  // Ensure pixel buffer is correctly sized
  size_t buffer_size = width_ * height_ * 4;
  if (pixel_buffer_.size() != buffer_size) {
    pixel_buffer_.resize(buffer_size);
  }

  // Read pixels from renderer (RGBA format)
  std::vector<uint8_t> temp_buffer(buffer_size);
  readOsgPixels(renderer_, temp_buffer.data());

  // Convert RGBA to BGRA and flip vertically (Flutter expects top-left origin)
  for (int y = 0; y < height_; y++) {
    int srcY = height_ - 1 - y;  // Flip vertically
    for (int x = 0; x < width_; x++) {
      int srcIdx = (srcY * width_ + x) * 4;
      int dstIdx = (y * width_ + x) * 4;
      
      // RGBA to BGRA
      pixel_buffer_[dstIdx + 0] = temp_buffer[srcIdx + 2];  // B
      pixel_buffer_[dstIdx + 1] = temp_buffer[srcIdx + 1];  // G
      pixel_buffer_[dstIdx + 2] = temp_buffer[srcIdx + 0];  // R
      pixel_buffer_[dstIdx + 3] = temp_buffer[srcIdx + 3];  // A
    }
  }

  flutter_pixel_buffer_.buffer = pixel_buffer_.data();
  flutter_pixel_buffer_.width = static_cast<size_t>(width_);
  flutter_pixel_buffer_.height = static_cast<size_t>(height_);

  return &flutter_pixel_buffer_;
}

void OsgTexture::Render() {
  if (renderer_) {
    renderOsgFrame(renderer_);
    if (texture_registrar_ && texture_id_ >= 0) {
      texture_registrar_->MarkTextureFrameAvailable(texture_id_);
    }
  }
}

void OsgTexture::Resize(int width, int height) {
  std::lock_guard<std::mutex> lock(mutex_);
  
  if (width <= 0 || height <= 0) return;
  if (width == width_ && height == height_) return;

  width_ = width;
  height_ = height;
  pixel_buffer_.resize(width * height * 4);

  if (renderer_) {
    resizeOsgRenderer(renderer_, width, height);
  }
}

std::pair<int, int> OsgTexture::GetSize() const {
  return {width_, height_};
}

void OsgTexture::OnMouseMove(int x, int y) {
  if (renderer_) {
    osgMouseMove(renderer_, x, y);
  }
}

void OsgTexture::OnMouseButton(int button, bool pressed, int x, int y) {
  if (renderer_) {
    osgMouseButton(renderer_, button, pressed ? 1 : 0, x, y);
  }
}

void OsgTexture::OnKeyEvent(int keyCode, bool pressed) {
  if (renderer_) {
    osgKeyEvent(renderer_, keyCode, pressed ? 1 : 0);
  }
}

void OsgTexture::OnScroll(double deltaX, double deltaY) {
  if (renderer_) {
    osgScroll(renderer_, deltaX, deltaY);
  }
}

}  // namespace osg_plugin
