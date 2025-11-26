#pragma once

#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/MatrixTransform>

class OsgRenderer {
public:
    OsgRenderer(int width, int height);
    ~OsgRenderer();

    void render();
    int getTextureId() const;
    void resize(int width, int height);
    void readPixels(void* buffer);

private:
    void initScene();
    void softwareRenderABC();
    osg::Node* createTextGeometry();

    osg::ref_ptr<osgViewer::Viewer> _viewer;
    osg::ref_ptr<osg::Group> _root;
    osg::ref_ptr<osg::GraphicsContext> _gc;
    unsigned char* _pixelData;
    int _width;
    int _height;
    int _frameCount;
};
