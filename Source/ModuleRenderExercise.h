#pragma once
#include "Module.h"
#include "Globals.h"


class ModuleRenderExercise : public Module
{
public:

	ModuleRenderExercise();
	~ModuleRenderExercise();

	bool Init();
	update_status Update();
	bool CleanUp();

	float* buffer_data = nullptr;
//	GLuint vbo;
	//GLuint vbo2;
	unsigned int VAO;

	//math::float3  up, target;
	//math::float3 cameraPos, camDirection, camUp, camRight;
	//math::float4x4 view, proj, model, transform;

};