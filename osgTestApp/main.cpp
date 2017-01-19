#include "3DEngineInterface.h"

#include "Managers.h"

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>

#include "VisibilityManager.h"
#include "ResourceManager.h"

#include <random>
#include <string>

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgViewer/CompositeViewer>

#include <osgGA/GUIEventHandler>
#include <osg/PolygonMode>

#include <math.h>

#define D2R 0.0174532925f

namespace
{
	const float g_nearPlane = 1.f;
	const float g_farPlane = 1000.f;

	const float g_horizontalFOV = 75.f;
	const float g_verticalFOV = 75.f;
};


class CameraDebugDraw
{
public:

	CameraDebugDraw()
	{
		createReferenceNode();
		buildGeometry();
	}

	osg::Node*	getRefNode()
	{
		return _referenceNode;
	}

	void setCamera(const osg::Vec3& pos, const osg::Vec3& dir, const osg::Vec3& up)
	{
		_cameraPos = pos;
		_cameraDir = dir;
		_cameraUp = up;

		rebuildVertexArrays();
		_geo->dirtyDisplayList();
	}

private:

	void createReferenceNode()
	{
		_referenceNode = new osg::Geode;
	}

	void rebuildVertexArrays()
	{
		if (!_points.get())
		{
			_points = new osg::Vec3Array;
			_points->resize(18);
		}

		// @{ Frustum 
		
		// center line
		(*_points)[0] = _cameraPos;
		(*_points)[1] = _cameraPos + _cameraDir * g_farPlane;

		osg::Vec3 left = _cameraUp ^ _cameraDir;

		osg::Vec3 leftSide = _cameraPos + _cameraDir * g_farPlane + left * (g_farPlane * sinf(D2R * g_horizontalFOV / 2));
		osg::Vec3 rightSide = _cameraPos + _cameraDir * g_farPlane - left * (g_farPlane * sinf(D2R * g_horizontalFOV / 2));

		osg::Vec3 topSide = _cameraPos + _cameraDir * g_farPlane + _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));
		osg::Vec3 bottomSide = _cameraPos + _cameraDir * g_farPlane - _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));

		osg::Vec3 leftTop = leftSide + _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));
		osg::Vec3 rightTop = rightSide + _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));
		osg::Vec3 leftBottom = leftSide - _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));
		osg::Vec3 rightBottom = rightSide - _cameraUp * (g_farPlane * sinf(D2R * g_verticalFOV / 2));

		//cross
		(*_points)[2] = leftSide;
		(*_points)[3] = rightSide;

		(*_points)[4] = topSide;
		(*_points)[5] = bottomSide;

		// side lines
		(*_points)[6] = _cameraPos;
		(*_points)[7] = rightSide;

		(*_points)[8] = _cameraPos;
		(*_points)[9] = bottomSide;

		(*_points)[10] = _cameraPos;
		(*_points)[11] = leftSide;

		(*_points)[12] = _cameraPos;
		(*_points)[13] = topSide;

		// square
		(*_points)[6] = _cameraPos;
		(*_points)[7] = leftTop;

		(*_points)[8] = _cameraPos;
		(*_points)[9] = rightTop;

		(*_points)[10] = _cameraPos;
		(*_points)[11] = leftBottom;

		(*_points)[12] = _cameraPos;
		(*_points)[13] = rightBottom;

		//@}

	}

	void buildGeometry()
	{
		rebuildVertexArrays();

		_geo = new osg::Geometry;

		osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;
		color->push_back(osg::Vec4(1.0, 1.0, 0.2, 1.0));

		_geo->setVertexArray(_points.get());
		_geo->setColorArray(color.get());
		_geo->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
		_geo->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, _points->size()));

		osg::PolygonMode * polygonMode = new osg::PolygonMode;
		polygonMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

		_geo->getOrCreateStateSet()->setAttributeAndModes(polygonMode,
			osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

		_geo->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		_referenceNode->addDrawable(_geo);
	}

	osg::ref_ptr<osg::Geode>		_referenceNode;
	osg::ref_ptr<osg::Vec3Array>	_points;
	osg::ref_ptr<osg::Geometry>		_geo;

	osg::Vec3						_cameraPos = osg::Vec3(0, 0, 0);
	osg::Vec3						_cameraDir = osg::Vec3(0, 0, 1);
	osg::Vec3						_cameraUp = osg::Vec3(0, 1, 0);
};

