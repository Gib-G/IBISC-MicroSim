#ifndef CLevelHandler
#define CLevelHandler

#include "cGenericLevel.h"

class cLevelHandler {
public:
	cLevelHandler();

	virtual ~cLevelHandler() {};

public:
	virtual void setLevel(cGenericLevel* level);

public:
	cGenericLevel* mainLevel;
};

#endif // !CLevelHandler
