#include "OsgBridge.h"
#include "OsgRenderer.h"

extern "C" {

void* createOsgRenderer(int width, int height) {
    return new OsgRenderer(width, height);
}

void destroyOsgRenderer(void* renderer) {
    delete static_cast<OsgRenderer*>(renderer);
}

void renderOsgFrame(void* renderer) {
    static_cast<OsgRenderer*>(renderer)->render();
}

int getOsgTextureId(void* renderer) {
    return static_cast<OsgRenderer*>(renderer)->getTextureId();
}

void readOsgPixels(void* renderer, void* buffer) {
    static_cast<OsgRenderer*>(renderer)->readPixels(buffer);
}

void resizeOsgRenderer(void* renderer, int width, int height) {
    static_cast<OsgRenderer*>(renderer)->resize(width, height);
}

int getOsgRendererWidth(void* renderer) {
    return static_cast<OsgRenderer*>(renderer)->getWidth();
}

int getOsgRendererHeight(void* renderer) {
    return static_cast<OsgRenderer*>(renderer)->getHeight();
}

void osgMouseMove(void* renderer, int x, int y) {
    static_cast<OsgRenderer*>(renderer)->onMouseMove(x, y);
}

void osgMouseButton(void* renderer, int button, int pressed, int x, int y) {
    static_cast<OsgRenderer*>(renderer)->onMouseButton(button, pressed != 0, x, y);
}

void osgKeyEvent(void* renderer, int keyCode, int pressed) {
    static_cast<OsgRenderer*>(renderer)->onKeyEvent(keyCode, pressed != 0);
}

void osgScroll(void* renderer, double deltaX, double deltaY) {
    static_cast<OsgRenderer*>(renderer)->onScroll(deltaX, deltaY);
}

}
