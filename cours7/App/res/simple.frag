#version 120
uniform sampler2D texture;
uniform float time;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel_color = texture2D(texture, coord);
	gl_FragColor = pixel_color * gl_Color;
}
