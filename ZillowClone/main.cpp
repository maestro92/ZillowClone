#include "main.h"


#include <stdio.h>
#include <string.h>

#include "global.h"

#define RENDER_DEBUG_FLAG 0



float CAMERA_POS_DELTA = 1;
float CAMERA_ZOOM_DELTA = 1;

#define PI 3.14159265

// the server simluates the game in descirete time steps called ticks



float REMOVED_EDGE_LINE_WIDTH = 0.1;

float INPUT_LINE_LINE_WIDTH = 0.5;
float MERGED_LINE_LINE_WIDTH = 0.4;

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


const float GAME_SPEED = 1.0f;
const float _FIXED_UPDATE_TIME_s = 0.01667f;
const float FIXED_UPDATE_TIME_s = _FIXED_UPDATE_TIME_s / GAME_SPEED;
const float FIXED_UPDATE_TIME_ms = FIXED_UPDATE_TIME_s * 1000;

const float MOUSE_DIST_THRESHOLD = 0.05;


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

	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();
//	m_pipeline.perspective(90, utl::SCREEN_WIDTH / utl::SCREEN_HEIGHT, utl::Z_NEAR, utl::Z_FAR);

	m_cameraZoom = 50;
	m_cameraCenter = glm::vec2(0, 0);

	updateCamera();

	Model::enableVertexAttribArrays();

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	SDL_WM_SetCaption("ZillowClone", NULL);


	loadData = false;

	if (loadData)
	{
		curDrawing.saveLatest = false;
		curDrawing.loadTestData("rand_shape11.txt");

		createRenderHandleForLoadedTestData(curDrawing);
	//	debugDrawing(curDrawing);

	//	curDrawing.postProcess();
	//	createLinesForRemovedEdges(curDrawing);
	}
	else
	{
		curDrawing.saveLatest = true;
	}
}

GLuint tempTexture;

