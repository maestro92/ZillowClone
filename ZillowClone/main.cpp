#include "main.h"


#include <stdio.h>
#include <string.h>

#include "global.h"

#define RENDER_DEBUG_FLAG 0


bool incrFlag = true;
bool incrFlag2 = true;

bool incrAngleFlag = true;
bool incrAngleFlag2 = true;

float tempPitch = 0;
float tempYaw = 0;

float tempPitch2 = 0;
float tempYaw2 = 0;





// the server simluates the game in descirete time steps called ticks

glm::vec3 oldPos;

/*
RakNet Ogre tutorial
http://classes.cs.kent.edu/gpg/trac/wiki/cmarshall
*/


// http://stackoverflow.com/questions/4845410/error-lnk2019-unresolved-external-symbol-main-referenced-in-function-tmainc
#undef main


using namespace std;
//using namespace std::placeholders;
// https://www.youtube.com/watch?v=tlXM8qDOS3U
// Screen dimension constants


// frame rate
// https://sites.google.com/site/sdlgamer/intemediate/lesson-7
// FPS is 50
// So Interval is 1000/50 which is 20ms
// meaning my while loop runs 50 frames per second
// which is every other 20 ms
const int FRAMES_PER_SECOND = 60;
const int INTERVAL = 1000 / FRAMES_PER_SECOND;

//display surface
SDL_Surface* pDisplaySurface = NULL;
//event structure
SDL_Event event;


// const bool SINGLE_PLAYER_MODE



const float GAME_SPEED = 1.0f;
const float _FIXED_UPDATE_TIME_s = 0.01667f;
const float FIXED_UPDATE_TIME_s = _FIXED_UPDATE_TIME_s / GAME_SPEED;
const float FIXED_UPDATE_TIME_ms = FIXED_UPDATE_TIME_s * 1000;
//const float _FIXED_UPDATE_TIME_s = 




const int SV_FRAMES_PER_SECOND = 20;
const float SV_FIXED_UPATE_TIME_s = 1 / SV_FRAMES_PER_SECOND;
const long long SV_FIXED_UPATE_TIME_ms = 1000 / SV_FRAMES_PER_SECOND;

const long long CLIENT_INTERP_DELAY_ms = SV_FIXED_UPATE_TIME_ms * 2;



// 15 ms, 66.6 ticks per sec are simulated
const int SERVER_SIMLUATION_FRAMES_PER_SECOND = 66;
const int SERVER_SIMLUATION_TIME_STEP = 1000 / SERVER_SIMLUATION_FRAMES_PER_SECOND;


// 50 ms, 20 snapshots per second
const int SERVER_SNAPSHOT_PER_SECOND = 20;
const int SERVER_SNAPSHOT_TIME_STEP = 1000 / SERVER_SNAPSHOT_PER_SECOND;

// But instead of sending a new packet to the server for each user command, the client sends command packets at a certain rate of packets per second (usually 30).
// This means two or more user commands are transmitted within the same packet.
const int CLIENT_INPUT_SENT_PER_SECOND = 33;
const int CLIENT_INPUT_SENT_TIME_STEP = 1000 / SERVER_SNAPSHOT_PER_SECOND;


const float SPAWN_POSITION_UNIT_OFFSET = 40.0f;

const int INVALID_OBJECT = 0x7FFFFFFF;

// RendererManager FaceOff::m_rendererMgr;
// modelMgr	FaceOff::m_modelMgr;



ZillowClone::ZillowClone()
{
	cout << "default ctr" << endl;
}

ZillowClone::ZillowClone(int nice)
{
	SDL_Surface* image = utl::loadRawImage("fern.png");
	cout << "assginment version" << endl;
	int a = 1;
}

ZillowClone::~ZillowClone()
{

}


