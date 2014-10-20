
uniform mat4 world_mat;
uniform mat4 screen_mat;
uniform vec4 origin;

void main(void) {
	vec4 pos=world_mat*gl_Vertex;
	vec4 offset=pos-origin;
	gl_Position=gl_ProjectionMatrix*screen_mat*(pos+(offset*( gl_VertexID%2 )*20000));
}
