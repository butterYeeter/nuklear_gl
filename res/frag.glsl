#version 330 core
in vec2 uv;

out vec4 FragColor;

uniform float time;
uniform sampler2D tex0;

void main() {
    // float alpah = sin(time);
    // FragColor = vec4(texture(tex0, uv).xyz, alpah);
    vec4 col = texture(tex0, uv) * sin(48*time);

    FragColor = col;
    // FragColor = vec4(1.0);
}
