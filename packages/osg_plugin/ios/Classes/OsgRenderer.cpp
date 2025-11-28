#include "OsgRenderer.h"
#include <cstring>
#include <cmath>
#include <chrono>
#include <algorithm>

OsgRenderer::OsgRenderer(int width, int height) 
    : _width(width), _height(height), _pixelData(nullptr), _frameCount(0),
      _mouseX(0), _mouseY(0), _mousePressed(false), _lastKeyCode(0),
      _lastFrameTime(std::chrono::steady_clock::now()), _fps(0.0), _frameTimeAccum(0.0), _fpsFrameCount(0) {
    _pixelData = new unsigned char[_width * _height * 4];
    memset(_pixelData, 0, _width * _height * 4);
}

OsgRenderer::~OsgRenderer() {
    if (_pixelData) {
        delete[] _pixelData;
        _pixelData = nullptr;
    }
}

// Draw a simple digit at position (7-segment style)
void OsgRenderer::drawDigit(int digit, int x, int y, int size, unsigned char r, unsigned char g, unsigned char b) {
    bool segments[10][7] = {
        {1,1,1,0,1,1,1}, {0,0,1,0,0,1,0}, {1,0,1,1,1,0,1}, {1,0,1,1,0,1,1}, {0,1,1,1,0,1,0},
        {1,1,0,1,0,1,1}, {1,1,0,1,1,1,1}, {1,0,1,0,0,1,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    
    int w = size / 2, h = size, t = std::max(2, size / 8);
    auto drawH = [&](int sx, int sy, int len) {
        for (int dy = 0; dy < t; dy++) for (int dx = 0; dx < len; dx++) {
            int px = sx + dx, py = sy + dy;
            if (px >= 0 && px < _width && py >= 0 && py < _height) {
                int idx = (py * _width + px) * 4;
                _pixelData[idx] = r; _pixelData[idx+1] = g; _pixelData[idx+2] = b; _pixelData[idx+3] = 255;
            }
        }
    };
    auto drawV = [&](int sx, int sy, int len) {
        for (int dy = 0; dy < len; dy++) for (int dx = 0; dx < t; dx++) {
            int px = sx + dx, py = sy + dy;
            if (px >= 0 && px < _width && py >= 0 && py < _height) {
                int idx = (py * _width + px) * 4;
                _pixelData[idx] = r; _pixelData[idx+1] = g; _pixelData[idx+2] = b; _pixelData[idx+3] = 255;
            }
        }
    };
    
    if (digit < 0 || digit > 9) return;
    if (segments[digit][0]) drawH(x, y + h - t, w);
    if (segments[digit][1]) drawV(x, y + h/2, h/2);
    if (segments[digit][2]) drawV(x + w - t, y + h/2, h/2);
    if (segments[digit][3]) drawH(x, y + h/2 - t/2, w);
    if (segments[digit][4]) drawV(x, y, h/2);
    if (segments[digit][5]) drawV(x + w - t, y, h/2);
    if (segments[digit][6]) drawH(x, y, w);
}

void OsgRenderer::drawNumber(int num, int x, int y, int size, unsigned char r, unsigned char g, unsigned char b) {
    if (num < 0) num = 0;
    int digits[10], count = 0;
    if (num == 0) { digits[count++] = 0; }
    else { while (num > 0 && count < 10) { digits[count++] = num % 10; num /= 10; } }
    int spacing = size / 2 + 4;
    for (int i = count - 1; i >= 0; i--) {
        drawDigit(digits[i], x, y, size, r, g, b);
        x += spacing;
    }
}

void OsgRenderer::drawFPS() {
    int fps = (int)_fps;
    // Background
    for (int dy = 0; dy < 30; dy++) for (int dx = 0; dx < 80; dx++) {
        int px = 5 + dx, py = _height - 35 + dy;
        if (px >= 0 && px < _width && py >= 0 && py < _height) {
            int idx = (py * _width + px) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 0; _pixelData[idx+2] = 0; _pixelData[idx+3] = 180;
        }
    }
    drawNumber(fps, 10, _height - 32, 20, 0, 255, 0);
}

void OsgRenderer::drawDebugInfo() {
    // Background for size/mouse info
    for (int dy = 0; dy < 50; dy++) for (int dx = 0; dx < 180; dx++) {
        int px = 5 + dx, py = _height - 90 + dy;
        if (px >= 0 && px < _width && py >= 0 && py < _height) {
            int idx = (py * _width + px) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 0; _pixelData[idx+2] = 0; _pixelData[idx+3] = 150;
        }
    }
    // Size
    drawNumber(_width, 10, _height - 60, 14, 255, 255, 255);
    drawNumber(_height, 80, _height - 60, 14, 255, 255, 255);
    // Mouse
    drawNumber(_mouseX, 10, _height - 85, 14, 255, 200, 0);
    drawNumber(_mouseY, 80, _height - 85, 14, 255, 200, 0);
    // Key
    if (_lastKeyCode > 0) drawNumber(_lastKeyCode, 150, _height - 85, 14, 255, 100, 100);
}

void OsgRenderer::drawMouseCursor() {
    int cx = _mouseX, cy = _height - _mouseY;
    int size = _mousePressed ? 15 : 10;
    unsigned char r = _mousePressed ? 255 : 200, g = _mousePressed ? 100 : 200, b = _mousePressed ? 100 : 200;
    
    for (int i = -size; i <= size; i++) {
        if (cx + i >= 0 && cx + i < _width && cy >= 0 && cy < _height) {
            int idx = (cy * _width + cx + i) * 4;
            _pixelData[idx] = r; _pixelData[idx+1] = g; _pixelData[idx+2] = b; _pixelData[idx+3] = 255;
        }
        if (cx >= 0 && cx < _width && cy + i >= 0 && cy + i < _height) {
            int idx = ((cy + i) * _width + cx) * 4;
            _pixelData[idx] = r; _pixelData[idx+1] = g; _pixelData[idx+2] = b; _pixelData[idx+3] = 255;
        }
    }
    if (_mousePressed) {
        for (int angle = 0; angle < 360; angle += 10) {
            double rad = angle * 3.14159 / 180.0;
            int px = cx + (int)(cos(rad) * size), py = cy + (int)(sin(rad) * size);
            if (px >= 0 && px < _width && py >= 0 && py < _height) {
                int idx = (py * _width + px) * 4;
                _pixelData[idx] = 255; _pixelData[idx+1] = 50; _pixelData[idx+2] = 50; _pixelData[idx+3] = 255;
            }
        }
    }
}

void OsgRenderer::softwareRenderABC() {
    // Calculate FPS
    auto now = std::chrono::steady_clock::now();
    double frameTime = std::chrono::duration<double>(now - _lastFrameTime).count();
    _lastFrameTime = now;
    _frameTimeAccum += frameTime;
    _fpsFrameCount++;
    if (_frameTimeAccum >= 0.5) {
        _fps = _fpsFrameCount / _frameTimeAccum;
        _frameTimeAccum = 0;
        _fpsFrameCount = 0;
    }
    
    // Clear background
    for (int i = 0; i < _width * _height; i++) {
        _pixelData[i*4] = 50; _pixelData[i*4+1] = 50; _pixelData[i*4+2] = 80; _pixelData[i*4+3] = 255;
    }
    
    // Draw ABC letters (scaled to window size)
    int letterSize = std::min(_width, _height) / 6;
    int spacing = letterSize + 30;
    int startX = _width / 2 - (int)(1.5 * spacing);
    int startY = _height / 2 - letterSize / 2;
    
    // Letter A (yellow)
    for (int i = 0; i < letterSize; i++) {
        int x1 = startX + i / 2, y1 = startY + i;
        if (x1 >= 0 && x1 < _width && y1 >= 0 && y1 < _height) {
            int idx = (y1 * _width + x1) * 4;
            _pixelData[idx] = 255; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
        int x2 = startX + letterSize - i / 2, y2 = startY + i;
        if (x2 >= 0 && x2 < _width && y2 >= 0 && y2 < _height) {
            int idx = (y2 * _width + x2) * 4;
            _pixelData[idx] = 255; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    int crossY = startY + letterSize / 2;
    for (int x = startX + letterSize/4; x < startX + letterSize*3/4; x++) {
        if (x >= 0 && x < _width && crossY >= 0 && crossY < _height) {
            int idx = (crossY * _width + x) * 4;
            _pixelData[idx] = 255; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    
    // Letter B (green)
    startX = _width / 2 - spacing / 2;
    for (int y = startY; y < startY + letterSize; y++) {
        if (startX >= 0 && startX < _width && y >= 0 && y < _height) {
            int idx = (y * _width + startX) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    for (int x = startX; x < startX + letterSize*2/3; x++) {
        for (int yoff : {0, letterSize/2, letterSize}) {
            int y = startY + yoff;
            if (x >= 0 && x < _width && y >= 0 && y < _height) {
                int idx = (y * _width + x) * 4;
                _pixelData[idx] = 0; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
            }
        }
    }
    int bX = startX + letterSize*2/3;
    for (int y = startY; y < startY + letterSize/2; y++) {
        if (bX >= 0 && bX < _width && y >= 0 && y < _height) {
            int idx = (y * _width + bX) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    for (int y = startY + letterSize/2; y < startY + letterSize; y++) {
        if (bX >= 0 && bX < _width && y >= 0 && y < _height) {
            int idx = (y * _width + bX) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 255; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    
    // Letter C (blue)
    startX = _width / 2 + spacing / 2;
    int centerY = startY + letterSize / 2;
    int radius = letterSize / 2;
    for (int angle = 45; angle < 315; angle++) {
        double rad = angle * 3.14159 / 180.0;
        int x = startX + radius + (int)(cos(rad) * radius);
        int y = centerY + (int)(sin(rad) * radius);
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx] = 0; _pixelData[idx+1] = 128; _pixelData[idx+2] = 255; _pixelData[idx+3] = 255;
        }
    }
    
    // Animated dot
    _frameCount++;
    int dotX = _width / 4 + (int)(sin(_frameCount * 0.05) * (_width / 6));
    int dotY = _height - 100;
    for (int dy = -8; dy <= 8; dy++) for (int dx = -8; dx <= 8; dx++) {
        int x = dotX + dx, y = dotY + dy;
        if (x >= 0 && x < _width && y >= 0 && y < _height && dx*dx + dy*dy <= 64) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx] = 255; _pixelData[idx+1] = 0; _pixelData[idx+2] = 0; _pixelData[idx+3] = 255;
        }
    }
    
    drawFPS();
    drawDebugInfo();
    drawMouseCursor();
}

void OsgRenderer::render() { softwareRenderABC(); }
int OsgRenderer::getTextureId() const { return 0; }

void OsgRenderer::resize(int width, int height) {
    if (width <= 0 || height <= 0 || (width == _width && height == _height)) return;
    _width = width; _height = height;
    delete[] _pixelData;
    _pixelData = new unsigned char[_width * _height * 4];
    memset(_pixelData, 0, _width * _height * 4);
}

void OsgRenderer::readPixels(void* buffer) {
    if (_pixelData && buffer) memcpy(buffer, _pixelData, _width * _height * 4);
}

void OsgRenderer::onMouseMove(int x, int y) { _mouseX = x; _mouseY = y; }
void OsgRenderer::onMouseButton(int button, bool pressed, int x, int y) { _mouseX = x; _mouseY = y; _mousePressed = pressed; }
void OsgRenderer::onKeyEvent(int keyCode, bool pressed) { if (pressed) _lastKeyCode = keyCode; }
void OsgRenderer::onScroll(double deltaX, double deltaY) {}
