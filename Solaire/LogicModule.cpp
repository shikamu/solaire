#include "LogicModule.h"
#include "SpaceObjectFactory.h"
#include "LogicConstants.h"

ModuleNode::ModuleNode() : Type(MT_NONE), Size(MS_ZERO), FittedModule(NULL), ActivationType(1) , m_ModuleTransferred(false)
{

}

ModuleNode::ModuleNode(const ModuleNode& other)
{
	Type = other.Type;
	Size = other.Size; 
	ActivationType = other.ActivationType;
	RelativePosition = other.RelativePosition;
	RelativeRotation = other.RelativeRotation;
	FittedModule = NULL;
	m_ModuleTransferred = false;
}
