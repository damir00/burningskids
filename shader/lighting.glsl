
uniform sampler2D texColor;	//alpha has dynamic lights intensity
uniform sampler2D texNormal;
uniform sampler2D texHeight;
uniform vec2 screenSizeInv;

uniform vec4 ambient;
uniform vec4 sun_dir;
uniform vec4 sun_color;
uniform vec2 clouds_offset;
uniform vec2 clouds_scale;
uniform float shadow_quality;

void main(void) {

	vec2 frag_coord=gl_FragCoord.xy*screenSizeInv;

	vec4 color=texture2D(texColor,frag_coord);

	vec3 height_shadow=texture2D(texHeight,frag_coord*shadow_quality).rgb;
	//r=height
	//g=shadow
	//b=clouds

	vec3 normal=texture2D(texNormal,frag_coord).rgb;

	float clouds=height_shadow.b;

	normal.xy=normal.xy*2-1.0;
	float val=dot(normal,sun_dir.xyz);

	float mul;
	mul=mix(1,0,(height_shadow.g-height_shadow.r)*5);

	mul=clamp(mul,0,1);
	if(height_shadow.g>height_shadow.r) val*=mul;

//	val*=clouds;

	gl_FragColor=color* mix(ambient,sun_color,val);
//	gl_FragColor=vec4(vec3(color.a),1);
}


