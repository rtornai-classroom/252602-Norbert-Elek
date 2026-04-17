#version 330 core
out vec4 outColor;
in vec3 FragColor;

void main() {
    outColor = vec4(FragColor, 1.0);
}