#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"

using namespace ci;
using namespace ci::app;

class BasicApp : public App {
  public:
    // *override* explcitely overrides a virtual function
    // so if you mess up the interface and it doesn't overload
    // properly, you'll get a compilererror
	void setup() override;
	void draw() override;
	
	static const int NUM_SLICES = 12;
	
	gl::BatchRef	mSlices[NUM_SLICES];
	CameraPersp		mCam; // set just once during setup()
};

void BasicApp::setup()
{
   // get a stock shader
	auto lambert = gl::ShaderDef().lambert().color();
	gl::GlslProgRef	shader = gl::getStockShader( lambert );
    // a GLSL Prog == shader
    
    
	for( int i = 0; i < NUM_SLICES; ++i ) {
		float rel = i / (float)NUM_SLICES;
		float sliceHeight = 1.0f / NUM_SLICES;
        
        // create a geometry - all children of geom::Source
		auto slice = geom::Cube().size( 1, sliceHeight, 1 );
        // creating two 'geom::Modifier's
		auto trans = geom::Translate( 0, rel, 0 );
		auto color = geom::Constant( geom::COLOR,
								Color( CM_HSV, rel, 1, 1 ) );
		
        // a gl::Batch
        // pairs a geometry (and chained modifiers) with a shader
        mSlices[i] = 
            gl::Batch::create( 
                slice 
                    // shift the slice vertically
                    >> trans
                    // modify the geom::Attrib::COLOR
                    // use by the shader
                    // (color gets baked in and can't be changed)
                    >> color,
				shader );
	}
    // Cinder has 3 setMatrices
    // Model matrix: where to place the model
    // View matrix: virtual eye position and direction
    // Projection matrix: View matrix + other parameters
    
    
    // default - set to match that of app::Window 	
	mCam.lookAt( vec3( 2, 3, 2 ), vec3( 0, 0.5f, 0 ) );
    // now set as a camera looking in some direction
}


void BasicApp::draw()
{
    // CONSTANTS - (maybe move out of this scope?)
	const float 
        delay = 0.25f;
	// time in seconds for one slice to rotate
	const float 
        rotationTime = 1.5f;
	// time in seconds to delay each slice's rotation
	const float 
        rotationOffset = 0.1f; // seconds
	// total time for entire animation
	const float 
        totalTime = 
            delay + rotationTime + NUM_SLICES * rotationOffset;

	gl::clear(); // clears the screen?

    // z-buffer -
    // makes pixels remember what depth their at 
    // (so you don't draw over stuff)
	gl::enableDepthRead(); 
	gl::enableDepthWrite();// similar?

	gl::setMatrices( mCam ); // sets the View/Projection matrices
    // To look at the values: - gl::getModelMatrix(), gl::getViewMatrix(), and gl::getProjectionMatrix()
    // ex:
    // console() << gl::getProjectionMatrix() << std::endl;

	// loop every 'totalTime' seconds
	float time = fmod( getElapsedFrames() / 30.0f, totalTime );

	for( int i = 0; i < NUM_SLICES; ++i ) {
		// animates from 0->1
		float 
            rotation = 0;
		// when does the slice begin rotating
		float 
            startTime = i * rotationOffset;
		// when does it complete
		float 
            endTime = startTime + rotationTime;

        // are we in the middle of our time section?
		if( time > startTime && time < endTime )
			rotation = ( time - startTime ) / rotationTime;

        // ease fn on rotation, then convert to radians
        // uses #include "cinder/Easing.h"
        // "remap the domain 0-1 into the range 0-1 nonlinearly"
		float 
            angle = easeInOutQuint( rotation ) * M_PI / 2.0f;

        // gl::Scoped* class
            // saves the OpenGL state,
            // and reverts it back on destruction
		gl::ScopedModelMatrix scpModelMtx;
        // changes the OpenGL state
		gl::rotate( 
            angleAxis( angle, vec3( 0, 1, 0 ) ) // a quaternion
        );
		// does the draw
        mSlices[i]->draw();
        
        // end of the loop, so the gl::ScopedModelMatrix
        // will go out of scope and restore the Model Matrix state
        // to whatever it was before
	}
}
CINDER_APP( BasicApp, RendererGl )

