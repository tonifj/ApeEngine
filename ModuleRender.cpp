﻿#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleTextures.h"
#include "ModuleUI.h"
#include "SDL.h"

#include <GL/glew.h>

//Always after glew.h
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

using namespace std;

//yolo
ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{

}

//yeys
void ModuleRender::lookAt() {

	f = float3(target - cameraPos);
	f.Normalize();
	s = float3(f.Cross(up));
	s.Normalize();
	u = float3(s.Cross(f));

	//View Matrix - Look at computation
	view[0][0] = s.x;
	view[0][1] = s.y;
	view[0][2] = s.z;

	view[1][0] = u.x;
	view[1][1] = u.y;
	view[1][2] = u.z;

	view[2][0] = -f.x;
	view[2][1] = -f.y;
	view[2][2] = -f.z;

	view[0][3] = -s.Dot(cameraPos);
	view[1][3] = -u.Dot(cameraPos);
	view[2][3] = f.Dot(cameraPos);

	view[3][0] = 0.0f;
	view[3][1] = 0.0f;
	view[3][2] = 0.0f;
	view[3][3] = 1.0f;
}


// Called before render is available
bool ModuleRender::Init()
{

	LOG("Creating Renderer context");
	App->ui->my_log.AddLog("Creating Renderer context\n");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GLContext glcontext = SDL_GL_CreateContext(App->window->window);
	SDL_GL_MakeCurrent(App->window->window, glcontext);
	//glClearColor(0.225f, 0, 0.225f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(App->window->window);

	GLenum err = glewInit();// ... check for errors

	App->ui->my_log.AddLog("Using Glew %s \n", glewGetString(GLEW_VERSION));
	App->ui->my_log.AddLog("Vendor: %s \n", glGetString(GL_VENDOR));
	App->ui->my_log.AddLog("Renderer: %s \n", glGetString(GL_RENDERER));
	App->ui->my_log.AddLog("OpenGL version supported %s \n", glGetString(GL_VERSION));
	App->ui->my_log.AddLog("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	
	float vertices[] = {
		// positions         
		-1.0f,  -1.0f, 0.0f, //bottom left
		1.0f, -1.0f, 0.0f,   //bottom right
		1.0f, 1.0f, 0.0f,    //top right
		-1.0f, 1.0f, 0.0f,   //top left


		//Texture coords
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	

	//Creates a new vbo, vao & ebo
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//Setting buffer to be used & assign data to it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Position coords (0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Texture coords attribute (1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 4 * 3));
	glEnableVertexAttribArray(1);
	
	//Passing transform matrix to the shader from the ModuleProgram.cpp

	
	return true;
}

update_status ModuleRender::PreUpdate()
{
	glClearColor(0.225f, 0, 0.225f, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAO);

	if (!mode) {
		//We use draw elements to indicate OpenGL to draw by the indexs stored in the EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	}

	else
		glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glBindVertexArray(0);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{



	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{



	//SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Destroy window

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
}

