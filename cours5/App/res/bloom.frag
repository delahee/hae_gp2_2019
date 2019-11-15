#version 120
uniform sampler2D	texture;
uniform float		bloomPass;
uniform vec3		bloomMul;

vec4 bloom(vec4 color){
	float 	alpha = color.a;
		
	vec4	lumVector(0.299f, 0.587f, 0.114f);
	float	luminance = dot(iLuminanceVector, color.rgb);
		
	luminance = max(0.0, luminance - bloomPass);
		
	color.rgb *= sign(luminance);
	color *= bloomMul;
	return color;
}

void main() {
    vec2 coord = gl_TexCoord[0].xy;
	vec4 pixel_color = bloom( texture2D(texture, coord) * gl_Color );
	gl_FragColor = pixel_color;
}
