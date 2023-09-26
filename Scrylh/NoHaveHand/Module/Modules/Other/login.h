#ifndef IDK_H
#define IDK_H

#include "../../../../Utils/Utils.h"
#include <windows.h>

#include "../Module.h"

class IDK : public IModule {
public:
	bool passedTest = false;
	bool test = false;
	bool BLACKLIST = false;
	std::string R1 = "R1";
	std::string e = "e";
	std::string de = "de";
	std::string DE = "De";
	std::string a = "a";
	std::string A = "A";
	std::string ci = "ci";
	std::string kug = "kug";
	std::string n = "N1";
	std::string T = "T0RI";
	std::string N = "Niwa";
	std::string t = "to";


	IDK();
	~IDK();

	virtual const char* getModuleName() override;
	virtual void onTick(C_GameMode* gm) override;
	virtual void onDisable();
};

#endif // IDK_H
