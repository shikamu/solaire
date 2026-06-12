#pragma once
#include "LogicScene.h"
#include "DummyAgent.h"

namespace FMODAudio
{
	class AudioClip;
};

namespace irr
{
	namespace scene
	{
		class IMeshSceneNode;
	}
	namespace gui
	{
		class IGUIStaticText;
		class IGUITab;
	}
}

class SinglePlayerScene : public LogicScene
{
public:
	SinglePlayerScene();
	~SinglePlayerScene();

	int init();

	int update(const float dt);

	int clean();

	irr::scene::ISceneManager* getSceneManager() const;
private: 

	int m_BulletCounter; 
	irr::scene::ISceneManager* m_smgr;

	DummyAgent m_Dummy; 
	DummyAgent m_Dummy2;
	DummyAgent m_Dummy3;

};

