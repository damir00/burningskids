
uniform sampler2D texColor;
in float sub2;

void main() {
	float c=texture2D(texColor,gl_TexCoord[0].xy).r;
	gl_FragColor=vec4(0,c-sub2,0,0);
}

