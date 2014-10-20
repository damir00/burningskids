uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform vec2 offset;
out float sub2;

void main(void) {
//	gl_Position=view_matrix*model_matrix*(gl_Vertex + vec4(offset.x,offset.y,0,0) );
	gl_Position=view_matrix*model_matrix*(gl_Vertex + vec4(offset.x,offset.y,0,0)*gl_InstanceID);
	gl_TexCoord[0]  = gl_MultiTexCoord0;
	sub2=gl_InstanceID/50.0;
}

