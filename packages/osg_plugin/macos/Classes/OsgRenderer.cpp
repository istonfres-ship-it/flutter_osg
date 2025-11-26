#include "OsgRenderer.h"
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osg/GraphicsContext> // Add this

// Callback to rotate the ball
class RollCallback : public osg::NodeCallback {
public:
    RollCallback(double speed) : _speed(speed), _angle(0.0) {}
    
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
        osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(node);
        if (pat) {
            _angle += _speed;
            // Simple rotation around X axis to simulate rolling along Y or Z
            pat->setAttitude(osg::Quat(_angle, osg::Vec3(1.0, 0.0, 0.0)));
        }
        traverse(node, nv);
    }
private:
    double _speed;
    double _angle;
};

OsgRenderer::OsgRenderer(int width, int height) : _width(width), _height(height) {
    initScene();
}

OsgRenderer::~OsgRenderer() {
    _viewer = nullptr;
}

void OsgRenderer::initScene() {
    _root = new osg::Group;

    // Create 3 balls
    _root->addChild(createRollingBall(osg::Vec3(-2.0, 0.0, 0.0), osg::Vec4(1.0, 0.0, 0.0, 1.0))); // Red
    _root->addChild(createRollingBall(osg::Vec3(0.0, 0.0, 0.0), osg::Vec4(0.0, 1.0, 0.0, 1.0)));  // Green
    _root->addChild(createRollingBall(osg::Vec3(2.0, 0.0, 0.0), osg::Vec4(0.0, 0.0, 1.0, 1.0)));  // Blue

    // Setup Texture for Off-screen rendering
    _texture = new osg::Texture2D;
    _texture->setTextureSize(_width, _height);
    _texture->setInternalFormat(GL_RGBA);
    _texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    _texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

    _image = new osg::Image;
    _image->allocateImage(_width, _height, 1, GL_RGBA, GL_UNSIGNED_BYTE);
    _texture->setImage(0, _image.get());

    // Setup Viewer
    _viewer = new osgViewer::Viewer;
    _viewer->setSceneData(_root);
    
    // Setup Camera for RTT (Render To Texture)
    osg::Camera* camera = _viewer->getCamera();

    // Create a graphics context (pbuffer)
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = _width;
    traits->height = _height;
    traits->red = 8;
    traits->green = 8;
    traits->blue = 8;
    traits->alpha = 8;
    traits->depth = 24;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->pbuffer = true; // Try pbuffer

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (!gc) {
        printf("OSG: Failed to create pbuffer context. Trying window.\n");
        traits->pbuffer = false;
        gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    }
    
    if (gc) {
        printf("OSG: GraphicsContext created.\n");
        camera->setGraphicsContext(gc.get()); 
    } else {
        printf("OSG: Failed to create GraphicsContext.\n");
    }

    camera->setViewport(0, 0, _width, _height);
    camera->setClearColor(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->attach(osg::Camera::COLOR_BUFFER, _texture.get());
    camera->attach(osg::Camera::COLOR_BUFFER, _image.get()); // Attach image to read back
    
    _viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    _viewer->realize();
}

osg::Node* OsgRenderer::createRollingBall(const osg::Vec3& position, const osg::Vec4& color) {
    osg::Geode* geode = new osg::Geode;
    osg::ShapeDrawable* sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0,0,0), 0.8f));
    sphere->setColor(color);
    geode->addDrawable(sphere);

    osg::PositionAttitudeTransform* pat = new osg::PositionAttitudeTransform;
    pat->setPosition(position);
    pat->addChild(geode);
    pat->setUpdateCallback(new RollCallback(0.05)); // Add rolling animation

    return pat;
}

void OsgRenderer::render() {
    if (_viewer.valid()) {
        // printf("OSG: Rendering frame...\n");
        _viewer->frame();
    }
}

int OsgRenderer::getTextureId() const {
    // This returns the OpenGL texture ID.
    // Note: This ID is only valid in the context where OSG created it.
    // To share with Flutter, you need context sharing or texture sharing extensions.
    if (_texture.valid()) {
         // We use context ID 0 as a default for single-context apps
         osg::Texture::TextureObject* to = _texture->getTextureObject(0);
         if (to) return to->id();
    }
    return 0; 
}

void OsgRenderer::resize(int width, int height) {
    _width = width;
    _height = height;
    if (_viewer.valid()) {
        _viewer->getCamera()->setViewport(0, 0, _width, _height);
        _texture->setTextureSize(_width, _height);
    }
}

void OsgRenderer::readPixels(void* buffer) {
    if (_image.valid() && _image->data()) {
        // Copy data from OSG image to the buffer
        // Assuming buffer is large enough: width * height * 4
        memcpy(buffer, _image->data(), _width * _height * 4);
    }
}
