uniform mat4 model_matrix;
uniform mat4 view_matrix;

void main(void) {
	//gl_Position=gl_ProjectionMatrix*screen_mat*gl_Vertex;
	//gl_Position=gl_ProjectionMatrix*gl_ModelViewMatrix*gl_Vertex;

	gl_Position=view_matrix*model_matrix*gl_Vertex;
	gl_TexCoord[0]  = gl_MultiTexCoord0;

//	gl_Position=ftransform();
}
