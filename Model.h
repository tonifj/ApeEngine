#ifndef __MODEL__
#define __MODEL__


#include "Shader.h"
#include "Mesh.h"
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <vector>

class Model
{
public:
	/*  Functions   */
	Model();
	Model(const char *path)
	{
		loadModel(path);
	}
	void Draw(Shader shader);
	std::string path;
	const aiScene* scene;
private:
	/*  Model Data  */
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	/*  Functions   */
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
	unsigned int TextureFromFile(const char *path, const std::string &directory);
};

#endif