#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void* createOsgRenderer(int width, int height);
void destroyOsgRenderer(void* renderer);
void renderOsgFrame(void* renderer);
int getOsgTextureId(void* renderer);
void readOsgPixels(void* renderer, void* buffer); // Add this

#ifdef __cplusplus
}
#endif
