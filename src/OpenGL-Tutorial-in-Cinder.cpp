
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "UI.h"

using namespace reza::ui;
using namespace ci;
using namespace ci::app;
using namespace std;

class OglTest : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
    void cleanup() override;
    fs::path getSaveLoadPath();
    
    void setupUI();
    
    void setupGL();
    void setupGL_shaders();
    void setupGL_vertices();
    void setupGL_triangle();
    void setupGL_rectangle();
    
    SuperCanvasRef mUi;
    float mRed = 0.0;
    float mGreen = 0.0;
    float mBlue = 0.0;
    
    
	gl::GlslProgRef m_Glsl_flat_orange;
	gl::GlslProgRef m_Glsl_textured_with_color;
    
    vector<GLfloat> m_triangle_vertices;
    gl::VaoRef m_triangle_VAO;
    gl::VboRef m_triangle_VBO;
    
    vector<GLfloat> m_rectangle_vertices;
    vector<GLuint> m_rectangle_indices;
    gl::TextureRef m_rectangle_texture;
    gl::VaoRef m_rectangle_VAO;
    gl::VboRef m_rectangle_VBO;
    gl::VboRef m_rectangle_EBO;
};

void OglTest::setup()
{
    setupUI();
    setupGL();
}
void OglTest::setupUI()
{
    mUi = SuperCanvas::create( "Color Control" );
    mUi->addSpacer();
    mUi->addLabel( 
        "WE CAN DO BETTER THAN PARAMS! ^_^",
        FontSize::SMALL );
    mUi->addLabel( 
        "DOUBLE CLICK BASIC TO MINIMIZE",
        FontSize::SMALL );
    mUi->addLabel( 
        "CLICK BASIC AND DRAG TO MOVE",
        FontSize::SMALL );
    mUi->addSpacer();

    mUi->addLabel( 
        "BASIC SLIDER:",
        FontSize::SMALL );
    mUi->addSliderf( "RED", &mRed );

    mUi->addSpacer();
    mUi->addLabel( 
        "SLIDER W/ OPTION: CROSSFADER",
        FontSize::SMALL );
    mUi->addSliderf( 
        "GREEN", 
        &mGreen, 
        0.0,
        1.0,
        Sliderf::Format().crossFader() );

    mUi->addSpacer();
    mUi->addLabel( 
        "SLIDER W/ OPTION: NO LABEL",
        FontSize::SMALL );
    mUi->addSliderf( 
        "BLUE",
        &mBlue,
        0.0,
        1.0,
        Sliderf::Format().label( false ) );

    mUi->addMultiSlider( 
        "BG COLOR", 
            {
            MultiSlider::Data( "RED", &mRed ),
            MultiSlider::Data( "GREEN", &mGreen ),
            MultiSlider::Data( "BLUE", &mBlue )
            });
    mUi->autoSizeToFitSubviews();
    
    mUi->load( getSaveLoadPath() );
}
void OglTest::setupGL()
{
    setupGL_shaders();
    setupGL_vertices();
    setupGL_triangle();
    setupGL_rectangle();
}
// GLSL example
void OglTest::setupGL_shaders()
{
    // this wil compile the shaders and assemble the shader program
	m_Glsl_flat_orange = 
        gl::GlslProg::create( 
            gl::GlslProg::Format()
                .vertex(	
                    CI_GLSL( // MACRO for inline GLSL code
                        330, // GLSL version number
                        // uniform: vars passed from C++
                        layout (location = 0) in vec3 position;
                        // in: per-vertex data supplied by draw()
                        // so the vertex data of geom::Cube
                        void main() {
                            gl_Position = 
                                vec4(
                                    position.x,
                                    position.y,
                                    position.z,
                                    1.0);
                        }))
                // the fragment shader:
                // sets the final pixel value
                .fragment(
                    CI_GLSL(
                        330,
                        out vec4 color; // output color val
                        uniform vec4 ourColor;
                        void main( void ) {
                            // just set to a constant value
                            color = ourColor;
                        }
        )));
        
    // a shader that takes in texture coordinates and colors
    m_Glsl_textured_with_color =
        gl::GlslProg::create( 
            gl::GlslProg::Format()
                .vertex(	
                    CI_GLSL( // MACRO for inline GLSL code
                        330, // GLSL version number
                        // uniform: vars passed from C++
                        layout (location = 0) in vec3 position;
                        // in: per-vertex data supplied by draw()
                        // so the vertex data of geom::Cube
                        void main() {
                            gl_Position = 
                                vec4(
                                    position.x,
                                    position.y,
                                    position.z,
                                    1.0);
                        }))
                // the fragment shader:
                // sets the final pixel value
                .fragment(
                    CI_GLSL(
                        330,
                        out vec4 color; // output color val
                        uniform vec4 ourColor;
                        void main( void ) {
                            // just set to a constant value
                            color = ourColor;
                        }
        )));
}
// Initialize values for the vertices
void OglTest::setupGL_vertices()
{
    m_triangle_vertices = {
        -0.2f, -0.2f, 0.0f, // Left  
         0.2f, -0.2f, 0.0f, // Right 
         0.0f,  0.2f, 0.0f  // Top
    };
    m_rectangle_texture = 
        gl::Texture::create(
            loadImage(
                loadAsset( "container.jpg" ) ) );


    m_rectangle_vertices = {
        // Positions        // Colors          // Texture Coordinates
        0.5f,  0.5f, 0.0f, // Top Right
        0.5f, -0.5f, 0.0f, // Bottom Right
       -0.5f, -0.5f, 0.0f, // Bottom Left
       -0.5f,  0.5f, 0.0f  // Top Left
       /*
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
       -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top Left
    */};
    m_rectangle_indices = {
        0, 1, 3, // top triangle half
        1, 2, 3  // bottom triangle half
    };
}
// VAO example
void OglTest::setupGL_triangle()
{
// *** VAO
// *** Stores Data (VBOs) + Vertex Attribute associations
    m_triangle_VAO = gl::Vao::create();     
    
// *** VBO    
// *** Allocating space for the vertices on the GPU:
    
    // Cinder temporarily binds the buffer to GL_ARRAY_BUFFER
    // (using a scoped bind), and allocated space for it on the GPU
    // see: Vbo.cpp and BufferObj.cpp
    m_triangle_VBO = 
        gl::Vbo::create(
            GL_ARRAY_BUFFER, // the buffer type 
            // we can bind to different buffers 
            // as long as they're different types
            // ie. this is the "buffer target"
            m_triangle_vertices.size()*sizeof(GLfloat),   // buffer size
            m_triangle_vertices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
    // GL_STATIC_DRAW: 
    //         the data will most likely not change at all or very rarely.
    // GL_DYNAMIC_DRAW: 
    //     the data is likely to change a lot.
    // GL_STREAM_DRAW: 
    //     the data will change every time it is drawn.

    // equivalent to :
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferData(
    //         GL_ARRAY_BUFFER,
    //         sizeof(vertices),
    //         vertices,
    //         GL_STATIC_DRAW);
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
  
    m_triangle_VAO->bind();
    m_triangle_VBO->bind();
    
// *** Vertex Attributes
// *** Tell the vertex shader how to interpret the data in the VBOs
    gl::vertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (const GLvoid*) 0 );
    
    gl::enableVertexAttribArray(0);

    // Finished configuring the VOA
    m_triangle_VAO->unbind();
    
}
// EBO example
void OglTest::setupGL_rectangle()
{
// *** VAO
// *** Stores Data (VBOs) + Vertex Attribute associations
    m_rectangle_VAO = gl::Vao::create();     
    
// *** VBO    
// *** Allocating space for the vertices on the GPU:
    
    // Cinder temporarily binds the buffer to GL_ARRAY_BUFFER
    // (using a scoped bind), and allocated space for it on the GPU
    // see: Vbo.cpp and BufferObj.cpp
    m_rectangle_VBO = 
        gl::Vbo::create(
            GL_ARRAY_BUFFER, // the buffer type 
            // we can bind to different buffers 
            // as long as they're different types
            // ie. this is the "buffer target"
            m_rectangle_vertices.size()*sizeof(GLfloat),   // buffer size
            m_rectangle_vertices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
    // GL_STATIC_DRAW: 
    //         the data will most likely not change at all or very rarely.
    // GL_DYNAMIC_DRAW: 
    //     the data is likely to change a lot.
    // GL_STREAM_DRAW: 
    //     the data will change every time it is drawn.

    // equivalent to :
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferData(
    //         GL_ARRAY_BUFFER,
    //         sizeof(vertices),
    //         vertices,
    //         GL_STATIC_DRAW);
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
        
// *** EBO
// *** Element Buffer Object - holds the indeces of the verts in the VBO
    m_rectangle_EBO = 
        gl::Vbo::create(
            GL_ELEMENT_ARRAY_BUFFER, // the buffer type 
            // we can bind to different buffers 
            // as long as they're different types
            // ie. this is the "buffer target"
            m_rectangle_indices.size()*sizeof(GLuint),   // buffer size
            m_rectangle_indices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
  
    m_rectangle_VAO->bind();
    m_rectangle_VBO->bind();
    m_rectangle_EBO->bind();
    
// *** Vertex Attributes
// *** Tell the vertex shader how to interpret the data in the VBOs
    gl::vertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (const GLvoid*) 0 );
    
    gl::enableVertexAttribArray(0);

    // Finished configuring the VOA
    m_rectangle_VAO->unbind();
    
}
void OglTest::update()
{
}

void OglTest::draw()
{
	gl::clear( Color( 0.2, 0.3, 0.4 ) );
   
    // find where the color value is
    m_Glsl_textured_with_color->uniform(
        "ourColor",
        vec4(mRed,mGreen,mBlue,1.0f)); 
    m_Glsl_textured_with_color->bind();
    {
        gl::polygonMode(GL_FRONT_AND_BACK, GL_LINE);
        m_rectangle_VAO->bind();
        {
            gl::drawElements(
                GL_TRIANGLES,
                6,
                GL_UNSIGNED_INT,
                0);
        }
        m_rectangle_VAO->unbind();
    }
    // find where the color value is
    m_Glsl_flat_orange->uniform(
        "ourColor",
        vec4(mRed,mGreen,mBlue,1.0f)); 
    m_Glsl_flat_orange->bind();
    {
        gl::polygonMode(GL_FRONT_AND_BACK, GL_FILL);
        m_triangle_VAO->bind();
        {
            gl::drawArrays(
                GL_TRIANGLES,
                0,
                3);
        }
        m_triangle_VAO->unbind();
    }
}

void OglTest::cleanup()
{
    mUi->save( getSaveLoadPath() );
}

fs::path OglTest::getSaveLoadPath()
{
    fs::path path = getAssetPath( "" );
    path += "/" + mUi->getName() + ".json";
    return path;
}

CINDER_APP( OglTest, RendererGl )

