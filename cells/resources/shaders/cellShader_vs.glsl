#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in ivec2 aOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    vec2 pos = aPos+vec2(aOffset);
    gl_Position = projection * view * model * vec4(pos, 0.0, 1.0);
}