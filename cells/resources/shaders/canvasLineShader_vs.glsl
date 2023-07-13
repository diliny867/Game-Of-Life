#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float cellSize = 10.0;
uniform ivec2 lineInstanceOffsetSide = ivec2(0);

void main(){
    vec2 pos = aPos+lineInstanceOffsetSide*gl_InstanceID*cellSize;
    gl_Position = projection * view * model * vec4(pos,0.0,1.0);
}