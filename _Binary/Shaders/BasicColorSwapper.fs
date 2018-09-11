#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool Complimentary = false;

out vec4 Fragment;

void main()
{
	//for creating the color shader
	vec3 colorController = Color;
	
	//regular rgb
	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0){
		colorController = SolidColor;
	}

	//change to complimentary
	if(Complimentary){
		colorController = vec3(1.0f, 1.0f, 1.0f) - colorController;
	}

	//for returning the color fragment in a way the graphics card can understand
	Fragment = vec4(colorController, 1);

	return;
}