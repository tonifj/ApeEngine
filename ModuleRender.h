#pragma once
#include "Module.h"
#include "Globals.h"

#include "MathGeoLib/include/MathGeoLib.h"

#include <gl/glew.h>
#pragma comment (lib, "GLFW/lib-vc2017/glfw3.lib")

#include "IMGUI/imgui.h"


struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void* context;

	ImVec4 clear_color;

	GLuint VBO, VAO, EBO;
	//GLuint texture;
	math::float3  up, target;
	math::float3 cameraPos, camDirection, camUp, camRight;
	math::float4x4 view, proj, model, transform;

private:
	
	
};
