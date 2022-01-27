#include "cGridLevel.h"

#include "cToolCursorLevel.h"

using namespace std;


#define RESOURCE_PATH(p)    (char*)((m_resourceRoot+string(p)).c_str())

cGridLevel::cGridLevel(const std::string a_resourceRoot,
	const int a_numDevices,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice1) : cToolCursorLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1) {
	
	m_resourceRoot = a_resourceRoot;

	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT MAP
	//--------------------------------------------------------------------------

	////////////////////////////////////////////////////////////////////////////
	// CREATE GROUND
	////////////////////////////////////////////////////////////////////////////
	moveSpeed = 10;
	double maxStiffness;

	toolRadius = 0.01;
	pattern = 0;
	numCube = 14;

	K_SIZE = 10;
	BRUSH_SIZE = 600;

	start = false;

	if (m_numTools > 0)
	{
		paintColor.setGreen();
		m_tool0->m_hapticPoint->m_sphereProxy->m_material->setColor(paintColor);
	}

	if (m_numTools > 1)
	{
		paintColor.setGreen();
		m_tool1->m_hapticPoint->m_sphereProxy->m_material->setColor(paintColor);
	}

	if (a_numDevices > 0) {
		cHapticDeviceInfo hapticDeviceInfo = a_hapticDevice0->getSpecifications();
		double workspaceScaleFactor = m_tools[0]->getWorkspaceScaleFactor();
		maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	}
	else {
		maxStiffness = 3;
	}

	//
	//Needle
	//
	bool fileload;
	double size;
	/*board = new cMesh();
	cCreatePlane(board, 1, 1, cVector3d(-1, 0, -1));
	board->rotateAboutGlobalAxisRad(cVector3d(0, 1, 0), cDegToRad(130));
	board->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));
	m_world->addChild(board);*/
	canvas = new cMesh();
	cCreatePlane(canvas, 1, 1, cVector3d(-0.3, 0, -0.45));
	m_world->addChild(canvas);
	// create collision detector
	canvas->createBruteForceCollisionDetector();
	canvas->setFriction(0.3, 0.3, true);
	canvas->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 50);
	/*
	canvas->rotateAboutGlobalAxisRad(cVector3d(0, 1, 0), cDegToRad(90));
	canvas->rotateAboutGlobalAxisRad(cVector3d(1, 0, 0), cDegToRad(90));*/

	// set graphic properties
	canvas->m_texture = cTexture2d::create();
	fileload = canvas->m_texture->loadFromFile(RESOURCE_PATH("../resources/Images/grid.jpg")); // Images/grid.jpg
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = canvas->m_texture->loadFromFile(ROOT_DIR "Resources/Images/grid.jpg");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}

	// create a copy of canvas so that we can clear page when requested
	canvasOriginal = canvas->m_texture->m_image->copy();

	// we disable lighting properties for canvas
	canvas->setUseMaterial(false);

	// enable texture mapping
	canvas->setUseTexture(true);

	// set haptic properties
	canvas->m_material->setStiffness(0.5 * maxStiffness);
	canvas->m_material->setStaticFriction(0.30);
	canvas->m_material->setDynamicFriction(0.30);
	canvas->m_material->setTextureLevel(1.5);
	ResetCanvas(pattern);


	//----------------------------------------------------------------------
	// Haptic Buttons in Scene
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	// TIMER in Scene
	//----------------------------------------------------------------------
	timer = new cMultiMesh();
	m_world->addChild(timer);
	timer1 = new cMesh();
	timer->addChild(timer1);
	cMatrix3d rot;
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePlane(timer1, .5, 1, cVector3d(0, 0, 0));
	timer1->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer1->translate(cVector3d(0, .75, 0));
	timer2 = new cMesh();
	timer->addChild(timer2);
	cCreatePlane(timer2, .5, 1, cVector3d(0, 0, 0));
	timer2->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer2->translate(cVector3d(0, .25, 0));
	timer3 = new cMesh();
	timer->addChild(timer3);
	cCreatePlane(timer3, .5, 1, cVector3d(0, 0, 0));
	timer3->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer3->translate(cVector3d(0, -0.25, 0));
	timer4 = new cMesh();
	timer->addChild(timer4);
	cCreatePlane(timer4, .5, 1, cVector3d(0, 0, 0));
	timer4->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	timer4->translate(cVector3d(0, -0.75, 0));
	timer->rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
	timer->translate(cVector3d(-3, 0, 1.5));
	//--------------------------------------------------------------
	//Individual timer textures
	//--------------------------------------------------------------
	timer1->m_texture = cTexture2d::create();
	fileload = timer1->m_texture->loadFromFile(RESOURCE_PATH("../resources/Images/0.png")); // Images/0.png
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer1->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	timer1->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer1->setUseTexture(true);
	timer2->m_texture = cTexture2d::create();
	fileload = timer2->m_texture->loadFromFile(RESOURCE_PATH("../resources//Images/0.png")); // Images/0.png
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer2->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	timer2->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer2->setUseTexture(true);
	timer3->m_texture = cTexture2d::create();
	fileload = timer3->m_texture->loadFromFile(RESOURCE_PATH("../resources//Images/0.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer3->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	timer3->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer3->setUseTexture(true);
	timer4->m_texture = cTexture2d::create();
	fileload = timer4->m_texture->loadFromFile(RESOURCE_PATH("../resources/Images/0.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = timer4->m_texture->loadFromFile(ROOT_DIR "Resources/Images/0.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	timer4->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	timer4->setUseTexture(true);
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------

	resetButton = new cMesh();
	m_world->addChild(resetButton);
	cMaterial mat;
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePanel(resetButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	resetButton->translate(cVector3d(-.4, 0.5, -0.45));
	/*
	cFontPtr font = NEW_CFONTCALIBRI20();
	cLabel* label = new cLabel(font);
	label->m_fontColor.setBlack();
	label->setText("Reset");
	resetButton->addChild(label);*/

	// compute collision detection algorithm
	mat.setHapticTriangleSides(true, true);
	mat.setDynamicFriction(0.2);
	mat.setStaticFriction(0.2);
	mat.setStiffness(0.3 * maxStiffness);
	mat.setTextureLevel(1);
	mat.setHapticTriangleSides(true, false);
	resetButton->m_texture = cTexture2d::create();
	fileload = resetButton->m_texture->loadFromFile(RESOURCE_PATH("../resources/Images/resetButton.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = resetButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/resetButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}
	resetButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	resetButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	resetButton->setUseCulling(false, true);


	// compute collision detection algorithm
	resetButton->createAABBCollisionDetector(toolRadius);


	/*plusButton = new cMesh();
	m_world->addChild(plusButton);
	cCreatePanel(plusButton, .25, .25, .1, 8, cVector3d(-.4, -1.1, -0.8), cMatrix3d(cVector3d(0, 1, 0), 1.22173), cColorf(255, 255, 0));
	plusButton->setMaterial(mat);
	plusButton->setUseTexture(true);
	plusButton->setUseCulling(false, true);
	plusButton->m_material->setYellow();
	plusButton->createAABBCollisionDetector(toolRadius);

	minusButton = new cMesh();
	m_world->addChild(minusButton);
	cCreatePanel(plusButton, .25, .25, .1, 8, cVector3d(-.4, -1.1, -1.2), cMatrix3d(cVector3d(0, 1, 0), 1.22173), cColorf(255, 255, 0));
	minusButton->setMaterial(mat);
	minusButton->setUseTexture(true);
	minusButton->setUseCulling(false, true);
	minusButton->m_material->setYellow();
	minusButton->createAABBCollisionDetector(toolRadius);*/

	saveButton = new cMesh();
	m_world->addChild(saveButton);
	rot.identity();
	rot.rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	cCreatePanel(saveButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	saveButton->translate(cVector3d(-.4, -1.05, -0.45));
	saveButton->setMaterial(mat);
	// set graphic properties
	saveButton->m_texture = cTexture2d::create();
	fileload = saveButton->m_texture->loadFromFile(RESOURCE_PATH("../resources/Images/saveButton.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = saveButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/saveButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}

	saveButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	saveButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	saveButton->setUseCulling(false, true);


	// compute collision detection algorithm
	saveButton->createAABBCollisionDetector(toolRadius);

	startButton = new cMesh();
	m_world->addChild(startButton);
	cCreatePanel(startButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	startButton->translate(cVector3d(.2, -.525, -0.45));
	startButton->setMaterial(mat);
	startButton->m_texture = cTexture2d::create();
	fileload = startButton->m_texture->loadFromFile(RESOURCE_PATH("../Resources/Images/startButton.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = startButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/startButton.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
	}

	startButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	startButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	startButton->setUseCulling(false, true);


	// compute collision detection algorithm
	startButton->createAABBCollisionDetector(toolRadius);

	changeButton = new cMesh();
	m_world->addChild(changeButton);
	cCreatePanel(changeButton, .5, .5, .1, 8, cVector3d(0, 0, 0), rot);
	changeButton->translate(cVector3d(.2, -.005, -0.45));
	changeButton->setMaterial(mat);
	changeButton->m_texture = cTexture2d::create();
	fileload = changeButton->m_texture->loadFromFile(RESOURCE_PATH("../Resources/Images/1.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = changeButton->m_texture->loadFromFile(ROOT_DIR "Resources/Images/1.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
		close();
	}

	changeButton->m_texture->setEnvironmentMode(GL_DECAL);
	// enable texture rendering 
	changeButton->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	changeButton->setUseCulling(false, true);


	// compute collision detection algorithm
	changeButton->createAABBCollisionDetector(toolRadius);

	changeButton->translate(cVector3d(0.1, 0.3, -0.8));
	startButton->translate(cVector3d(0.1, 0.3, -0.8));
	saveButton->translate(cVector3d(0.1, 0.3, -0.8));
	resetButton->translate(cVector3d(0.1, 0.3, -0.8));
	canvas->rotateAboutGlobalAxisDeg(cVector3d(0, 0, 1), 90);
	canvas->translate(cVector3d(-0.2, 0.31, -0.8));
	//--------------------------------------------------------------------------
	// CREATE SHADERS
	//--------------------------------------------------------------------------

	// create vertex shader
	cShaderPtr vertexShader = cShader::create(C_VERTEX_SHADER);

	// load vertex shader from file
	fileload = vertexShader->loadSourceFile(RESOURCE_PATH("../resources/Shaders/bump.vert"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = vertexShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.vert");
#endif
	}

	// create fragment shader
	cShaderPtr fragmentShader = cShader::create(C_FRAGMENT_SHADER);

	// load fragment shader from file
	fileload = fragmentShader->loadSourceFile(RESOURCE_PATH("../resources/Shaders/bump.frag"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = fragmentShader->loadSourceFile(ROOT_DIR "Resources/Shaders/bump.frag");
#endif
	}

	// create program shader
	cShaderProgramPtr programShader = cShaderProgram::create();

	// assign vertex shader to program shader
	programShader->attachShader(vertexShader);

	// assign fragment shader to program shader
	programShader->attachShader(fragmentShader);

	// link program shader
	programShader->linkProgram();

	// set uniforms
	programShader->setUniformi("uColorMap", 0);
	programShader->setUniformi("uShadowMap", 0);
	programShader->setUniformi("uNormalMap", 2);
	programShader->setUniformf("uInvRadius", 0.0f);


	//--------------------------------------------------------------------------
	// CREATE ENVIRONMENT GLOBE
	//--------------------------------------------------------------------------

	// create a virtual mesh
	cMesh* globe = new cMesh();

	// add object to m_world
	m_world->addChild(globe);

	// set the position of the object at the center of the m_world
	globe->setLocalPos(0.0, 0.0, 0.0);

	// create cube
	cCreateSphere(globe, 6.0, 360, 360);
	globe->setUseDisplayList(true);
	globe->deleteCollisionDetector();

	// create a texture
	cTexture2dPtr textureSpace = cTexture2d::create();

	fileload = textureSpace->loadFromFile(RESOURCE_PATH("../resources/Images/sky.jpg"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = textureSpace->loadFromFile(ROOT_DIR "Resources/Images/sky.jpg");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Texture image failed to load correctly." << endl;
	}

	// apply texture to object
	globe->setTexture(textureSpace);

	// enable texture rendering 
	globe->setUseTexture(true);

	// Since we don't need to see our polygons from both sides, we enable culling.
	globe->setUseCulling(false);

	// disable material properties and lighting
	globe->setUseMaterial(false);

	for (int i = 0; i < m_numTools; i++)
	{
		state[i] = IDLE;
		selectedObject[i] = NULL;
		InitialPos[i] = m_tools[i]->getDeviceGlobalPos();
		pressed[i] = false;
		touching[i] = false;
	}

	timerNum = 0;

}

void cGridLevel::moveCamera() {

	cVector3d cameraPos = m_camera->getLocalPos();

	if (pressedKey == GLFW_KEY_W) {
		movementVector = m_camera->getLookVector();
	}
	else if (pressedKey == GLFW_KEY_D) {
		movementVector = m_camera->getRightVector();
	}
	else if (pressedKey == GLFW_KEY_A) {
		movementVector = m_camera->getRightVector();
		movementVector.negate();
	}
	else if (pressedKey == GLFW_KEY_S) {

		movementVector = m_camera->getLookVector();
		movementVector.negate();

	}
	else if (pressedKey == GLFW_KEY_SPACE) {
		movementVector.zero();
		movementVector.z(1);
	}
	else if (pressedKey == GLFW_KEY_LEFT_CONTROL) {
		movementVector.zero();
		movementVector.z(-1);
	}
	else {
		movementVector.zero();
	}

	movementVector.mul(deltaTime);
	movementVector.mul(moveSpeed);
	cameraPos.add(movementVector);
	m_camera->setLocalPos(cameraPos);

}

void cGridLevel::keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {

	// filter calls that only include a key press
	if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
	{
		pressedKey = -1;
		return;
	}
	// option - exit
	else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
	{
		glfwSetWindowShouldClose(a_window, GLFW_TRUE);
	}

	pressedKey = a_key;

}

void cGridLevel::updateGraphics() {

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	moveCamera();


}

void cGridLevel::updateHaptics() {

	/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// stop the simulation simClock

	simClock.stop();

	// read the time increment in seconds
	double timeInterval = simClock.getCurrentTimeSeconds();

	// restart the simulation simClock
	simClock.reset();
	simClock.start();
	errorColor.setRed();
	// update frequency counter
	frequencyCounter.signal(1);
	for (int i = 0; i < m_numTools; i++)
	{

		/////////////////////////////////////////////////////////////////////
		// HAPTIC FORCE COMPUTATION
		/////////////////////////////////////////////////////////////////////

		// compute global reference frames for each object
		m_world->computeGlobalPositions(true);

		// update position and orientation of tool
		m_tools[i]->updateFromDevice();

		// compute interaction forces
		m_tools[i]->computeInteractionForces();

		// get interaction forces magnitude
		//force[i] = 5 * tool[i]->getDeviceGlobalForce().length();
		// send forces to haptic device
		m_tools[i]->applyToDevice();

		// get status of user switch
		bool button = m_tools[i]->getUserSwitch(0);
		/////////////////////////////////////////////////////////////////////
	// INTERACTION WITH CANVAS
	/////////////////////////////////////////////////////////////////////
		if (m_tools[i]->isInContact(canvas))
		{
			cCollisionEvent* contact = m_tools[i]->m_hapticPoint->getCollisionEvent(0);
			if (contact != NULL)
			{
				// retrieve contact information
				cVector3d localPos = contact->m_localPos;
				unsigned int triangleIndex = contact->m_index;
				cTriangleArrayPtr triangles = contact->m_triangles;

				// retrieve texture coordinate
				cVector3d texCoord = triangles->getTexCoordAtPosition(triangleIndex, localPos);
				// retrieve pixel information
				int px, py;
				canvas->m_texture->m_image->getPixelLocation(texCoord, px, py);
				cout << "px : " << px << "py : " << py << endl;
				size[i] = cClamp((K_SIZE), 1.0, (double)(BRUSH_SIZE));
				for (int x = -BRUSH_SIZE; x < BRUSH_SIZE; x++)
				{
					for (int y = -BRUSH_SIZE; y < BRUSH_SIZE; y++)
					{
						// compute new color percentage
						distance[i] = sqrt((double)2 * (x * x + y * y));
						if (distance[i] <= size[i])
						{
							// get color at location
							cColorb color, newColor;
							canvas->m_texture->m_image->getPixelColor(px + x, py + y, color);
							if (color != paintColor || color != errorColor) {
								bool hit;
								hit = PaintCanvas(px + x, py + y, pattern);
								if (hit) {
									newColor = paintColor;
								}
								else {
									newColor = errorColor;
								}
								// assign new color to pixel
								int pixelX = px + x;
								int pixelY = py + y;
								if ((pixelX >= 0) && (pixelY >= 0) && (pixelX < canvas->m_texture->m_image->getWidth()) && (pixelY < canvas->m_texture->m_image->getHeight()))
								{
									canvas->m_texture->m_image->setPixelColor(pixelX, pixelY, newColor);
								}
							}
						}
					}
				}
				// update texture
				canvas->m_texture->markForUpdate();
			}
		}

		if (m_tools[i]->getHapticPoint(0)->getNumCollisionEvents() > 0) {
			touching[i] = true;
		}
		else {
			touching[i] = false;
			pressed[i] = false;
		}

		if (m_tools[i]->isInContact(resetButton) && button == true && !pressed[i]) {

			// copy original image of canvas to texture
			ResetCanvas(pattern);

			// update console message
			cout << "> Canvas has been erased.            \r";
			pressed[i] = true;
		}
		else if (m_tools[i]->isInContact(saveButton) && button == true && !pressed[i]) {
			SaveCanvas();
			pressed[i] = true;


		}
		else if (m_tools[i]->isInContact(startButton) && button == true && !pressed[i]) {
			Start();
			pressed[i] = true;

		}
		else if (m_tools[i]->isInContact(changeButton) && button == true && !pressed[i]) {
			ChangePattern();
			pressed[i] = true;

		}
		if (start) posData[i].insert(pair<float, cVector3d>(timerNum, m_tools[i]->getDeviceGlobalPos()));
	}
 
}

void cGridLevel::ResetCanvas(int pattern) {
	for (int k = 0; k < m_numTools; k++) {
		posData[k].clear();
	}
	canvasOriginal->copyTo(canvas->m_texture->m_image);
	cubesize = 1024.0f / numCube;
	cout << pattern << endl;
	switch (pattern) {
	case 1:
		for (int j = 0; j < numCube; j++) {
			for (int x = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); x > 1024 - (j + 1) * cubesize; x--) {
				for (int y = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); y > 1024 - ((j + 1) * cubesize); y--) {
					cColorb getcolor;
					canvas->m_texture->m_image->getPixelColor(x, y, getcolor);
					if (getcolor != paintColor) {
						cColorb yellow;
						yellow.setYellow();
						canvas->m_texture->m_image->setPixelColor(x, y, yellow);
					}
					goalPixels++;
				}
			}
		}
		canvas->m_texture->markForUpdate();
		break;
	default:
		for (int j = 0; j < numCube; j++) {
			for (int x = j * cubesize + 1; x < (j + 1) * cubesize; x++) {
				for (int y = 1023 + (j == 0 ? 0 : 1) - (j * cubesize); y > 1024 - ((j + 1) * cubesize); y--) {
					cColorb getcolor;
					canvas->m_texture->m_image->getPixelColor(x, y, getcolor);
					if (getcolor != paintColor) {
						cColorb yellow;
						yellow.setYellow();
						canvas->m_texture->m_image->setPixelColor(x, y, yellow);
					}
					goalPixels++;
				}
			}
		}
		canvas->m_texture->markForUpdate();
		break;
	}
}
bool cGridLevel::PaintCanvas(int x, int y, int pattern) {
	bool hit = false;
	switch (pattern) {
	case 1:
		for (int j = 0; j < numCube; j++) {
			if (x <= 1024 - cubesize * (j + 1) && x >= 1024 - cubesize * j && y >= 1024 - cubesize * (j + 1) && y <= 1024 - cubesize * j) {
				hit = true;
				break;
			}
		}
		break;
	default:
		for (int j = 0; j < numCube; j++) {
			if (x <= cubesize * (j + 1) && x >= cubesize * j && y >= 1024 - cubesize * (j + 1) && y <= 1024 - cubesize * j) {
				hit = true;
				break;
			}
		}
		break;
	}
	return hit;
}

bool cGridLevel::CompareVectors(cVector3d v1, cVector3d v2) {
	if (v1.x() != v2.x() || v1.y() != v2.y() || v1.z() != v2.z()) return false;
	else return true;
}

void cGridLevel::GetResult() {
	errorColor.setRed();
	errorPixel = 0;
	totalColoredPixels = 0;
	greenPixels = 0;
	for (int k = 0; k < 1024; k++) {
		for (int l = 0; l < 1024; l++) {
			// get color at location
			cColorb getcolor;
			canvas->m_texture->m_image->getPixelColor(k, l, getcolor);
			if (getcolor == errorColor || getcolor == paintColor) {
				if (getcolor == errorColor) {
					errorPixel++;
				}
				if (getcolor == paintColor) {
					greenPixels++;
				}
				totalColoredPixels++;
			}
		}
	}
	bool hit = false;
	errorPercent = (totalColoredPixels ? errorPixel / totalColoredPixels * 100 : 0);
	correctPercent = greenPixels / goalPixels * 100;
	cout.precision(10);
	cout << "Pourcentage d'erreurs : " << errorPercent << "%" << endl;
	cout << "Pourcentage de compl�tion : " << correctPercent << "%" << endl;
	cout << "greenPixels | goalPixels : " << greenPixels << "|" << goalPixels << endl;
}

void cGridLevel::DisplayTimer(float time) {
	std::stringstream temp;
	temp << (int)time;
	string str = temp.str();
	cout << str << endl;
	bool fileload = false;
	string folder = RESOURCE_PATH("../Resources/Images/");
	for (int i = 0; i < str.length(); i++) {
		string stringnum = folder + str[str.length() - 1 - i] + ".png";
		switch (i) {
		case(0):
			fileload = timer1->m_texture->loadFromFile(stringnum);
			timer1->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(1):
			fileload = timer2->m_texture->loadFromFile(stringnum);
			timer2->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(2):
			fileload = timer3->m_texture->loadFromFile(stringnum);
			timer3->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		case(3):
			fileload = timer4->m_texture->loadFromFile(stringnum);
			timer4->m_texture->markForUpdate();
			cout << stringnum << endl;
			break;
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly : " << stringnum << endl;
		}
	}
	for (int j = str.length(); j < 4; j++) {
		string stringnum = folder + "0.png";
		switch (j) {
		case(0):
			fileload = timer1->m_texture->loadFromFile(stringnum);
			timer1->m_texture->markForUpdate();
			break;
		case(1):
			fileload = timer2->m_texture->loadFromFile(stringnum);
			timer2->m_texture->markForUpdate();
			break;
		case(2):
			fileload = timer3->m_texture->loadFromFile(stringnum);
			timer3->m_texture->markForUpdate();
			break;
		case(3):
			fileload = timer4->m_texture->loadFromFile(stringnum);
			timer4->m_texture->markForUpdate();
			break;
		}
		if (!fileload)
		{
			cout << "Error - Texture image failed to load correctly." << endl;
		}

	}
}

void cGridLevel::ChangePattern() {
	pattern = (pattern != MAX_PATTERN ? pattern + 1 : 0);
	ResetCanvas(pattern);
	std::stringstream temp;
	bool fileload = false;
	string folder = RESOURCE_PATH("../Resources/Images/");
	temp << folder << pattern + 1 << ".png";
	fileload = changeButton->m_texture->loadFromFile(temp.str());
	changeButton->m_texture->markForUpdate();
}

void cGridLevel::Start() {
	timerNum = 0;
	start = true;
	ResetCanvas(pattern);
	startButton->setEnabled(false);
	changeButton->setEnabled(false);
}

void cGridLevel::SaveCanvas() {
	GetResult();
	canvas->m_texture->m_image->saveToFile(RESOURCE_PATH("../Resources/Images/myPicture.jpg"));
	string path = RESOURCE_PATH("../Resources/CSV/trajectory-Arm_");
	DisplayTimer(timerNum);
	for (int k = 0; k < m_numTools; k++) {
		std::stringstream temp;
		temp << path << k << ".csv";
		std::ofstream myfile;
		std::cout << "Saving trajectory into /Resources/CSV/trajectory-Arm_" << k << ".csv\n";
		myfile.open(temp.str());
		myfile << "Temps" << " , " << "Position - x" << " , " << "Position - y" << " , " << "Position - z" << " , " << "Temps total" << " , " << "Pourcentage d'erreur" << " , " << "Pourcentage de completion" << "\n";
		std::map<float, cVector3d>::iterator it = posData[k].begin();
		myfile << it->first << " , " << it->second << " , " << timerNum << " , " << errorPercent << " , " << correctPercent << "\n";
		it++;
		for (it; it != posData[k].end(); ++it)
			myfile << it->first << ',' << it->second << '\n';
		myfile.close();
	}
}