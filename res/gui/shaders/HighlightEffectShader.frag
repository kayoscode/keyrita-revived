#version 300 es
precision mediump float;

uniform sampler2D Texture;
uniform vec2 MousePos;
uniform vec2 WindowDims;

const vec4 HighlightColor = vec4(1.0, 1.0, 0.8, 1);
const float Sigma = 0.035;
const float Intensity = 0.15;

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

void main()
{
	float aspectRatio = WindowDims.x / WindowDims.y;
	vec2 newMousePos = vec2(MousePos.x, -MousePos.y);
	newMousePos /= WindowDims;
	newMousePos.y += 1.0;
	vec2 relativePos = (gl_FragCoord.xy / WindowDims) - newMousePos;
	relativePos.x *= aspectRatio;

	float distanceToMouse = distance(vec2(0.0, 0.0), relativePos);
	float gaussian = exp(-distanceToMouse * distanceToMouse / (2.0 * Sigma * Sigma));

	Out_Color = Frag_Color * texture(Texture, Frag_UV.xy);
	Out_Color.rgb = mix(Out_Color.rgb, HighlightColor.rgb, gaussian * Intensity);
}
