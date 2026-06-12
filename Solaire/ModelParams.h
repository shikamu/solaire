#pragma once
#include <vector3d.h>
#include <vector>
#include <string>

using irr::core::vector3df;
using std::vector;
using std::string;

class ModelParams
{
public:
	vector<vector3df> NodeLocations;
	float Radius; 
	string ModelFileName;
	vector<string> TextureFileNames; 
	ModelParams() {}
	~ModelParams() {} 
};