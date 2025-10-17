#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec4 aColor;


void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
