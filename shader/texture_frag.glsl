uniform sampler2D tex0;
uniform vec4 color;

void main(void) {
	gl_FragColor=texture2D(tex0,gl_TexCoord[0].xy)*color;
}
