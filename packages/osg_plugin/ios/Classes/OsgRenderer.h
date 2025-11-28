#pragma once

#include <chrono>

class OsgRenderer {
public:
    OsgRenderer(int width, int height);
    ~OsgRenderer();

    void render();
    int getTextureId() const;
    void resize(int width, int height);
    void readPixels(void* buffer);
    
    // Input events
    void onMouseMove(int x, int y);
    void onMouseButton(int button, bool pressed, int x, int y);
    void onKeyEvent(int keyCode, bool pressed);
    void onScroll(double deltaX, double deltaY);
    
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

private:
    void softwareRenderABC();
    void drawDigit(int digit, int x, int y, int size, unsigned char r, unsigned char g, unsigned char b);
    void drawNumber(int num, int x, int y, int size, unsigned char r, unsigned char g, unsigned char b);
    void drawFPS();
    void drawDebugInfo();
    void drawMouseCursor();

    unsigned char* _pixelData;
    int _width;
    int _height;
    int _frameCount;
    
    // Input state
    int _mouseX, _mouseY;
    bool _mousePressed;
    int _lastKeyCode;
    
    // FPS tracking
    std::chrono::steady_clock::time_point _lastFrameTime;
    double _fps;
    double _frameTimeAccum;
    int _fpsFrameCount;
};
