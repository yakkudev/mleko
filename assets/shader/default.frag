#version 330 core

in vec2 _texCoord;

out vec4 _fragColor;

uniform sampler2D uTexture;

void main() {
    _fragColor = texture(uTexture, _texCoord);
    // _fragColor = vec4(_texCoord, 0.0, 0.0);
}

