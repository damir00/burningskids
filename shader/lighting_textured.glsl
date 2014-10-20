
uniform sampler2D texColor;
uniform sampler2D texNormal;
uniform sampler2D texLight;
uniform vec3 pos;
uniform vec3 dir;
uniform vec2 screenSizeInv;
uniform vec2 origin;

void main(void) {
	vec4 light=texture2D(texLight,gl_TexCoord[0].xy);
//	if(light.a<=0) discard;

	vec2 frag_coord=gl_FragCoord.xy*screenSizeInv;

	vec3 color=texture2D(texColor,frag_coord).rgb;
	vec4 normal=texture2D(texNormal,frag_coord).rgba;

	float shadow=normal.a;

	normal=normal*2-1.0;

	vec3 sun_dir=normalize(vec3(gl_TexCoord[0].xy-origin,0.2));	//0.2 = light height

	normal=vec4(
		normal.x*dir.x - normal.y*dir.y, 
		normal.x*dir.y + normal.y*dir.x,
		normal.z,
		1);

	float val=dot(normal,sun_dir)*(shadow);

	gl_FragColor=vec4((color*light.rgb)*val,light.a*val);
	//gl_FragColor=vec4(vec3(shadow),1);
}


