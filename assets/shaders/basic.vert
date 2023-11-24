#version 430

// x, y, z
layout(location = 0) in vec3 position;
// x, y, z
layout(location = 1) in vec3 normal;
// u, v
layout(location = 2) in vec2 texCoord;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

out vec2 TexCoord;

// TODO#2-1: Render texture (vertex & fragment shader)
//           Implement basic.vert and basic.frag to render texture color
// Note:
//       1. How to write a vertex shader:
//          a. The output is gl_Position and anything you want to pass to the fragment shader.
//          b. You may need to pass texture coordinate for this vertex to fragment shader
//       2. How to write a fragment shader:
//          a. The output is FragColor
//          b. You may pass texture to shader with uniform sampler2D sampler
//          c. You may sample color for this vertex with texture() function
void main() {
    gl_Position = Projection * ViewMatrix * ModelMatrix * vec4(position, 1.0);
    TexCoord = texCoord;
}
