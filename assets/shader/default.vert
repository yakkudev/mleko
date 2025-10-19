#version 330 core
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec2 _uv;
layout (location = 3) in vec4 _color;

uniform float uTime;

out vec2 _texCoord;

void main() {
    gl_Position = vec4(_pos.x, _pos.y + (sin(uTime * 2.0) + 1.0) * 0.25 , _pos.z, 1.0);

    _texCoord = _uv;
}

