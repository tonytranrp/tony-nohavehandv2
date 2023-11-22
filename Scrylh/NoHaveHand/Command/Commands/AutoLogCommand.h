#pragma once
#include "ICommand.h"
#include <sstream>

class AutoLogCommand : public IMCCommand {
public:
	virtual bool execute(std::vector<std::string>* args) override;
	AutoLogCommand();
};