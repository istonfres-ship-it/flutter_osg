#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define OSG_BRIDGE_API __declspec(dllexport)
#else
#define OSG_BRIDGE_API
#endif

OSG_BRIDGE_API void* createOsgRenderer(int width, int height);
OSG_BRIDGE_API void destroyOsgRenderer(void* renderer);
OSG_BRIDGE_API void renderOsgFrame(void* renderer);
OSG_BRIDGE_API int getOsgTextureId(void* renderer);
OSG_BRIDGE_API void readOsgPixels(void* renderer, void* buffer);
OSG_BRIDGE_API void resizeOsgRenderer(void* renderer, int width, int height);
OSG_BRIDGE_API int getOsgRendererWidth(void* renderer);
OSG_BRIDGE_API int getOsgRendererHeight(void* renderer);

// Input events
OSG_BRIDGE_API void osgMouseMove(void* renderer, int x, int y);
OSG_BRIDGE_API void osgMouseButton(void* renderer, int button, int pressed, int x, int y);
OSG_BRIDGE_API void osgKeyEvent(void* renderer, int keyCode, int pressed);
OSG_BRIDGE_API void osgScroll(void* renderer, double deltaX, double deltaY);

#ifdef __cplusplus
}
#endif
