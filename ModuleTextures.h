#pragma once
#include "Application.h"
#include "Module.h"
#include "Globals.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class ModuleTextures : public Module {

public:
	ModuleTextures();
	~ModuleTextures();

	bool Init();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	bool CleanUp();
	GLuint texture1, texture2, texture3;
	ILubyte *data = nullptr;
	ILuint kirbo, lenna, muffin;

	int width;
	int height;

	bool MINfilter = false;
	bool MAGfilter = true;

	bool WRAPs = true;
	bool WRAPt = false;

	//Which image to show
	int imageButtonValue = 0;
	bool once = true; //so it doesn't execude code all the time

	//Allows to draw the image
	bool mipmap = true;

private:


};