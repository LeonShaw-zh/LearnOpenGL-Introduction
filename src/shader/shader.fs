#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main(){
   FragColor = texture(Texture1, TexCoord) * vec4(vertexColor, 1.0);
}