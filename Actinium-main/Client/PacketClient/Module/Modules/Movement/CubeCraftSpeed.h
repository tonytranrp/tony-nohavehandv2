#include "../Module.h"

class CubeCraftSpeed : public IModule {
private:
	int delay = 0;
	int delay2 = 0;
	bool isBypass = false;
	float speedMod = 0.70f;

public:
	virtual void onMove(C_MoveInputHandler* input) override;
	C_MovePlayerPacket* jetpack = nullptr;
	CubeCraftSpeed();
	~CubeCraftSpeed();


	// Inherited via IModule
	virtual const char* getModuleName() override;
};