namespace
{
	const float g_worldRadius = 1000000.f;

	const float g_minCellSize = 20.f;

	const float g_slowMoveSpeed = 100;
	const float g_fastMoveSpeed = 1000;


	CVisibilityManager*	g_visibilityManager = nullptr;
	CResourceManager*	g_resourceManager = nullptr;
	osgViewer::Viewer*	g_mainViewer = nullptr;

	CameraDebugDraw*	g_DebugDraw = nullptr;
};



class MyCompositeViewer : public osgViewer::CompositeViewer
{
public:


	virtual void advance(double simulationTime = USE_REFERENCE_TIME)
	{
		if (!g_mainViewer)
			return;

		ObjectManager::Instance().onBeginFrame();

		osg::Vec3 eye, center, up, dir;
		//getCamera()->getViewMatrixAsLookAt(eye, center, up);

		osg::Matrix mView = g_mainViewer->getCamera()->getViewMatrix();

		mView.getLookAt(eye, center, up);

		dir = center - eye;

		Vector3 vCamPos(eye.x(), eye.y(), eye.z());
		Vector3 vCamDir(dir.x(), dir.y(), dir.z());
		Vector3 vCamUp(up.x(), up.y(), up.z());

		g_visibilityManager->SetCamera(vCamPos, vCamDir, vCamUp, g_horizontalFOV, g_verticalFOV, g_nearPlane, g_farPlane);
		g_visibilityManager->UpdateVisibleObjectsSet();

		double curTime = getFrameStamp()->getReferenceTime();
		_deltaTime = (curTime - _prevTime);

		g_resourceManager->Update((float)_deltaTime);

		g_DebugDraw->setCamera(eye, dir, up);

		//@{ move camera
		osg::Vec3 manCenter = _cameraManipulator->getCenter();

		if (_moveForward)
			manCenter += dir*_deltaTime*(_fastMove ? g_fastMoveSpeed : g_slowMoveSpeed);

		if (_moveBack)
			manCenter -= dir*_deltaTime*(_fastMove ? g_fastMoveSpeed : g_slowMoveSpeed);

		_cameraManipulator->setCenter(manCenter);
		//@}

		osgViewer::CompositeViewer::advance(simulationTime);

		_prevTime = curTime;
	}


	void setControlledManipulator(osgGA::TrackballManipulator* cameraManipulator, osg::Camera* operatingCamera)
	{
		_cameraManipulator = cameraManipulator;
		_operatingCamera = operatingCamera;
	}

	void moveForward(bool move)
	{
		_moveForward = move;
	}

	void moveBackward(bool move)
	{
		_moveBack = move;
	}

	void moveLeft(bool move)
	{

	}

	void moveRight(bool Move)
	{

	}

	void fastMove(bool fast)
	{
		_fastMove = fast;
	}

private:

	double				_prevTime = 0;
	osgGA::TrackballManipulator* _cameraManipulator = nullptr;
	osg::Camera*		_operatingCamera = nullptr;

	double				_deltaTime = 0;

	bool				_moveForward = false, _moveBack = false, _moveLeft = false, _moveRight = false, _fastMove = false;
};


void initVisibilityAndResourceManagers()
{
	g_visibilityManager = new CVisibilityManager(&ObjectManager::Instance(), g_worldRadius, g_minCellSize);
	g_resourceManager = new CResourceManager();

	g_resourceManager->AddVisibilityManager(g_visibilityManager);

	g_DebugDraw = new CameraDebugDraw;
}

void destroyVisibilityAndResourceManagers()
{
	delete g_visibilityManager;
	delete g_resourceManager;

	delete g_DebugDraw;
}

void generateTestScene()
{
	size_t NumObjects = 30000;

	std::default_random_engine generator;
	std::uniform_real_distribution<float> posDistribution(-g_worldRadius, g_worldRadius);

	ObjectManager::Instance().generateTestObject(osg::Vec3(0, 0, 0));

	for (size_t i = 0; i < NumObjects; i++)
	{
		float posX = posDistribution(generator);
		float posY = posDistribution(generator);
		float posZ = 10;

		ObjectManager::Instance().generateTestObject(osg::Vec3(posX, posY, posZ));
	}
}

