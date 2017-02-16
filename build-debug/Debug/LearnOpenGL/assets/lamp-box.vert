
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 transform;

void main()
{
    // you can either use one transform or use the projection/view/model
    // transforms since uninitialized uniforms are set to zero, 
    // the other terms will disappear
	gl_Position = 
        transform * vec4(position, 1.0f) + 
        projection * view * model * vec4(position, 1.0f);
	
	ourColor = color;
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	// TexCoord = texCoord;
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}


// NDC = Normalized Device Coordinates
// range from -1 to 1 and is the space that gets displayed on-screen

// Model Matrix
// takes your object from the "local space" to the "world space"
// translate/scales/rotates the object to place it in the world

// View Matrix
// moves the objects from the coordinates of the "world space"
// to be in the "view space" of a camera

// Projection Matrix
// takes the objects from the 3D "view space" and brings them to the NDC of
// the 2D "clip Space". Objects outside of the range of the clip space are
// thrown away.
