#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 uv;

// uniform ivec2 res;

void main(){
    // vec2 resf = vec2(res);
    // float x = aPos.x * resf.x / resf.y;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    uv = aUV;
}