void ZillowClone::init()
{
	global.modelMgr = new ModelManager();
	global.modelMgr->init();

	// renderMgr has to init after the lightMgr
	global.rendererMgr = new RendererManager();
	global.rendererMgr->init(utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);




	isRunning = true;




	latencyOptions = { 0, 20, 50, 100, 200 };	// millisecond
	curLatencyOption = latencyOptions.size()-1;
	latency = latencyOptions[curLatencyOption] / 2;

	curLatencyOption = 0;

	containedFlag = false;

	timeProfilerIndex = 0;
	fpsProfilerIndex = 0;

	initObjects();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// initGUI depends on the m_defaultPlayerID, so initNetworkLobby needs to run first
	initGUI();

	for (int i = 0; i < FPS_PROFILER_BUFFER; i++)
	{
		fpsProfiler[i] = 0;
	}


	//Initialize clear color
	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);

	m_zoomFactor = 1.0f;
	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();
//	m_pipeline.perspective(45 * m_zoomFactor, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);
	float range = 50;
	m_pipeline.ortho(-range, range, -range, range, utl::Z_NEAR, utl::Z_FAR);


	Model::enableVertexAttribArrays();

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	SDL_WM_SetCaption("ZillowClone", NULL);
}

GLuint tempTexture;

void ZillowClone::initObjects()
{
	float scale = 100.0;
	o_worldAxis.setScale(scale);
	o_worldAxis.setModelEnum(ModelEnum::xyzAxis);
	o_worldAxis.setModel(global.modelMgr->get(ModelEnum::xyzAxis));

	o_bezierPoint.setScale(20);
	o_bezierPoint.setModelEnum(ModelEnum::bezierPoints);
	o_bezierPoint.setModel(global.modelMgr->get(ModelEnum::bezierPoints));	
}




/*
The client creates user commands from sampling input devices with the same tick rate that the server is running 
with. 

Instead of sending a new packet to the server for each user command, the client sends command packets at a certain rate of packets per second (usually 30).
This means two or more user commands are transmitted within the same packet. The Clients can increase the command rate with c1-cmdrate. This will increase
responsiveness but requires more outgoing bandwidth, too. 

https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking
*/
void ZillowClone::clientFrame(long long dt)
{
	render();	
}



void ZillowClone::GetTimeProfilerAverages()
{
	
	long long total = 0;

	for (int i = 0; i < TIME_PROFILER_BUFFER; i++)
	{
		total += timeProfiler[i];
	}

	cout << "average is " << total / TIME_PROFILER_BUFFER << endl;
	
	/*
	uint64 total = 0;

	for (int i = 0; i < TIME_PROFILER_BUFFER; i++)
	{
		total += timeProfiler[i];
	}

	cout << "average is " << total / TIME_PROFILER_BUFFER << endl;
	*/
	
	//	utl::debug()
}


void ZillowClone::start()
{
	cout << "Start" << endl;
//	Uint32 startTime = SDL_GetTicks();
//	Uint32 curTime = startTime;
//	m_nextGameTick = 0;

	long long dt = 0;
	long long oldTime = utl::getCurrentTime_ms(); 
	long long newTime = 0;
	
	Uint32 time0 = 0;
	Uint32 time1 = 0;
	
	while (isRunning)
	{
		// cout << "frame" << endl;
		time0 = SDL_GetTicks();

		newTime = utl::getCurrentTime_ms();

		dt = newTime - oldTime;
		// cout << "update" << endl;
		update();



		clientFrame(dt);


		oldTime = newTime;
		
		time1 = SDL_GetTicks();
		
		
		// cout << fpsProfilerIndex << endl;
		if (fpsProfilerIndex == FPS_PROFILER_BUFFER)
		{
			fpsProfilerIndex = 0;
		}
		fpsProfiler[fpsProfilerIndex] = (int)(time1 - time0);
		++fpsProfilerIndex;
		
		int fps = getAverageFPS();
		// cout << fps << endl;
		
		/*
		++fpsProfilerIndex;
		if (fpsProfilerIndex > 1000)
		{
			fpsProfilerIndex = 0;
		}
		*/
		
		// fpsProfilerIndex = 1;
		m_gui.setFPS(fps);
	}
}


