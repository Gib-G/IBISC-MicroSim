#include "Pick-and-Place.h"
#include "chai3d.h"
#include <iostream>

//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;

void virtsaGoThreadFunction();

int main()
{
	cThread* myThread = new cThread();
	InitializeApp(true, true, false, 5, 115200);
	myThread->start(virtsaGoThreadFunction, CTHREAD_PRIORITY_GRAPHICS);

	StartApp();

	cSleepMs(1000);

	cVector3d newPos = cVector3d(0, 0, 0.02);
	char c = 'x';
	bool haptics = true;

	do
	{
		cin >> c;

		if (c == 'i') {
			//StartVirtsa();
		}

		if (c == 'r') {
			resetObjectsPositions();
		}

		if (c == 'h') {
			SetHapticForces(haptics);
			haptics = !haptics;
			printf("forces  %d\n", !haptics);
		}

		if (c == 'n') {
			UpdateGameScenePosition(newPos.x(), newPos.y(), newPos.z());
		}

		if (c == 'd') {
			double inf[255];
			int res = GetInteractObjects(inf);

			for (int i = 0; i < res; i++) {
				cout << inf[i] << " ";
			}
			cout << endl;
		}

		if (c >= '0' && c <= '5') {
			char cc[2];
			cc[0] = c;
			cc[1] = '\0';
			int ci = atoi(cc);
			updateDynamics(ci, true);
			printf("dynamics ON cube %d\n", ci);
		}

	} while (c != 's');

	finishApp();
	myThread->stop();
}

void virtsaGoThreadFunction() {
	SetVirtualScene();
}