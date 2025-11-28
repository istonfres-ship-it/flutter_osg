#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void* createOsgRenderer(int width, int height);
void destroyOsgRenderer(void* renderer);
void renderOsgFrame(void* renderer);
int getOsgTextureId(void* renderer);
void readOsgPixels(void* renderer, void* buffer);
void resizeOsgRenderer(void* renderer, int width, int height);
int getOsgRendererWidth(void* renderer);
int getOsgRendererHeight(void* renderer);

// Input events
void osgMouseMove(void* renderer, int x, int y);
void osgMouseButton(void* renderer, int button, int pressed, int x, int y);
void osgKeyEvent(void* renderer, int keyCode, int pressed);
void osgScroll(void* renderer, double deltaX, double deltaY);

#ifdef __cplusplus
}
#endif
