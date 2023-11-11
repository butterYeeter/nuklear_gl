#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

out vec2 UV;
out vec4 Color;


void main(){
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    UV = aUV;
    Color = aColor;
}
