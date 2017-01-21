#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"

using namespace ci;
using namespace ci::app;
class BasicApp : public App {
  public:
	void	setup() override;
	void	draw() override;

	CameraPersp         mCam;
	gl::BatchRef        mCube;
	gl::GlslProgRef		mGlsl;
};

void BasicApp::setup()
{
	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	mGlsl = 
        gl::GlslProg::create( 
            gl::GlslProg::Format()
                // the vertex shader:
                // sets attributes for the fragment shader
                // set positons from Object space to Clip space
                .vertex(	
                    CI_GLSL( // MACRO for inline GLSL code
                        150, // GLSL version number
                        // uniform: vars passed from C++
                        uniform mat4	ciModelViewProjection;
                        // in: per-vertex data supplied by draw()
                        // so the vertex data of geom::Cube
                        in vec4			ciPosition;
                        void main( void ) {
                            // built in var:
                            // specifies the position in Clip space
                            gl_Position	=
                                // vanilla transform 
                                // from Object to Clip space
                                ciModelViewProjection * ciPosition;
                        }))
                // the fragment shader:
                // sets the final pixel value
                .fragment(
                    CI_GLSL(
                        150,
                        out vec4		oColor; // output color val
                        void main( void ) {
                            // just set to a constant value
                            oColor = vec4( 1, 0.5, 0.25, 1 );
                        }
        )));

    // same as before in OpenGL-Tutorial...
	mCube = gl::Batch::create( geom::Cube(), mGlsl );
	
	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void BasicApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );
	gl::setMatrices( mCam );
	mCube->draw();
}
CINDER_APP( BasicApp, RendererGl )
