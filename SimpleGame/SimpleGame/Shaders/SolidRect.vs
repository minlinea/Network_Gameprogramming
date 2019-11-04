#version 330

in vec3 a_Position;

uniform vec4 u_Trans;

uniform mat4 u_ProjView;

void main()
{
	vec4 newPosition;
	newPosition.xy = a_Position.xy*u_Trans.w + u_Trans.xy;
	newPosition.z = 0.0;
	newPosition.w= 1.0;
	gl_Position = u_ProjView * newPosition;
}
