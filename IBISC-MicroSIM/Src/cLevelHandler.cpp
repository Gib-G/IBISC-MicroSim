#include "cLevelHandler.h"

cLevelHandler::cLevelHandler(){
	mainLevel = NULL;
}

void cLevelHandler::setLevel(cGenericLevel* level) {
	mainLevel = level;
}