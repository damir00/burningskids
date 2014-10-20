
uniform sampler2D texNormal;
uniform vec2 dir;

void main(void) {

	vec4 normal=texture2D(texNormal,gl_TexCoord[0].xy);
	//if(normal.a<=0) discard;

	//normal=normal*4-1.0;

	//[0 - 1] -> [-1 - 1]
	normal=normal*2-1;

	normal=vec4(
		-normal.g*dir.x - normal.r*dir.y,
		-normal.g*dir.y + normal.r*dir.x,
		normal.b,normal.a);

	//normal=normal/2+0.5;
	//[-1 - 1] -> [0 - 1]
	normal=normal+1*0.5;

	gl_FragColor=normal;
}


