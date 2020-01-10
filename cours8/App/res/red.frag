#version 120
uniform sampler2D texture;

void main() {
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel_color = texture2D(texture, coord);
	pixel_color.g = pixel_color.b = 0.0f;
	gl_FragColor = pixel_color * gl_Color;
}
