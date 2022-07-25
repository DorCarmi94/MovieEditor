#version 330

in vec2 texCoord0;
in vec3 normal0;
in vec3 color0;
in vec3 position0;

uniform float transperancy;
uniform vec4 lightColor;
uniform sampler2D sampler1;


out vec4 Color;
void main()
{
	// Fog parameters
	float fog_maxdist = 50.0;
	float fog_mindist = 10.0;
	vec4  fog_colour = vec4(0.4, 0.4, 0.4, 1.0);

	// // Calculate fog
	float dist = length(position0.xyz);
	float fog_factor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
	fog_factor = clamp(fog_factor, 0.0, 1.0);

	Color = texture2D(sampler1, texCoord0)* vec4(color0, transperancy) * lightColor; //you must have gl_FragColor
	Color = mix(fog_colour, Color, fog_factor);
}
