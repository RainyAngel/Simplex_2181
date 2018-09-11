#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool Complimentary = false;

out vec4 Fragment;

void main()
{
	Fragment = vec4(Color,1);
	
	if(Complimentary == false)
		Fragment = vec4(SolidColor, 1);
	else if(Complimentary)
		

	return;
}