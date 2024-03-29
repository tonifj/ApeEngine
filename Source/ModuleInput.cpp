#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleUI.h"
#include "ModelLoader.h"


#include "IMGUI/imgui.h"

#include"IMGUI/imgui_impl_opengl3.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_sdl.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#define ORBIT_RADIUS 10


ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{

	App->ui->my_log.AddLog("Init SDL input event system\n");

	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

void ModuleInput::ResetAngles() {
	pitch = 0;
	yaw = -90;
}

void ModuleInput::FreeMovement(float&x, float&y, float& _x, float& _y) {
	xOffset = x - _x;
	yOffset = _y - y;

	_x = x;
	_y = y;

	xOffset *= App->camera->sensitivity;
	yOffset *= App->camera->sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	App->camera->frustum.front.x = cos(DegToRad(yaw))* cos(DegToRad(pitch));
	App->camera->frustum.front.y = sin(DegToRad(pitch));
	App->camera->frustum.front.z = sin(DegToRad(yaw)) * cos(DegToRad(pitch));
	App->camera->frustum.front.Normalize();
	App->camera->dirty = true;
}

void ModuleInput::Orbit(float& x, float& y, float& _x, float& _y) {
	yOffset = x - _x;
	xOffset = y - _y;

	_x = x;
	_y = y;

	xOffset *= App->camera->sensitivity;
	yOffset *= App->camera->sensitivity;


	//Updting camera Position
	if (yOffset > 1.5 || yOffset < -1.5) {

		theta += yOffset;
		phi += xOffset;

		App->camera->frustum.pos.x = App->modelLoader->model.getCenter().x + ORBIT_RADIUS * sin(DegToRad(theta));
		App->camera->frustum.pos.z = App->modelLoader->model.getCenter().z + ORBIT_RADIUS * cos(DegToRad(theta));

		App->camera->LookAt(App->camera->frustum.pos, App->modelLoader->model.getCenter(), float3(0, 1, 0));
	}

	if (xOffset > 1.5 || xOffset < -1.5) {

		theta += yOffset;
		phi += xOffset;

		App->camera->frustum.pos.y = App->modelLoader->model.getCenter().y + ORBIT_RADIUS * cos(DegToRad(phi));
		App->camera->frustum.pos.z = App->modelLoader->model.getCenter().z + ORBIT_RADIUS * sin(DegToRad(phi));

		App->camera->LookAt(App->camera->frustum.pos, App->modelLoader->model.getCenter(), float3(0, 1, 0));
	}
}

void ModuleInput::ProcessMovement(const float3& direction) {
	//if (speed_boost)
		App->camera->camSpeed = App->camera->speedValue * direction;

	//else if(!speed_boost)
		//App->camera->camSpeed = App->camera->speedValue * direction;
}

update_status ModuleInput::Update()
{

	static SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_QUIT:
			return UPDATE_STOP;
			break;

		case SDL_WINDOWEVENT:

			if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				App->renderer->WindowResized(e.window.data1, e.window.data2);

				float asp = RadToDeg(App->window->GetWidth() / App->window->GetHeight());
				App->camera->SetAspectRatio(asp);

				App->camera->dirty = true;
			}

			break;

		case SDL_MOUSEBUTTONDOWN:

			if (e.button.button == SDL_BUTTON_RIGHT) {

				//If the camera mode is not Orbit
				if (App->camera->mode)
					enable_camera_movement = true;

				//Reset the condition so offsets are reseted once
				once = false;
			}

			break;
		case SDL_MOUSEBUTTONUP:

			if (e.button.button == SDL_BUTTON_RIGHT) {

				enable_camera_movement = false;

				xOffset = 0;
				yOffset = 0;

				lastX = 0;
				lastY = 0;

				oldP = 0;
				oldT = 0;
			}

			break;

		case SDL_MOUSEMOTION:
			if (enable_camera_movement) {

				//FREE MOVEMENT
				if (!altPressed) {

					currentX = (float)e.motion.x;
					currentY = (float)e.motion.y;

					if (!once) {
						lastX = currentX;
						lastY = currentY;
						once = true;
					}

					FreeMovement(currentX, currentY, lastX, lastY);

				}

				//Orbit
				if (altPressed) {

					currT = (float)e.motion.x;
					currP = (float)e.motion.y;

					if (!once) {
						oldT = currT;
						oldP = currP;
						once = true;
					}

					Orbit(currT, currP, oldT, oldP);

				}

			}
				//Reset useful variables from the two movement methods
			else {

				oldT = 0;
				oldP = 0;

				lastX = 0;
				lastY = 0;
			}

			break;

		case SDL_DROPFILE:

			directory = e.drop.file;

			extension = directory.substr(directory.size() - 4, directory.size());

			if (extension == ".fbx" || extension == ".obj") {

				App->ui->my_log.AddLog("[DROP] Model loded from: ");
				App->ui->my_log.AddLog(directory.c_str());
				App->ui->my_log.AddLog("\n");

				std::string modelTexturePath = directory.substr(0, directory.find_last_of('\\'));
				std::string folderName = directory.substr(directory.find_last_of('\\')+1);
				folderName = folderName.substr(0, folderName.find_last_of('.'));

				std::string textureName;

				if (directory.substr(directory.size() - 5, directory.size()) == "e.fbx") {
					textureName = folderName + ".png";
					App->ui->my_log.AddLog(modelTexturePath.c_str());
					App->ui->my_log.AddLog("\n");
				}
				if (directory.substr(directory.size() - 5, directory.size()) == "o.fbx") {
					textureName = folderName + ".dds";
					App->ui->my_log.AddLog(modelTexturePath.c_str());
					App->ui->my_log.AddLog("\n");
				}

				if (directory.substr(directory.size() - 5, directory.size()) == "i.fbx") {
					textureName = folderName + ".dds";
					App->ui->my_log.AddLog(modelTexturePath.c_str());
					App->ui->my_log.AddLog("\n");
				}

				

				App->modelLoader->textureDir = modelTexturePath + '\\' +  textureName;
				App->modelLoader->modelDir = directory.c_str();
				App->modelLoader->hasChanged = true;

				App->camera->ResetCamera(false);
				
				float3 offset = App->modelLoader->model.getCenter() - App->camera->frustum.pos;
				App->camera->frustum.pos -= offset / App->modelLoader->model.GetHeight();
				App->camera->dirty = true;

			}

			else if (extension == ".png" || extension == ".jpg" || extension == ".dds") {

				App->ui->my_log.AddLog("[DROP] Image dropped from: ");
				App->ui->my_log.AddLog(directory.c_str());
				App->ui->my_log.AddLog("\n");

				if (extension == ".png")
					App->textures->CreateTexture(IL_PNG, directory.c_str());

				if (extension == ".jpg")
					App->textures->CreateTexture(IL_JPG, directory.c_str());

				if (extension == ".dds")
					App->textures->CreateTexture(IL_DDS, directory.c_str());

				App->modelLoader->previousTexture = directory.c_str();
				App->modelLoader->textureDir = directory.c_str();

				App->modelLoader->hasChanged = true;
		
			}

			else {
				App->ui->my_log.AddLog("[DROP] The object dropped has to be a .fbx, .obj, .png or .jpg\n");
			}

			break;

		case SDL_KEYDOWN:

			if (e.key.keysym.scancode == SDL_SCANCODE_LSHIFT) {
				App->camera->speedValue = 0.3;
			}

			else if (e.key.keysym.scancode == SDL_SCANCODE_LALT)
				altPressed = true;

			else if (e.key.keysym.scancode == SDL_SCANCODE_F) {


				float angle = App->camera->frustum.front.AngleBetween(App->modelLoader->model.getCenter() - App->camera->frustum.pos);
				pitch = 0;
				yaw = -90;
				//App->camera->LookAt(App->camera->frustum.pos, App->modelLoader->model.getCenter(), float3(0, 1, 0));
				App->camera->ResetCamera(false);
				
				//App->ui->my_log.AddLog("%0.1f\n", RadToDeg(angle));


				//Pitch and yaw angles needs to be recalculated. 
				//Procediment. 
				//1. Calculate angle between frustum.front, and direction from frustum.pos to model.pos
				//2. Extract x component and y component ( sin & cos )
				//3. angle * sin = new angle to apply to the yaw
				//4. angle*cos = new angle to apply to pitch

				//float _yaw = angle * sin(angle);
				//float _pitch = angle * cos(angle);

				//yaw = _yaw;
				//pitch = _pitch;

				App->camera->dirty = true;


			}

			if (enable_camera_movement) {
				if (e.key.keysym.scancode == SDL_SCANCODE_W) {
					ProcessMovement(App->camera->frustum.front);
				}

				else if (e.key.keysym.scancode == SDL_SCANCODE_S) {
					ProcessMovement(-App->camera->frustum.front);
				}

				else if (e.key.keysym.scancode == SDL_SCANCODE_D) {
					ProcessMovement(App->camera->camRight);
					
				}

				else if (e.key.keysym.scancode == SDL_SCANCODE_A) {
					ProcessMovement(-App->camera->camRight);

				}

				else if (e.key.keysym.scancode == SDL_SCANCODE_Q) {
					ProcessMovement(App->camera->frustum.up);

				}

				else if (e.key.keysym.scancode == SDL_SCANCODE_E) {
					ProcessMovement(-App->camera->frustum.up);
				}
			}

			break;

		case SDL_KEYUP:
			//if (e.key.keysym.scancode == SDL_SCANCODE_F)
				//App->camera->dirty = true;

			if (e.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
				App->camera->speedValue = 0.1;

			if (e.key.keysym.scancode == SDL_SCANCODE_LALT)
				altPressed = false;

			if (e.key.keysym.scancode == SDL_SCANCODE_W || e.key.keysym.scancode == SDL_SCANCODE_S ||
				e.key.keysym.scancode == SDL_SCANCODE_D || e.key.keysym.scancode == SDL_SCANCODE_A ||
				e.key.keysym.scancode == SDL_SCANCODE_Q || e.key.keysym.scancode == SDL_SCANCODE_E) {
				App->camera->camSpeed = float3::zero;
				App->camera->dirty = true;
			}



			break;

		case SDL_MOUSEWHEEL:
			if (e.wheel.y > 0) // scroll up
			{
				App->camera->frustum.pos +=  App->camera->frustum.front;
				App->camera->dirty = true;
			}
			if (e.wheel.y < 0) // scroll down
			{
				App->camera->frustum.pos +=  -App->camera->frustum.front;
				App->camera->dirty = true;
			}
			
				
			break;
		}


	}


	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
