#include <chrono>
#include <iostream>
#include <stdlib.h>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
 
using namespace cv;

#include "UI.h"

using namespace reza::ui;
using namespace ci;
using namespace ci::app;
using namespace std::chrono;
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
    void setupGL_cube();
    
    void setupMesh();


    void draw_triangle();
    void draw_rectangle(float time_seconds);
    void draw_cubes(float time_seconds);
    void draw_lamp();
    void draw_cube(gl::GlslProgRef shader, float time_seconds, glm::vec3 position,float scale_factor);
    void draw_mesh();
    
    void keyDown( KeyEvent key_event) override;
    
    void triangles2dToVerticesAndIndices(
        const vector<Vec6f> triangle_list,
        vector<Vec2f>& vertex_list,
        vector<GLint>& index_list);
    void Vec2fToVec3fWithRandomValues(
        const vector<Vec2f>& vertex_list_2d,
        vector<Vec3f>& vertex_list_3d);

    SuperCanvasRef mUi;
    float mRed = 0.0;
    float mGreen = 0.0;
    float mBlue = 0.0;
    
    
	gl::GlslProgRef m_Glsl_flat_orange;
	gl::GlslProgRef m_Glsl_textured_with_color;
    gl::GlslProgRef m_Glsl_lamp_box;
    
    gl::TextureRef m_wooden_crate_texture;

    vector<GLfloat> m_triangle_vertices;
    gl::VaoRef m_triangle_VAO;
    gl::VboRef m_triangle_VBO;
    
    vector<GLfloat> m_rectangle_vertices;
    vector<GLuint> m_rectangle_indices;
    gl::VaoRef m_rectangle_VAO;
    gl::VboRef m_rectangle_VBO;
    gl::VboRef m_rectangle_EBO;
    
    vector<GLfloat> m_cube_vertices;
    vector<glm::vec3> m_cube_positions;
    gl::VaoRef m_cube_VAO;
    gl::VboRef m_cube_VBO;

    vector<GLfloat> m_mesh_vertices;
    gl::VaoRef m_mesh_VAO;
    gl::VboRef m_mesh_VBO;
    gl::VboRef m_mesh_EBO;
    int m_mesh_number_of_indeces;

    steady_clock::time_point m_start_time;
    
    CameraPersp m_camera;
    CameraUi m_mouse_camera;
};