void ZillowClone::initObjects()
{
	float scale = 100.0;
	o_worldAxis.setScale(scale);
	o_worldAxis.setModel(global.modelMgr->get(ModelEnum::xyzAxis));

	o_bezierPoint.setScale(20);
	o_bezierPoint.setModel(global.modelMgr->get(ModelEnum::bezierPoints));	



//	lineMarkers
	
	for (int i = -50; i <= 50; i += 10)
//	for (int i = 0; i <= 50; i += 10)
	{
		WorldObject obj = WorldObject();
		obj.setModel(global.modelMgr->get(ModelEnum::centeredQuad));
		obj.setPosition(glm::vec3(i, 0, 0));

		obj.setScale(1);

		lineMarkers.push_back(obj);
	}


	
	for (int i = -10; i <= 10; i += 10)
//	for (int i = 0; i <= 10; i += 10)

	{
		WorldObject obj = WorldObject();
		obj.setModel(global.modelMgr->get(ModelEnum::centeredQuad));
		obj.setPosition(glm::vec3(i, 10, 0));

		obj.setScale(1);

		lineMarkers.push_back(obj);

		
		obj = WorldObject();
		obj.setModel(global.modelMgr->get(ModelEnum::centeredQuad));
		obj.setPosition(glm::vec3(i, -10, 0));

		obj.setScale(1);

		lineMarkers.push_back(obj);
		
	}
	



	curDrawing.onAddIntersection = [this](glm::vec2 point)
	{
		WorldObject obj = constructPoint(point, MERGED_LINE_LINE_WIDTH);
		polygonPoints.push_back(obj);
	};


	curDrawing.createLineCallback = [this](glm::vec2 p0, glm::vec2 p1)
	{
		WorldObject line = constructLine(p0, p1, MERGED_LINE_LINE_WIDTH);
		polygonLines.push_back(line);
	};

	
	curDrawing.createPointCallback = [this](glm::vec2 point)
	{
		WorldObject obj = constructPoint(point, MERGED_LINE_LINE_WIDTH);
		polygonPoints.push_back(obj);
	};

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



/*
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

					case SDLK_UP:
						m_cameraCenter.y += delta;
					case SDLK_DOWN:
						m_cameraCenter.y -= delta;
					case SDLK_LEFT:
						m_cameraCenter.x -= delta;
					case SDLK_RIGHT:
						m_cameraCenter.y += delta;
						updateCamera();
						break;
				}
				break;
		}
	}

}
#endif
*/
void ZillowClone::updateCamera()
{
	m_pipeline.ortho(m_cameraCenter.x - m_cameraZoom, 
					m_cameraCenter.x + m_cameraZoom,
					m_cameraCenter.y - m_cameraZoom,
					m_cameraCenter.y + m_cameraZoom, utl::Z_NEAR, utl::Z_FAR);

//	debugDrawing(curDrawing);
}

void ZillowClone::onMouseBtnUp()
{
//	cout << "onMouseBtnUp" << endl;

	if (inDrawingMode)
	{
		startedCurrentLine = false;
		
		addPoint(curDrawing.getFirstPoint());

		curDrawing.postProcessInputPoints();


		curDrawing.postProcess();
	//	debugDrawing(curDrawing);

		
		curDrawing.doEarClipping();
		createPointHandlesForEarclippingPolygons();

		curDrawing.determinePolygonsInsideOutside();
		createLinesForInsideOutsidePolygons(curDrawing);
		createModelsForDrawing(curDrawing);

//		createPointHandlesForEarclippingPolygons();

		curDrawing.reset();
		drawingList.push_back(curDrawing);
	}
}




void ZillowClone::createLinesForRemovedEdges(Drawing drawingIn)
{
	removedEdges.clear();
	for (int i = 0; i < drawingIn.m_removedEdges.size(); i++)
	{
		int id0 = drawingIn.m_removedEdges[i].id0;
		int id1 = drawingIn.m_removedEdges[i].id1;

		Vertex v0 = drawingIn.vertices[id0];
		Vertex v1 = drawingIn.vertices[id1];
		
		WorldObject obj = constructLine(v0.pos, v1.pos, REMOVED_EDGE_LINE_WIDTH);
		removedEdges.push_back(obj);
	}
}

void ZillowClone::createRenderHandleForLoadedTestData(Drawing drawingIn)
{
	polygonLines.clear();
	polygonPoints.clear();

	glm::vec2 lastPoint;
	glm::vec2 worldPoint;

	vector<Vertex> temp = drawingIn.vertices;
	temp.push_back(temp[0]);
	float width = 0.1;

	for (int i = 0; i < temp.size(); i++)
	{
		if (i == 0)
		{
			// do nothing for now
		}
		else
		{
			lastPoint = temp[i - 1].pos;
			worldPoint = temp[i].pos;

			WorldObject obj = constructLine(lastPoint, worldPoint, width);
			polygonLines.push_back(obj);
		}

		WorldObject obj = constructPoint(worldPoint, width);
		polygonPoints.push_back(obj);
	}
}



WorldObject ZillowClone::constructPoint(glm::vec2 p, float width) const
{

	WorldObject obj = WorldObject();
	obj.setModel(global.modelMgr->get(ModelEnum::centeredQuad));
	obj.setPosition(glm::vec3(p.x, p.y, 0));


	obj.setScale(width);

	return obj;
}

WorldObject ZillowClone::constructLine(glm::vec2 p0, glm::vec2 p1, float width) const
{
	WorldObject obj = WorldObject();
	obj.setModel(global.modelMgr->get(ModelEnum::centeredQuad));

	glm::vec2 diffVector = p1 - p0;
	glm::vec2 centerPoint = p0 + glm::vec2(diffVector.x / 2.0, diffVector.y / 2.0);

	obj.setPosition(glm::vec3(centerPoint.x, centerPoint.y, 0));

	float angle = atan2(diffVector.y, diffVector.x) * 180 / PI;

	float length = glm::distance(p0, p1);

	glm::vec3 scale(length, width, 1);

	obj.setRotation(glm::rotate(angle, 0.0f, 0.0f, 1.0f));

	obj.setScale(scale);

	return obj;
}



void ZillowClone::createPointHandlesForEarclippingPolygons()
{
	ecTriangles.clear();
	for (int i = 0; i < curDrawing.earclippingPolygons.size(); i++)
	{
		createPointHandlesForEarclippingPolygon(curDrawing.earclippingPolygons[i]);
	}
}


void ZillowClone::createPointHandlesForEarclippingPolygon(EarclippingPolygon ecPolygon)
{
	float earClippingLineWidth = 0.3f;
	for (int i = 0; i < ecPolygon.triangles.size(); i++)
	{
		vector<Vertex> triangle = ecPolygon.triangles[i];
		WorldObject line0 = constructLine(triangle[0].pos, triangle[1].pos, earClippingLineWidth);
		WorldObject line1 = constructLine(triangle[0].pos, triangle[2].pos, earClippingLineWidth);
		WorldObject line2 = constructLine(triangle[1].pos, triangle[2].pos, earClippingLineWidth);
		ecTriangles.push_back(line0);
		ecTriangles.push_back(line1);
		ecTriangles.push_back(line2);
	}
}



void ZillowClone::createLinesForInsideOutsidePolygons(Drawing drawingIn)
{
	insidePolygonLines.clear();
	outsidePolygonLines.clear();

	for (int i = 0; i < drawingIn.polygons.size(); i++)
	{
		vector<Vertex> vertices = drawingIn.getVerticesByIds(drawingIn.polygons[i]);
		createLinesForInsideOutsidePolygon(vertices, drawingIn.polygonInsideFlags[i]);
	}
}


void ZillowClone::createLinesForInsideOutsidePolygon(vector<Vertex> polygons, bool isInside)
{
	for (int i = 0; i < polygons.size(); i++)
	{
		if (i == 0)
		{

		}
		else
		{
			Vertex v0 = polygons[i - 1];
			Vertex v1 = polygons[i];
			
			WorldObject line = constructLine(v0.pos, v1.pos, 0.5f);

			if (isInside)
			{
				insidePolygonLines.push_back(line);
			}
			else
			{
				outsidePolygonLines.push_back(line);
			}
		}
	}
}



void ZillowClone::debugDrawing(Drawing drawing)
{
	m_gui.removeDebugLabels();
 	float size = 300 / m_cameraZoom;
//	float size = m_cameraZoom * 0.8;
//	float size = 300 / m_cameraZoom;

	for (int i = 0; i < drawing.vertices.size(); i++)
	{
		Vertex v = drawing.vertices[i];

		glm::vec2 pos = v.pos;
		glm::vec3 screenPos = worldToScreen(glm::vec3(pos.x, pos.y, 0));
		glm::vec3 labelPos = screenToUISpace(glm::vec2(screenPos.x, screenPos.y));

		string s = utl::vec2ToStr(pos);

		Label* verticeLabel = new Label(utl::intToStr(v.id), labelPos.x, labelPos.y, 0, 0, COLOR_WHITE);
//		Label* verticeLabel = new Label(s, labelPos.x, labelPos.y, 0, 0, COLOR_WHITE);
		verticeLabel->setFont(size, COLOR_PURPLE);
		m_gui.addDebugLabel(verticeLabel);
	}
}




// VBO with dynamically changing number of points
// https://www.opengl.org/discussion_boards/showthread.php/178828-VBO-with-dynamically-changing-number-of-points-%21%21
// will need to store it both CPU and GPU
// need it on CPU to process enclosed data
// need it on GPU for rendering
// thickness is inversely proportional


// method1: load your sprites, then render them as textured quad.
// method2: glBufferData Way





void ZillowClone::addPoint(glm::vec2 worldPoint)
{
	float width = 0.5;
	if (curDrawing.getNumInputPoints() > 0)
	{
		curDrawing.addInputPoint(worldPoint);
		WorldObject line = constructLine(lastPoint, worldPoint, width);
		inputPolygonLines.push_back(line);
	}
	else
	{
		curDrawing.processNewPoint(worldPoint);
	}

	WorldObject obj = constructPoint(worldPoint, width);
	inputPolygonPoints.push_back(obj);
	

	/*
	cout << endl << endl << endl << endl;
	float width = 0.5;
	if (curDrawing.getNumPoints() > 0)
	{
		curDrawing.processNewPoint(worldPoint);
		WorldObject line = constructLine(lastPoint, worldPoint, width);
		polygonLines.push_back(line);
	}
	else
	{
		curDrawing.processNewPoint(worldPoint);
	}
		
	WorldObject obj = constructPoint(worldPoint, width);
	polygonPoints.push_back(obj);
	*/
}


glm::vec3 ZillowClone::screenToWorldPoint(glm::vec2 screenPoint)
{
	glm::vec4 viewPort = glm::vec4(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);
	glm::vec3 temp = glm::vec3(screenPoint.x, screenPoint.y, 0);

	glm::vec3 worldPoint = glm::unProject(temp, (m_pipeline.getModelViewMatrix()), m_pipeline.getProjectionMatrix(), viewPort);
	return worldPoint;
}


glm::vec3 ZillowClone::screenToUISpace(glm::vec2 screenPoint)
{
	glm::vec4 viewPort = glm::vec4(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);
	glm::vec3 temp = glm::vec3(screenPoint.x, screenPoint.y, 0);

	glm::vec3 worldPoint = glm::unProject(temp, (m_gui.getPipeline().getModelViewMatrix()), m_gui.getPipeline().getProjectionMatrix(), viewPort);
	
	return worldPoint;
}


glm::vec3 ZillowClone::worldToScreen(glm::vec3 pos)
{
	glm::vec4 viewPort = glm::vec4(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);
//	glm::vec3 screenPos = glm::project(pos, glm::inverse(m_pipeline.getModelViewMatrix()), m_pipeline.getProjectionMatrix(), viewPort);
	glm::vec3 screenPos = glm::project(pos, m_pipeline.getModelViewMatrix(), m_pipeline.getProjectionMatrix(), viewPort);
	return screenPos;
}


void ZillowClone::onMouseBtnDown()
{
	int tmpx, tmpy;
	SDL_GetMouseState(&tmpx, &tmpy);
	tmpy = utl::SCREEN_HEIGHT - tmpy;

	if (inDrawingMode)
	{
		startedCurrentLine = true;
		glm::vec2 screenPoint = glm::vec2(tmpx, tmpy);

		glm::vec3 worldPoint = screenToWorldPoint(screenPoint);
		glm::vec2 tempWorldPoint = glm::vec2(worldPoint.x, worldPoint.y);		

		addPoint(tempWorldPoint);
		lastPoint = tempWorldPoint;
	}
}


void ZillowClone::onMouseBtnHold()
{
	int tmpx, tmpy;
	SDL_GetMouseState(&tmpx, &tmpy);
	tmpy = utl::SCREEN_HEIGHT - tmpy;

	if (startedCurrentLine)
	{
		glm::vec2 screenPoint = glm::vec2(tmpx, tmpy);

		glm::vec3 worldPoint = screenToWorldPoint(screenPoint);
		glm::vec2 tempWorldPoint = glm::vec2(worldPoint.x, worldPoint.y);

		if (isNewPoint(tempWorldPoint))
		{
			addPoint(tempWorldPoint);
		}
		lastPoint = tempWorldPoint;
	}
}


bool ZillowClone::isNewPoint(glm::vec2 newPoint)
{
	glm::vec2 diff = newPoint - lastPoint;

	float distSqr = glm::dot(diff, diff);
	/*
	utl::debug("		newPoint is ", newPoint);
	utl::debug("		lastPoint is ", lastPoint);

	utl::debug("		distSqr is ", distSqr);
	utl::debug("		MOUSE_DIST_THRESHOLD is ", MOUSE_DIST_THRESHOLD);
	*/
	if (distSqr > MOUSE_DIST_THRESHOLD)
	{
		return true;
	}
	else
	{
		return false;
	}

}


void ZillowClone::onExistDrawingMode()
{
	curDrawing.reset();
	drawingList.clear();
	polygonLines.clear();
	polygonPoints.clear();

	inputPolygonLines.clear();
	inputPolygonPoints.clear();

	drawingWorldObjects.clear();

}

void ZillowClone::update()
{
	int mx, my;
	SDL_GetMouseState(&mx, &my);

	// need this for GUI
	m_mouseState.m_pos = glm::vec2(mx, utl::SCREEN_HEIGHT - my);


	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{


			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						isRunning = false;
						break;

					case SDLK_q:
						m_cameraZoom -= CAMERA_ZOOM_DELTA;
						updateCamera();

						break;

					case SDLK_v:
						curDrawing.postProcess();
						createLinesForRemovedEdges(curDrawing);
						break;

					case SDLK_t:
						curDrawing.doEarClipping();
						createPointHandlesForEarclippingPolygons();
						break;

					case SDLK_y:
						curDrawing.determinePolygonsInsideOutside();
						createLinesForInsideOutsidePolygons(curDrawing);
						createModelsForDrawing(curDrawing);
						break;


					case SDLK_w:
						m_cameraZoom -= CAMERA_ZOOM_DELTA;
						updateCamera();
						break;

					case SDLK_e:
						/*
						if (startedCurrentLine)
						{
							onMouseBtnHold();
						}
						*/

						break;
					case SDLK_x:
						break;
					case SDLK_z:
						utl::debug("In here");
						inDrawingMode = !inDrawingMode;

						if (inDrawingMode)
						{

						}
						else
						{
							onExistDrawingMode();
						}

						m_gui.setDrawingModeFlag(inDrawingMode);
						break;

					case SDLK_UP:
						m_cameraCenter.y += CAMERA_POS_DELTA;
						updateCamera();
						break;
					case SDLK_DOWN:
						m_cameraCenter.y -= CAMERA_POS_DELTA;
						updateCamera();
						break;
					case SDLK_LEFT:
						m_cameraCenter.x -= CAMERA_POS_DELTA;
						updateCamera();
						break;
					case SDLK_RIGHT:
						m_cameraCenter.x += CAMERA_POS_DELTA;
						updateCamera();
						break;

					default:
						break;
				}
				break;
			
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						onMouseBtnDown();
						break;
					case SDL_BUTTON_WHEELUP:
						m_cameraZoom -= CAMERA_ZOOM_DELTA;
						updateCamera();
						break;
					case SDL_BUTTON_WHEELDOWN:
						m_cameraZoom += CAMERA_ZOOM_DELTA;
						updateCamera();
						break;
				}

				break;

			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
					case SDL_BUTTON_LEFT:
						onMouseBtnUp();
						break;
				}



				break;
		}
	}

	
	if (startedCurrentLine)
	{
		onMouseBtnHold();
	}
	

}

