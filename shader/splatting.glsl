
uniform sampler2D tex0;	//alpha
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

void main(void) {

	vec4 alpha=texture2D(tex0,gl_TexCoord[0].xy);
	vec2 coord=gl_TexCoord[0].xy*100;
	vec4 c=mix(texture2D(tex1,coord),texture2D(tex2,coord),alpha.r);
	c=mix(c,texture2D(tex3,coord),alpha.g);
	c=mix(c,texture2D(tex4,coord),alpha.b);

	gl_FragColor=vec4(c.rgb,1);
}