void OglTest::setup()
{
    setWindowSize(1000,1000);
    m_camera = CameraPersp(1000,1000,45.0f);
    m_camera.setPerspective(45.0f,1.0f,0.1f,1000.0f);
        //projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
    m_camera.setEyePoint(glm::vec3(0.0f, 0.0f, 3.0f));
    m_camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
    m_mouse_camera.setCamera(&m_camera);
    m_mouse_camera.connect(getWindow());
    
    setupMesh();
    cout << "Frame rate" << getFrameRate() << endl;
    setupUI();
    setupGL();
    m_start_time = steady_clock::now();
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
    setupGL_cube();
}
// GLSL example
void OglTest::setupGL_shaders()
{
    // this wil compile the shaders and assemble the shader program
    {m_Glsl_flat_orange = 
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
        )));}   
    // a shader that takes in texture coordinates and colors
    m_Glsl_textured_with_color =
        gl::GlslProg::create(
            loadAsset( "texture.vert" ),
            loadAsset( "texture.frag" ) );
        
    m_Glsl_lamp_box =
        gl::GlslProg::create(
            loadAsset( "lamp-box.vert" ),
            loadAsset( "lamp-box.frag" ) );    
}
// Initialize values for the vertices
void OglTest::setupGL_vertices()
{
    m_wooden_crate_texture =
        gl::Texture::create(
            loadImage(
                loadAsset( "container2.png" ) ) );

    m_triangle_vertices = {
        -0.2f, -0.2f, 0.0f, // Left  
         0.2f, -0.2f, 0.0f, // Right 
         0.0f,  0.2f, 0.0f  // Top
    };

    m_rectangle_vertices = {
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom Right
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom Left
       -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top Left
    };
    m_rectangle_indices = {
        0, 1, 3, // top triangle half
        1, 2, 3  // bottom triangle half
    };
    
    m_cube_vertices = {
        // Positions           // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
        // World space positions of our cubes
     m_cube_positions = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
}
// 1: VAO example
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
    {
        m_triangle_VBO->bind();
        
        // *** Vertex Attributes
        // *** Tell the vertex shader how to interpret the data in the VBOs
        // *** to put it in to the vertex inputs
        gl::vertexAttribPointer(
            0, // which attribute we are configuring (from layout = ?)
            3, // size of the vertex attribute (vec3 has 3 values)
            GL_FLOAT, // the type of the values
            GL_FALSE, // normalize data or not
            3 * sizeof(GLfloat), // space btwn vertex attribute sets
            (const GLvoid*) 0 ); // data start offset
        
        gl::enableVertexAttribArray(0);
    }
    // Finished configuring the VOA
    m_triangle_VAO->unbind();
}
// 2: EBO + texture example
void OglTest::setupGL_rectangle()
{
    m_rectangle_VAO = gl::Vao::create();     
    m_rectangle_VBO = 
        gl::Vbo::create(
            GL_ARRAY_BUFFER,
            m_rectangle_vertices.size()*sizeof(GLfloat),   // buffer size
            m_rectangle_vertices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
// *** EBO
// *** Element Buffer Object - holds the indeces of the verts in the VBO
    m_rectangle_EBO = 
        gl::Vbo::create(
            GL_ELEMENT_ARRAY_BUFFER, // EBO! this time
            m_rectangle_indices.size()*sizeof(GLuint),   // buffer size
            m_rectangle_indices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
  
    m_rectangle_VAO->bind();
    {
        m_rectangle_VBO->bind();
        m_rectangle_EBO->bind();
        
        // *** Vertex Attributes
        // *** Tell the vertex shader how to interpret the data in the VBOs
        // *** to put it in to the vertex inputs

        // NOTE How it's unrelated to the EBO! B/c it's defining 
        // colors and textures for the verteces
        
        // Position attribute 
        // (second input to vertex shader, ie. 0)
        gl::vertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        gl::enableVertexAttribArray(0);
        
        // Color attribute 
        // (second input, 1)
        gl::vertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        gl::enableVertexAttribArray(1);
        
        // TexCoord attribute 
        // (third input, 2)
        gl::vertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        gl::enableVertexAttribArray(2);
    }
    m_rectangle_VAO->unbind();
    
}
// 2: EBO + texture example
void OglTest::setupGL_cube()
{
    m_cube_VAO = gl::Vao::create();     
    m_cube_VBO = 
        gl::Vbo::create(
            GL_ARRAY_BUFFER,
            m_cube_vertices.size()*sizeof(GLfloat),   // buffer size
            m_cube_vertices.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:
  
    m_cube_VAO->bind();
    {
        m_cube_VBO->bind();
        // Position attribute 
        gl::vertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        gl::enableVertexAttribArray(0);
        
        // TexCoord attribute 
        gl::vertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        gl::enableVertexAttribArray(2);
    }
    m_cube_VAO->unbind();
    
}
void OglTest::setupMesh()
{
    Subdiv2D opencv_triangle_generator = Subdiv2D(Rect(0,0,1.0f,1.0f));
    // insert random points
    //srand(1);
    srand(duration_cast<duration<float>>(steady_clock::now() - m_start_time).count());
    for(int i = 0; i !=50; ++i)
    {
        opencv_triangle_generator.insert(
            Point2f(
                static_cast<float>( rand() ) / static_cast<float>( RAND_MAX ),
                static_cast<float>( rand() ) / static_cast<float>( RAND_MAX )));
    }
    vector<Vec6f> triangle_list;
    opencv_triangle_generator.getTriangleList(triangle_list);
    cout << "Number of dirty triangles: " << triangle_list.size() << endl;
    
    // due to some OpenCV bug there are 3 extra vertices.
    // Their associated triangles need to be removed
    vector<Vec6f> clean_triangles(triangle_list.size());
    copy_if(
        triangle_list.begin(),
        triangle_list.end(),
        clean_triangles.begin(),
            [](Vec6f triangle)
            { return (triangle[0] != 3 &&
                triangle[1] != 3 &&
                triangle[2] != 3 &&
                triangle[3] != 3 &&
                triangle[4] != 3 &&
                triangle[5] != 3 &&
                triangle[0] != -3 &&
                triangle[1] != -3 &&
                triangle[2] != -3 &&
                triangle[3] != -3 &&
                triangle[4] != -3 &&
                triangle[5] != -3 );
        } );
    
    // 2D triangles to EBO
    // ...
    vector<Vec2f> vertex_list_2d;
    vector<GLint> index_list;
    triangles2dToVerticesAndIndices(
        clean_triangles,
        vertex_list_2d,
        index_list);
    
    vector<Vec3f> vertex_list_3d;
    Vec2fToVec3fWithRandomValues(
        vertex_list_2d,
        vertex_list_3d);
    
    // due to Vec2f and Vec3f's data types, the numbers are sequential
    // need to put into temp buffers
    vector<GLfloat> vertex_buffer;
    for(auto vertex : vertex_list_3d)
    {
        vertex_buffer.push_back(vertex[0]);
        vertex_buffer.push_back(vertex[1]);
        vertex_buffer.push_back(vertex[2]);
    }
    m_mesh_number_of_indeces = index_list.size(); // needed for drawing
    
    m_mesh_VAO = gl::Vao::create();     
    m_mesh_VBO = 
        gl::Vbo::create(
            GL_ARRAY_BUFFER,
            vertex_buffer.size()*sizeof(GLfloat),   // buffer size
            vertex_buffer.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:

    m_mesh_EBO = 
        gl::Vbo::create(
            GL_ELEMENT_ARRAY_BUFFER, // EBO! this time
            index_list.size()*sizeof(GLuint),   // buffer size
            index_list.data(),           // buffer data
            GL_STATIC_DRAW); // specifies the nature of the memory:

    m_mesh_VAO->bind();
    {
        m_mesh_VBO->bind();
        m_mesh_EBO->bind();
        // Position attribute 
        gl::vertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        gl::enableVertexAttribArray(0);
        
//         // TexCoord attribute  GARBAGE (but so we can see the random triangles!)
        gl::vertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        gl::enableVertexAttribArray(2);
    }
    m_mesh_VAO->unbind();
 
}
void OglTest::update()
{
}

void OglTest::draw_triangle()
{
    // find where the color value is
    m_Glsl_flat_orange->uniform(
        "ourColor",
        vec4(mRed,mGreen,mBlue,1.0f)); 
    
    
    m_Glsl_flat_orange->bind();
    m_triangle_VAO->bind();
    {
        gl::drawArrays(
            GL_TRIANGLES,
            0,
            3);
    }
    m_triangle_VAO->unbind();

}
void OglTest::draw_rectangle(float time_seconds)
{
    auto radians = time_seconds * 3.0f * float(M_PI) / 180.0f;
    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, radians, glm::vec3(0.0, 0.0, 1.0)); 
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    m_Glsl_textured_with_color->uniform("transform",trans);
    
    m_Glsl_textured_with_color->bind();
    m_wooden_crate_texture->bind();
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
void OglTest::draw_cubes(float time_seconds)
{
    for(auto position:m_cube_positions)
    {
        draw_cube(m_Glsl_textured_with_color, time_seconds,position,1.0f);
    }
    //draw_cube(time_seconds,glm::vec3(0.0f, 0.0f, -3.0f));
}
void OglTest::draw_lamp()
{
    draw_cube(m_Glsl_lamp_box,0.0f,vec3(0.1f,0.1f,0.1f),0.3f);
}
void OglTest::draw_cube(gl::GlslProgRef shader, float time_seconds, glm::vec3 position,float scale_factor)
{
    auto radians = time_seconds * 100.0f*mRed * float(M_PI) / 180.0f;
    
    shader->bind();
    
    glm::mat4 model;
    model = glm::translate(model, position);
    model = glm::rotate(model, radians, position);//glm::vec3(0.5f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale_factor));
    shader->uniform("model",model);
    
    glm::mat4 view;
    view = m_camera.getViewMatrix();
    shader->uniform("view",view);
    
    glm::mat4 projection;
    projection = m_camera.getProjectionMatrix();
    shader->uniform("projection",projection);    

    m_wooden_crate_texture->bind();
    
    m_cube_VAO->bind();
    {
        gl::drawArrays(GL_TRIANGLES,0,36);
    }
    m_cube_VAO->unbind();
}
void OglTest::draw_mesh()
{
    auto shader = m_Glsl_textured_with_color;
    
    shader->bind();
    
    glm::mat4 model;
     model = glm::translate(model, glm::vec3(-0.5f, -0.5f, -0.5f));
//     model = glm::rotate(model, radians, position);//glm::vec3(0.5f, 1.0f, 0.0f));
//     model = glm::scale(model, glm::vec3(scale_factor));
    shader->uniform("model",model);
    
    glm::mat4 view;
    view = m_camera.getViewMatrix();
    shader->uniform("view",view);
    
    glm::mat4 projection;
    projection = m_camera.getProjectionMatrix();
    shader->uniform("projection",projection);    

    m_wooden_crate_texture->bind();
    
    m_mesh_VAO->bind();
    {
        gl::drawElements(
            GL_TRIANGLES,
            m_mesh_number_of_indeces,
            GL_UNSIGNED_INT,
            0);
    }
    m_mesh_VAO->unbind();
}
void OglTest::draw()
{
	gl::clear( Color( 0.2, 0.3, 0.4 ) );
    gl::clear(GL_COLOR_BUFFER_BIT);
    
    // enable the Z-buffer
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    steady_clock::time_point now = steady_clock::now();
    duration<float> time_span = duration_cast<duration<float>>(now - m_start_time);
   
    // needs a separate shader... 
    // clashes with the cube one
    // draw_rectangle(time_span.count()); 
    
    //draw_triangle();
//     draw_cubes(time_span.count());
//     draw_lamp();
    draw_mesh();
}

void OglTest::cleanup()
{
    mUi->save( getSaveLoadPath() );
}

void OglTest::keyDown( KeyEvent key_event)
{
    if( key_event.getCode() == KeyEvent::KEY_c)
    {
        if(m_mouse_camera.isEnabled())
            m_mouse_camera.disable();
        else
            m_mouse_camera.enable();
    }
    
    if( key_event.getCode() == KeyEvent::KEY_r)
    {
        setupMesh();
    }
}
fs::path OglTest::getSaveLoadPath()
{
    fs::path path = getAssetPath( "" );
    path += "/" + mUi->getName() + ".json";
    return path;
}

void OglTest::triangles2dToVerticesAndIndices(
    const vector<Vec6f> triangle_list,
    vector<Vec2f>& vertex_list,
    vector<GLint>& index_list)
{
    for(auto triangle: triangle_list)
        for(auto vertex_number = 0; vertex_number <3; ++vertex_number) // for the 3 vertices
        {
            const auto x_coordinate = triangle[2 * vertex_number];
            const auto y_coordinate = triangle[2 * vertex_number + 1];
            const auto vertex = Vec2f(x_coordinate, y_coordinate);
            const auto vertex_location =
                find(vertex_list.begin(),vertex_list.end(),vertex);
            if(vertex_location != vertex_list.end())
            {
                const auto vertex_location_pointer = vertex_location.base();
                const auto vertex_index = vertex_location_pointer - vertex_list.data();
                index_list.push_back(vertex_index);
            }
            else
            {
                index_list.push_back(vertex_list.size());
                vertex_list.push_back(vertex); // increments the size
                // but since the indeces start from zero, 
                // the previous size is correct
            }
        }
}

void OglTest::Vec2fToVec3fWithRandomValues(
    const vector<Vec2f>& vertex_list_2d,
    vector<Vec3f>& vertex_list_3d)
{
    for(auto vertex :vertex_list_2d)
    {
        vertex_list_3d.push_back(
            Vec3f(
                vertex[0],
                vertex[1],
                0.1f*static_cast<float>( rand() ) / static_cast<float>( RAND_MAX )));
    }
}
CINDER_APP( OglTest, RendererGl )
