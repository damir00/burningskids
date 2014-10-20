uniform sampler2D tex0;
uniform vec4 color;
uniform float border_width;
uniform vec4 border_color;

void main() {

//	gl_FragColor=vec4(1,0,0,1);
//	gl_FragColor=vec4(gl_TexCoord[0].x,gl_TexCoord[0].y,1,1);


	float a=texture2D(tex0,gl_TexCoord[0].xy).a;
	if(a<0.5) discard;

	//if(a<0.55) gl_FragColor=vec4(1,0,0,1);
	//gl_FragColor=mix(vec4(1,0,0,1),color,(a-0.5)*2);
	gl_FragColor=color;
}


