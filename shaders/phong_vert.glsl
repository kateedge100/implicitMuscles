#version 150                                          // Keeping you on the bleeding edge!
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack: enable


// The modelview and projection matrices are no longer given in OpenGL 4.2
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 N; // This is the inverse transpose of the MV matrix
uniform vec3 Color;
// The vertex position attribute
layout (location = 0) in vec3 VertexPosition;

// The texture coordinate attribute
layout (location = 1) in vec2 TexCoord;

// The vertex normal attribute
layout (location = 2) in vec3 VertexNormal;

// The time in the scene
layout (location = 3) in float Time;

//layout ( location = 4 ) in vec3 Color;

out vec3 FragmentPosition;
out vec3 FragmentNormal;
out vec2 texCoord;
out float time;
out vec3 color;

/************************************************************************************/
void main()
{
    // Set the position of the current vertex
		gl_Position = MVP * vec4(VertexPosition, 1.0);
		FragmentPosition = vec3(MV * vec4(VertexPosition, 1.0));
                FragmentNormal = N * VertexNormal;
		texCoord = TexCoord;
                time = Time;
                color = Color;
}