int ZillowClone::getAverageFPS()
{
	float averageFrameTime = 0;
	for (int i = 0; i < FPS_PROFILER_BUFFER; i++)
	{
		averageFrameTime += fpsProfiler[i];
	}

	if (averageFrameTime == 0)
	{
		return 0;
	}
	else
	{
		averageFrameTime = averageFrameTime / FPS_PROFILER_BUFFER;

		int fps = 1000 / averageFrameTime;

	//	cout << averageFrameTime << " " << fps << endl;
		return fps;
	}
}




#if 0
void FaceOff::serverHandleDeviceEvents()
{

	while (SDL_PollEvent(&event))
	{
		int tmpx, tmpy;
		switch (event.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_MOUSEBUTTONDOWN:

				switch (event.button.button)
				{
					int tmpx, tmpy;
					case SDL_BUTTON_LEFT:
					{
						SDL_GetMouseState(&tmpx, &tmpy);
						m_mouseState.m_leftButtonDown = true;
						m_spectatorCamera.setMouseIn(true);
						break;
					}
				}
				break;


			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						isRunning = false;
						break;

					case SDLK_0:
						containedFlag = !containedFlag;
						break;


					case SDLK_z:
						if (m_server.isInitalized)
							m_spectatorCamera.setMouseIn(false);
						break;
				}
				break;
		}
	}

}
#endif



void ZillowClone::update()
{
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	// need this for GUI
	m_mouseState.m_pos = glm::vec2(mx, utl::SCREEN_HEIGHT - my);


	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				case SDLK_x:

					break;
				case SDLK_z:

					break;
				default:
					break;
			}
		}
		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
				case SDLK_F1:				
					break;


				case SDLK_F2:

					break;

				case SDLK_F3:

					break;
				case SDLK_F4:

					break;

				case SDLK_F5:
					break;

				default:
					break;
			}
		}
	}
}


/*

Frame
	get input, update velocity
	gravity on velocity
	ser
	pos = pos + velocity

	resolveVelocity
	resolvePenetration

	update camera
*/



void ZillowClone::render()
{
	// *******************************************************
	// ************* Rendering *******************************
	// *******************************************************

	m_pipeline.setMatrixMode(MODEL_MATRIX);
	glBindFramebuffer(GL_FRAMEBUFFER, RENDER_TO_SCREEN);
	glViewport(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);

	
	m_pipeline.setMatrixMode(VIEW_MATRIX);
	m_pipeline.loadIdentity();

	m_pipeline.translate(0.0f, 0.0f, 5.0f);
	


	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	


	// Rendering wireframes
	p_renderer = &global.rendererMgr->r_fullVertexColor;
	p_renderer->enableShader();

		o_worldAxis.renderGroup(m_pipeline, p_renderer);
		o_bezierPoint.renderGroup(m_pipeline, p_renderer);

	p_renderer->disableShader();
	



	long long timeNowMillis = getCurrentTimeMillis();

	int deltaTimeMillis = (unsigned int)(timeNowMillis - m_currentTimeMillis);
	m_currentTimeMillis = timeNowMillis;



	int fps = getAverageFPS();
	m_gui.setFPS(fps);

	m_gui.initGUIRenderingSetup();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_BLEND);


	m_gui.updateAndRender(m_mouseState);

	SDL_GL_SwapBuffers();
}


long long ZillowClone::getCurrentTimeMillis()
{
#ifdef WIN32
	return GetTickCount();
#else
	timeval t;
	gettimeofday(&t, NULL);

	long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
	return ret;
#endif
}



#define MAX_CLIENTS 10
#define SERVER_PORT 60000


int main(int argc, char *argv[])
{
//	RakNetFunction2();
//	return 0;

	utl::debug("Game Starting"); 
	utl::initSDL(utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT, pDisplaySurface);
	utl::initGLEW();

	ZillowClone Martin1(2);
	ZillowClone Martin;


	Martin.init();
	Martin.start();

	utl::exitSDL(pDisplaySurface);
	//normal termination

	while (1)
	{

	}

	cout << "Terminating normally." << endl;
	return EXIT_SUCCESS;
}


