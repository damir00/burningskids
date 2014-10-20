uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float time;
void main() {
//	vec2 coord=gl_TexCoord[0].xy+vec2(0,sin(time/10000+gl_TexCoord[0].x*sin(time/10000)*10  )/4);
//	coord.y=clamp(coord.y*2-0.5,0,1);
//	gl_FragColor=texture2D(tex0, coord);

	vec2 coord=gl_TexCoord[0].xy;
	vec4 o=texture2D(tex0, coord);
	o*= sin(time/10000 + coord.x*2)/2+0.5;
	gl_FragColor=o + texture2D(tex1,coord);
}

