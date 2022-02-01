#ifndef CLevelHandler
#define CLevelHandler

#include "cGenericLevel.h"
#include <COculus.h>

class cLevelHandler {
public:
	cLevelHandler(cOVRDevice* o);

	virtual ~cLevelHandler() {};

public:
	virtual void setLevel(cGenericLevel* level);
	virtual void resetCamera();

public:
	cGenericLevel* mainLevel;
	cOVRDevice* oculusVR;
};

#endif // !CLevelHandler