int ZillowClone::endWithError(char* msg, int error)
{
	//Display error message in console
	cout << msg << "\n";
	system("PAUSE");
	return error;
}


// http://kcat.strangesoft.net/mpstream.c



void ZillowClone::initGUI()
{
	// run this before m_gui.init, so the textEngine is initialized
	// need to comeback and re-organize the gui the minimize dependencies
	Control::init("", 25, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);
	m_gui.init(utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);

	/*
//	if (!m_isServer)
	{
		int xOffset = 55;
		int yOffset = 570;

		int BAR_WIDTH = 60;
		int BAR_HEIGHT = 10;

		Control* HPBar = new Bar(xOffset, yOffset, BAR_WIDTH, BAR_HEIGHT, GREEN, "icon_hp.png");
		// cl_players.get(m_defaultPlayerObjectId)->m_healthBarGUI = (Bar*)HPBar;

		xOffset = 175;
		Control* armorBar = new Bar(xOffset, yOffset, BAR_WIDTH, BAR_HEIGHT, GRAY, "icon_armor.png");
		// cl_players.get(m_defaultPlayerObjectId)->m_armorBarGUI = (Bar*)armorBar;

		xOffset = 700;
		Control* ammoBar = new Bar(xOffset, yOffset, BAR_WIDTH, BAR_HEIGHT, GRAY, "icon_ammo.png");
		// cl_players.get(m_defaultPlayerObjectId)->m_ammoBarGUI = (Bar*)ammoBar;

		xOffset = 0; yOffset = 0;
		Label* fpsLabel = new Label("90", xOffset, yOffset, 50, 50, GRAY);

		int aimWidth = 20;
		int aimHeight = 20;

		int aimX = utl::SCREEN_WIDTH / 2 - aimWidth / 2;
		int aimY = utl::SCREEN_HEIGHT / 2;

		utl::debug("aimX", aimX);
		utl::debug("aimY", aimY);
		Control* horiAim = new Label("", aimX, aimY - 1, aimWidth, 2, GREEN);

		aimX = utl::SCREEN_WIDTH / 2;
		aimY = utl::SCREEN_HEIGHT / 2 - aimHeight / 2;

		utl::debug("aimX", aimX);
		utl::debug("aimY", aimY);

		Control* vertAim = new Label("", aimX - 1, aimY, 2, aimHeight, GREEN);

		m_gui.addGUIComponent(HPBar);
		m_gui.addGUIComponent(armorBar);
		m_gui.addGUIComponent(ammoBar);

		m_gui.addGUIComponent(fpsLabel);
		m_gui.setFPSLabel(fpsLabel);

		m_gui.addGUIComponent(horiAim);
		m_gui.setHorAimIndex(m_gui.getNumGUIComponent() - 1);

		m_gui.addGUIComponent(vertAim);
		m_gui.setVerAimIndex(m_gui.getNumGUIComponent() - 1);
	}
	*/
}


void ZillowClone::renderGUI()
{

	m_gui.initGUIRenderingSetup();
	/// http://sdl.beuc.net/sdl.wiki/SDL_Average_FPS_Measurement
	//	unsigned int getTicks = SDL_GetTicks();

	//	static string final_str = "(" + utl::floatToStr(m_mouseState.m_pos.x) + ", " + utl::floatToStr(m_mouseState.m_pos.y) + ")";
	m_gui.updateAndRender(m_mouseState);

	// healthbar and text


}




