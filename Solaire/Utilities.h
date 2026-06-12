#pragma once

enum SolaireInputCode;
enum Action;

class Utilities
{
public:

	static wchar_t* getPrintable(const SolaireInputCode code);

	static wchar_t* getActionName(const Action action);


private:
	Utilities();
	~Utilities();
};

