#version 300 es

uniform mat4 ProjectionMatrix;

in vec2 Position;
in vec2 TexCoord;
in vec4 Color;

out vec2 Frag_UV;
out vec4 Frag_Color;

void main() 
{
	Frag_UV = TexCoord;
	Frag_Color = Color;
	gl_Position = ProjectionMatrix * vec4(Position.xy, 0, 1);
}