void ZillowClone::clientHandleDeviceEvents()
{
	while (SDL_PollEvent(&event))
	{
		int tmpx, tmpy;
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_MOUSEBUTTONUP:
			break;

		case SDL_MOUSEBUTTONDOWN:

			switch (event.button.button)
			{
				int tmpx, tmpy;
			case SDL_BUTTON_LEFT:
				SDL_GetMouseState(&tmpx, &tmpy);
				m_mouseState.m_leftButtonDown = true;






				break;


			case SDL_BUTTON_RIGHT:
				cout << "clicking right" << endl;
				SDL_GetMouseState(&tmpx, &tmpy);
				m_mouseState.m_rightButtonDown = true;

				m_zoomedIn = !m_zoomedIn;
				m_gui.setSniperZoomMode(m_zoomedIn);

				if (m_zoomedIn)
				{
					m_zoomFactor = 0.2f;
					m_pipeline.perspective(45 * m_zoomFactor, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);
				}
				else
				{
					m_zoomFactor = 1.0f;
					m_pipeline.perspective(45 * m_zoomFactor, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);
				}
				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			}
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				isRunning = false;
				break;

			case SDLK_0:
				containedFlag = !containedFlag;
				break;
				/*
				// main gun
				case SDLK_1:
				m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::MAIN);
				break;

				// pistol
				case SDLK_2:
				m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::PISTOL);
				break;

				// MELEE
				case SDLK_3:
				m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::MELEE);
				break;

				// GRENADES
				case SDLK_4:
				m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::PROJECTILE);
				break;
				*/

			case SDLK_8:
			{


			}

			break;



			case SDLK_9:
				//	if (m_players[m_defaultPlayerID]->has
				break;


			case SDLK_r:
			{
				utl::debug("Reloading Weapon");
				//m_players[m_defaultPlayerID]->reloadWeapon();
			}
			break;

			case SDLK_g:
			{

				utl::debug("Dropping Weapon");
				/*
				Weapon* droppedWeapon = m_players[m_defaultPlayerID]->drop();
				if (droppedWeapon != NULL)
				{
				m_objectKDtree.insert(droppedWeapon);
				}
				*/
			}
			break;




			case SDLK_SPACE:
				//		if (m_players[m_defaultPlayerID]->m_velocity.y == 0.0)
				//		utl::debug(">>>> Just Jumped");
				//		utl::debug("m_players[m_defaultPlayerID]->m_velocity", m_players[m_defaultPlayerID]->m_velocity);

				//		if (m_players[m_defaultPlayerID]->isNotJumping())
				//			m_players[m_defaultPlayerID]->m_velocity += glm::vec3(0.0, 150.0, 0.0) * utl::GRAVITY_CONSTANT;

				break;

			case SDLK_z:
				/*
				if (m_isServer)
				m_serverCamera.setMouseIn(false);
				else
				m_players[m_defaultPlayerID]->m_camera->setMouseIn(false);
				|*/
				break;
			}
			break;
		}
	}

}





