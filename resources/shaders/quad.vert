#version 430 core

// structs
struct Transform {
  ivec2 atlasOffset;
  ivec2 spriteSize;
  vec2 position;
  vec2 size;
};

// uniforms
uniform vec2 screenSize;
uniform mat4 projection;

// input
layout (std430, binding = 0) buffer TransformBuffer {
    // Holds an array of transforms, determined by cpu
  Transform transforms[];
};

// Output
layout (location = 0) out vec2 textureCoordsOut;

void main() {
    // gl_instanceID is the index of the instance in the buffer
    Transform transform = transforms[gl_InstanceID];
    vec2 verticies[6] = {
        transform.position,                                     //  Top left
        vec2(transform.position + vec2(0.0, transform.size.y)), // Bot left
        vec2(transform.position + vec2(transform.size.x, 0.0)), // Top right
        vec2(transform.position + vec2(transform.size.x, 0.0)), // Top right
        vec2(transform.position + vec2(0.0, transform.size.y)), // Bot left
        transform.position + transform.size                     // bot right
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bot = transform.atlasOffset.y + transform.spriteSize.y;
    vec2 textureCoords[6] = {
        vec2(left, top),
        vec2(left, bot), 
        vec2(right, top), 
        vec2(right, top), 
        vec2(left, bot), 
        vec2(right, bot) 
    };
    // Normalize the verticies
    vec2 vertPos = verticies[gl_VertexID];
    // vertPos.y = -vertPos.y + screenSize.y;
    // vertPos = 2.0 * (vertPos / screenSize) - 1.0;
    gl_Position = projection * vec4(vertPos, 0.0, 1.0);
    
    textureCoordsOut = textureCoords[gl_VertexID];
}