/*
void ZillowClone::updateZoom(bool zoomingIn)
{
	if (zoomingIn)
	{
		m_range *= 2;
		m_pipeline.ortho(-m_range, m_range, -m_range, m_range, utl::Z_NEAR, utl::Z_FAR);
	}
	else
	{
		m_range /= 2;
		m_pipeline.ortho(-m_range, m_range, -m_range, m_range, utl::Z_NEAR, utl::Z_FAR);
	}
	utl::debug("m_range ", m_range);
}
*/
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

/*
fixing the first and end point,

combine points that can do a line fit

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
	
	m_pipeline.setMatrixMode(MODEL_MATRIX);
//	m_pipeline.loadIdentity();

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);
	

	/*
	// Rendering wireframes
	p_renderer = &global.rendererMgr->r_fullVertexColor;
	p_renderer->enableShader();
		o_worldAxis.renderGroup(m_pipeline, p_renderer);
	p_renderer->disableShader();
	*/


	// Rendering wireframes
	p_renderer = &global.rendererMgr->r_fullVertexColor;

	
	p_renderer->enableShader();

	for (int i = 0; i < drawingWorldObjects.size(); i++)
	{
		WorldObject obj = drawingWorldObjects[i];
		
		if (obj.canRender())
		{
			obj.renderGroup(m_pipeline, p_renderer);
		}
	}
	p_renderer->disableShader();
	


	// Rendering wireframes
	p_renderer = &global.rendererMgr->r_fullColor;
	p_renderer->enableShader();

		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_RED);
		for (int i = 0; i < lineMarkers.size(); i++)
		{
			WorldObject obj = lineMarkers[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}


		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_YELLOW);
		for (int i = 0; i < inputPolygonLines.size(); i++)
		{
			WorldObject obj = inputPolygonLines[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}

		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_TEAL);
		for (int i = 0; i < inputPolygonPoints.size(); i++)
		{
			WorldObject obj = inputPolygonPoints[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}




		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_BLUE);

		for (int i = 0; i < polygonLines.size(); i++)
		{
			WorldObject obj = polygonLines[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}


		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_RED);
		for (int i = 0; i < polygonPoints.size(); i++)
		{
			WorldObject obj = polygonPoints[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}

		/*
		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_RED);
		for (int i = 0; i < ecTriangles.size(); i++)
		{
			WorldObject obj = ecTriangles[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}

		*/

		/*
		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_GRAY);
		for (int i = 0; i < removedEdges.size(); i++)
		{
			WorldObject obj = removedEdges[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}
		*/


		/*
		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_GRAY);
		for (int i = 0; i < insidePolygonLines.size(); i++)
		{
			WorldObject obj = insidePolygonLines[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}


		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_BLUE);
		for (int i = 0; i < outsidePolygonLines.size(); i++)
		{
			WorldObject obj = outsidePolygonLines[i];
			obj.renderGroup(m_pipeline, p_renderer);
		}
		*/
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



void ZillowClone::createModelsForDrawing(Drawing drawingIn)
{
	Model* model = new Model();

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	int counter = 0;

	float earClippingLineWidth = 0.3f;


	for (int i = 0; i < drawingIn.earclippingPolygons.size(); i++)
	{
		bool isInside = drawingIn.polygonInsideFlags[i];
		createModelsForPolyginsInDrawing(drawingIn.earclippingPolygons[i], isInside, vertices, indices);
	}

	Mesh m(vertices, indices);
	model->m_meshes.push_back(m);
//	int a = 1;
//	drawingWorldObject.setModel(model);

	WorldObject obj;
	obj.setModel(model);
	drawingWorldObjects.push_back(obj);

}


void ZillowClone::createModelsForPolyginsInDrawing(EarclippingPolygon ecPolygon, bool isInside, 
													vector<VertexData>& vertices, vector<unsigned int>& indices)
{
	// cout << "drawingIn.createModelsForPolyginsInDrawing.size() " << ecPolygon.triangles.size() << endl;
	for (int i = 0; i < ecPolygon.triangles.size(); i++)
	{
		vector<Vertex> triangle = ecPolygon.triangles[i];
		Vertex v0 = triangle[0];
		Vertex v1 = triangle[1];
		Vertex v2 = triangle[2];

		VertexData tmp;

		/*
		cout << "v0 " << v0.pos.x << " " << v0.pos.y << endl;
		cout << "v1 " << v1.pos.x << " " << v1.pos.y << endl;
		cout << "v2 " << v2.pos.x << " " << v2.pos.y << endl;
		*/

		tmp.m_position = glm::vec3(v0.pos.x, v0.pos.y, 0);
		tmp.m_color = isInside ? COLOR_LIGHT_BLUE : COLOR_WHITE;
		vertices.push_back(tmp);
		indices.push_back(indices.size());

		tmp.m_position = glm::vec3(v1.pos.x, v1.pos.y, 0);
		tmp.m_color = isInside ? COLOR_LIGHT_BLUE : COLOR_WHITE;
		vertices.push_back(tmp);
		indices.push_back(indices.size());

		tmp.m_position = glm::vec3(v2.pos.x, v2.pos.y, 0);
		tmp.m_color = isInside ? COLOR_LIGHT_BLUE : COLOR_WHITE;
		vertices.push_back(tmp);
		indices.push_back(indices.size());
	}

	/*
	for (int i = 0; i < indices.size(); i++)
	{
		cout << indices[i] << " ";
	}
	cout << endl;
	*/
}

int main(int argc, char *argv[])
{
//	RakNetFunction2();
//	return 0;

	utl::debug("Game Starting"); 
	utl::initSDL(utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT, pDisplaySurface);
	utl::initGLEW();

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


/*
void ZillowClone::tests()
{
	
}
*/


/*
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
			}
			break;

			case SDLK_SPACE:

				break;

			case SDLK_z:
				break;
			}
			break;
		}
	}
}
*/





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