#if 0
void FaceOff::clientHandleDeviceEvents()
{
	while (SDL_PollEvent(&event))
	{
		int tmpx, tmpy;
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				m_mouseState.m_leftButtonDown = false;
				SDL_GetMouseState(&tmpx, &tmpy);

				if (m_players[m_defaultPlayerID]->inGrenadeGatherMode())
				{
					utl::debug("here in Grenade gather mode");
					Weapon* grenade = m_players[m_defaultPlayerID]->throwGrenade();
				}

				hitNode = NULL;

				break;

			case SDL_BUTTON_RIGHT:
				m_mouseState.m_rightButtonDown = false;
				SDL_GetMouseState(&tmpx, &tmpy);
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:

			switch (event.button.button)
			{
				int tmpx, tmpy;
			case SDL_BUTTON_LEFT:
				SDL_GetMouseState(&tmpx, &tmpy);
				m_mouseState.m_leftButtonDown = true;

				if (m_players[m_defaultPlayerID]->m_camera->getMouseIn())
				{

					m_players[m_defaultPlayerID]->fireWeapon();

					if (m_players[m_defaultPlayerID]->isUsingLongRangedWeapon())
					{

						WorldObject* hitObject = NULL;

						glm::vec3 lineStart = m_players[m_defaultPlayerID]->getFirePosition();
						glm::vec3 lineDir = -m_players[m_defaultPlayerID]->m_camera->m_targetZAxis;


						utl::debug("lineStart", lineStart);
						utl::debug("lineDir", lineDir);

						// m_objectKDtree.visitNodes(m_objectKDtree.m_head, lineStart, lineDir, 500.0f, hitObject, 0, hitNode);

						float hitObjectSqDist = FLT_MAX;
						glm::vec3 hitPoint;

						unordered_set<int> objectsAlreadyTested;

						m_objectKDtree.visitNodes(m_objectKDtree.m_head, m_players[m_defaultPlayerID], lineStart, lineDir, 500.0f, hitObject, hitObjectSqDist, hitPoint, objectsAlreadyTested);

						//	utl::debug("player pos", lineStart);
						//	utl::debug("target z", lineDir);

						if (hitObject != NULL)
						{
							utl::debug("name", hitObject->m_name);
							hitObject->isHit = true;

							WorldObject* hitPointMark = new WorldObject();
							hitPointMark->setPosition(hitPoint);
							hitPointMark->setScale(1.0, 1.0, 1.0);
							hitPointMark->setModelEnum(ModelEnum::cube);
							hitPointMark->setModel(m_modelMgr.get(ModelEnum::cube));
							hitPointMark->m_name = "hitMark";
							//								m_hitPointMarks.push_back(hitPointMark);
						}
						else
							utl::debug("hitObject is NULL");
						// VisitNodes

					}

				}

				m_players[m_defaultPlayerID]->m_camera->setMouseIn(true);





				break;


			case SDL_BUTTON_RIGHT:
				cout << "clicking right" << endl;
				SDL_GetMouseState(&tmpx, &tmpy);
				m_mouseState.m_rightButtonDown = true;

				m_zoomedIn = !m_zoomedIn;
				m_gui.setSniperZoomMode(m_zoomedIn);

				if (m_zoomedIn)
				{
					m_zoomFactor = 0.2f;
					m_pipeline.perspective(45 * m_zoomFactor, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);
				}
				else
				{
					m_zoomFactor = 1.0f;
					m_pipeline.perspective(45 * m_zoomFactor, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);
				}
				break;
			}
			break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			}
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				isRunning = false;
				break;

			case SDLK_0:
				containedFlag = !containedFlag;
				break;

				// main gun
			case SDLK_1:
				//	m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::MAIN);
				break;

				// pistol
			case SDLK_2:
				//	m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::PISTOL);
				break;

				// MELEE
			case SDLK_3:
				//	m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::MELEE);
				break;

				// GRENADES
			case SDLK_4:
				//	m_players[m_defaultPlayerID]->switchWeapon(WeaponSlotEnum::PROJECTILE);
				break;


			case SDLK_8:
			{


			}

			break;



			case SDLK_9:
				//	if (m_players[m_defaultPlayerID]->has
				break;


			case SDLK_r:
			{
				utl::debug("Reloading Weapon");
				//	m_players[m_defaultPlayerID]->reloadWeapon();
			}
			break;

			case SDLK_g:
			{
				utl::debug("Dropping Weapon");
				/*
				Weapon* droppedWeapon = m_players[m_defaultPlayerID]->drop();
				if (droppedWeapon != NULL)
				{
				m_objectKDtree.insert(droppedWeapon);
				}
				*/
			}
			break;




			case SDLK_SPACE:
				//		if (m_players[m_defaultPlayerID]->m_velocity.y == 0.0)
				//		utl::debug(">>>> Just Jumped");
				//		utl::debug("m_players[m_defaultPlayerID]->m_velocity", m_players[m_defaultPlayerID]->m_velocity);

				//		if (m_players[m_defaultPlayerID]->isNotJumping())
				//			m_players[m_defaultPlayerID]->m_velocity += glm::vec3(0.0, 150.0, 0.0) * utl::GRAVITY_CONSTANT;

				break;

			case SDLK_z:
				/*
				if (m_server.isInitalized)
				m_spectatorCamera.setMouseIn(false);
				else
				m_players[m_defaultPlayerID]->m_camera->setMouseIn(false);
				*/
				break;
			}
			break;
		}
	}

}
#endif





