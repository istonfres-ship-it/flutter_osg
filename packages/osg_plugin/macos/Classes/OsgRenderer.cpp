#include "OsgRenderer.h"
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osg/GraphicsContext>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <cstring>
#include <cmath>

OsgRenderer::OsgRenderer(int width, int height) : _width(width), _height(height), _pixelData(nullptr), _frameCount(0) {
    // Allocate pixel buffer
    _pixelData = new unsigned char[_width * _height * 4];
    memset(_pixelData, 0, _width * _height * 4);
    
    // For now, skip OSG and just do software rendering test
    // to verify the pixel transfer works correctly
}

OsgRenderer::~OsgRenderer() {
    _viewer = nullptr;
    if (_pixelData) {
        delete[] _pixelData;
        _pixelData = nullptr;
    }
}

// Software render "ABC" text for testing
void OsgRenderer::softwareRenderABC() {
    // Clear to dark blue background
    for (int i = 0; i < _width * _height; i++) {
        _pixelData[i * 4 + 0] = 50;   // R
        _pixelData[i * 4 + 1] = 50;   // G
        _pixelData[i * 4 + 2] = 80;   // B
        _pixelData[i * 4 + 3] = 255;  // A
    }
    
    // Draw letter A (yellow) - simple triangle shape
    int letterSize = 100;
    int startX = _width / 2 - 200;
    int startY = _height / 2 - letterSize / 2;
    
    // Letter A - two diagonal lines and a crossbar
    for (int i = 0; i < letterSize; i++) {
        // Left diagonal
        int x1 = startX + i / 2;
        int y1 = startY + i;
        if (x1 >= 0 && x1 < _width && y1 >= 0 && y1 < _height) {
            int idx = (y1 * _width + x1) * 4;
            _pixelData[idx + 0] = 255; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
        // Right diagonal
        int x2 = startX + letterSize - i / 2;
        int y2 = startY + i;
        if (x2 >= 0 && x2 < _width && y2 >= 0 && y2 < _height) {
            int idx = (y2 * _width + x2) * 4;
            _pixelData[idx + 0] = 255; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    // Crossbar for A
    int crossY = startY + letterSize / 2;
    for (int x = startX + letterSize / 4; x < startX + letterSize * 3 / 4; x++) {
        if (x >= 0 && x < _width && crossY >= 0 && crossY < _height) {
            int idx = (crossY * _width + x) * 4;
            _pixelData[idx + 0] = 255; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    
    // Letter B (green) - vertical line + bumps
    startX = _width / 2 - 50;
    for (int y = startY; y < startY + letterSize; y++) {
        int idx = (y * _width + startX) * 4;
        if (startX >= 0 && startX < _width && y >= 0 && y < _height) {
            _pixelData[idx + 0] = 0; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    // Top horizontal
    for (int x = startX; x < startX + 60; x++) {
        int y = startY + letterSize;
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx + 0] = 0; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    // Middle horizontal
    for (int x = startX; x < startX + 60; x++) {
        int y = startY + letterSize / 2;
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx + 0] = 0; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    // Bottom horizontal
    for (int x = startX; x < startX + 60; x++) {
        int y = startY;
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx + 0] = 0; _pixelData[idx + 1] = 255; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
        }
    }
    
    // Letter C (blue) - arc shape
    startX = _width / 2 + 100;
    int centerY = startY + letterSize / 2;
    int radius = letterSize / 2;
    for (int angle = 45; angle < 315; angle++) {
        double rad = angle * 3.14159 / 180.0;
        int x = startX + radius + (int)(cos(rad) * radius);
        int y = centerY + (int)(sin(rad) * radius);
        if (x >= 0 && x < _width && y >= 0 && y < _height) {
            int idx = (y * _width + x) * 4;
            _pixelData[idx + 0] = 0; _pixelData[idx + 1] = 128; _pixelData[idx + 2] = 255; _pixelData[idx + 3] = 255;
        }
    }
    
    // Add animated element - a moving red dot
    _frameCount++;
    int dotX = (_width / 4) + (int)(sin(_frameCount * 0.05) * 100);
    int dotY = _height - 50;
    for (int dy = -5; dy <= 5; dy++) {
        for (int dx = -5; dx <= 5; dx++) {
            int x = dotX + dx;
            int y = dotY + dy;
            if (x >= 0 && x < _width && y >= 0 && y < _height && dx*dx + dy*dy <= 25) {
                int idx = (y * _width + x) * 4;
                _pixelData[idx + 0] = 255; _pixelData[idx + 1] = 0; _pixelData[idx + 2] = 0; _pixelData[idx + 3] = 255;
            }
        }
    }
}

void OsgRenderer::initScene() {
    // Skip OSG initialization for now
}

osg::Node* OsgRenderer::createTextGeometry() {
    return nullptr;
}

void OsgRenderer::render() {
    // Use software rendering for testing
    softwareRenderABC();
}

int OsgRenderer::getTextureId() const {
    return 0; 
}

void OsgRenderer::resize(int width, int height) {
    _width = width;
    _height = height;
    if (_pixelData) {
        delete[] _pixelData;
    }
    _pixelData = new unsigned char[_width * _height * 4];
    memset(_pixelData, 0, _width * _height * 4);
}

void OsgRenderer::readPixels(void* buffer) {
    if (_pixelData && buffer) {
        memcpy(buffer, _pixelData, _width * _height * 4);
    }
}
