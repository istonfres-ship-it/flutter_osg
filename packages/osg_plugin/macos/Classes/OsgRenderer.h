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
    void readPixels(void* buffer); // Add this

private:
    void initScene();
    osg::Node* createRollingBall(const osg::Vec3& position, const osg::Vec4& color);

    osg::ref_ptr<osgViewer::Viewer> _viewer;
    osg::ref_ptr<osg::Texture2D> _texture;
    osg::ref_ptr<osg::Image> _image;
    osg::ref_ptr<osg::Group> _root;
    int _width;
    int _height;
};
