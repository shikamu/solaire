#pragma once 
#include <string>
#include <vector>

class Shader;
using std::string;
using std::vector; 

class SceneNodeParams
{
public:
	SceneNodeParams() {} 
	~SceneNodeParams() {}
	string MeshFilename;
	vector<string> TextureFilenames; 
	Shader* ShaderInstance;
	SceneNodeParams* Child;
};