#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    // A [-1, 1] tartományú koordinátákat közvetlenül továbbítjuk a fragment shadernek
    gl_Position = vec4(aPos, 0.0, 1.0);
}