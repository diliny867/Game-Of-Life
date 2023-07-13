#version 330 core
out vec4 fragColor;

uniform float cellSize = 10.0;
uniform vec2 canvasOffset = vec2(0.0);
uniform float scale = 1.0;
uniform const vec3 lineColor = vec3(0.0,0.0,0.0);
uniform const float lineThickness = 1.0;

void main(){
    vec2 transformedCoord = (gl_FragCoord.xy - vec2(canvasOffset.x,-canvasOffset.y)) / scale;

    bool isOutline = (mod(transformedCoord.x,cellSize) < lineThickness ||
        mod(transformedCoord.y,cellSize) < lineThickness);

    if(isOutline) {
        fragColor = vec4(lineColor,1.0);
    } else {
        discard;
    }
}