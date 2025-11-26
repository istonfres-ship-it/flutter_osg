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

}