void loadTestSceneFile()
{
	//osg::Node* sceneRoot = osgDB::readNodeFile(std::string("../data/scenes/DamagedDowntown/Downtown_Damage_0.obj"));
	//osg::Node* sceneRoot = osgDB::readNodeFile(std::string("../data/scenes/medieval_city/Medieval_City.osgb"));
	osg::Node* sceneRoot = osgDB::readNodeFile(std::string("../data/spheres.3ds"));
	//osg::Node* sceneRoot = osgDB::readNodeFile(std::string("../data/scenes/compas.obj"));
	ObjectManager::Instance().generateObjectsFromOsgNode(sceneRoot);
}

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:

	KeyboardEventHandler(MyCompositeViewer* owner) : _owner(owner) {}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&);

private:
	MyCompositeViewer*	_owner = nullptr;
};

bool KeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

	switch (ea.getKey())
	{

	case 'w':
		_owner->moveForward(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN);
		return true;
	
	case 's':
		_owner->moveBackward(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN);
		return true;

	case osgGA::GUIEventAdapter::KEY_Shift_L:
		_owner->fastMove(ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN);
		return true;

	}

	return false;
}

int main(int, char**)
{
	

	MyCompositeViewer compositeViewer;

	//-------------------
	//@{ MainView
	osgViewer::Viewer* mainView = new osgViewer::Viewer;
	g_mainViewer = mainView;

	//generateTestScene();

	loadTestSceneFile();

	initVisibilityAndResourceManagers();

	osg::ref_ptr<osg::MatrixTransform> rootTransform = new osg::MatrixTransform();

	rootTransform->addChild(ObjectManager::Instance().getOsgRoot());


	// set the scene to render
	mainView->setSceneData(rootTransform);

	mainView->addEventHandler(new KeyboardEventHandler(&compositeViewer));


	// установить манипулятор камеры
	osgGA::TrackballManipulator* cameraManipulator = new osgGA::TrackballManipulator;
	cameraManipulator->setAllowThrow(false);
	cameraManipulator->setAutoComputeHomePosition(false);
	cameraManipulator->setHomePosition(osg::Vec3d(7, 0, 100), osg::Vec3d(0, 0, 100), osg::Vec3d(0, 0, 1));
	cameraManipulator->setMinimumDistance(0.5, false);

	//osgGA::FlightManipulator* flightManipulator = new osgGA::FlightManipulator;
	//flightManipulator->setHomePosition(osg::Vec3d(7, 50, 0), osg::Vec3d(0, 50, 0), osg::Vec3d(0, 0, 1));

	mainView->setCameraManipulator(cameraManipulator);
	mainView->setUpViewInWindow(100, 100, 800, 600);

	compositeViewer.addView(mainView);
	compositeViewer.setControlledManipulator(cameraManipulator, mainView->getCamera());
	//@} MainView


	osg::ref_ptr<osg::MatrixTransform> debugRootTransform = new osg::MatrixTransform();

	debugRootTransform->addChild(ObjectManager::Instance().getOsgRoot());
	debugRootTransform->addChild(g_DebugDraw->getRefNode());


	//-------------------
	//@{ DebugView
	osgViewer::Viewer* debugView = new osgViewer::Viewer;

	debugView->setSceneData(debugRootTransform);

	osgGA::TrackballManipulator* debugCameraManipulator = new osgGA::TrackballManipulator;
	debugCameraManipulator->setAllowThrow(false);
	debugCameraManipulator->setAutoComputeHomePosition(false);
	debugCameraManipulator->setHomePosition(osg::Vec3d(7, 0, 100), osg::Vec3d(0, 0, 100), osg::Vec3d(0, 0, 1));
	debugCameraManipulator->setMinimumDistance(0.5, false);


	debugView->setCameraManipulator(debugCameraManipulator);
	debugView->setUpViewInWindow(900, 100, 800, 600);


	compositeViewer.addView(debugView);
	//@}

	compositeViewer.run();

	destroyVisibilityAndResourceManagers();

	return 0;
}
