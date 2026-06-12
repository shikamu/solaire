#pragma once 
#include <vector>

#include "CSLock.h"
#include "PhysicsObject.h"
//#include "LockPointer.h"
#include "Task.h"
//#include "LogicConstants.h"

template<class T> class LockPointer;
enum MaskComparison;

using std::vector;

class PhysicsController : public Task
{
private:
	static PhysicsController m_PhysicsController; 
	vector<LockPointer<PhysicsObject>*> m_ObjectList;
	CSLock m_ListLock;
	MaskComparison MaskCheck(const PhysicsObject &A, const PhysicsObject &B);
	PhysicsController() {}
	~PhysicsController();
protected:
	unsigned int __stdcall run();

public:
	void ForcedObjectsCleanup();

	static PhysicsController& Get();
	void CleanupObjects();
	void finish();
	LockPointer<PhysicsObject>* CreateObject(PhysicsParams& params);
	void Update(float deltaTime);
	void AcquireLock(); 
	void ReleaseLock(); 
};