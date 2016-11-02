#include "3DEngineInterface.h"

#include "Managers.h"

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

#include "VisibilityManager.h"
#include "ResourceManager.h"

#include <random>
#include <string>

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

namespace
{
	const float g_worldRadius = 300.f;

	const float _minCellSize = 20.f;

	const float _nearPlane = 1.f;
	const float _farPlane = 100.f;

	const float _horizontalFOV = 75.f;
	const float _verticalFOV = 75.f;


	CVisibilityManager*	g_visibilityManager = nullptr;
	CResourceManager*	g_resourceManager = nullptr;

};

class SceneViewer : public osgViewer::Viewer
{
public:


	virtual void frame(double simulationTime = USE_REFERENCE_TIME)
	{
		ObjectManager::Instance().onBeginFrame();

		osg::Vec3 eye, center, up, dir;
		//getCamera()->getViewMatrixAsLookAt(eye, center, up);

		osg::Matrix mView = getCamera()->getViewMatrix();

		mView.getLookAt(eye, center, up);

		dir = center - eye;

		Vector3 vCamPos(eye.x(), eye.y(), eye.z());
		Vector3 vCamDir(dir.x(), dir.y(), dir.z());
		Vector3 vCamUp(up.x(), up.y(), up.z());

		g_visibilityManager->SetCamera(vCamPos, vCamDir, vCamUp, _horizontalFOV, _verticalFOV, _nearPlane, _farPlane);
		g_visibilityManager->UpdateVisibleObjectsSet();

		double curTime = getFrameStamp()->getReferenceTime();

		g_resourceManager->Update((float)(curTime - _prevTime));


		osgViewer::ViewerBase::frame(simulationTime);

		_prevTime = curTime;
	}


private:

	double				_prevTime = 0;
};

void initVisibilityAndResourceManagers()
{
	g_visibilityManager = new CVisibilityManager(&ObjectManager::Instance(), g_worldRadius*100, _minCellSize);
	g_resourceManager = new CResourceManager();

	g_resourceManager->AddVisibilityManager(g_visibilityManager);

}

void destroyVisibilityAndResourceManagers()
{
	delete g_visibilityManager;
	delete g_resourceManager;
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
	osg::Node* sceneRoot = osgDB::readNodeFile(std::string("../data/scenes/medieval_city/Medieval_City.osgb"));
	ObjectManager::Instance().generateObjectsFromOsgNode(sceneRoot);
}

int main(int, char**)
{
	// construct the viewer
	SceneViewer viewer;

	//generateTestScene();

	loadTestSceneFile();

	initVisibilityAndResourceManagers();

	//osg::Matrix coordinateSwitch;
	//coordinateSwitch(2, 2) = 1;

	osg::ref_ptr<osg::MatrixTransform> rootTransform = new osg::MatrixTransform();

	rootTransform->addChild(ObjectManager::Instance().getOsgRoot());

	// set the scene to render
	viewer.setSceneData(rootTransform);


	// установить манипулятор камеры
	osgGA::TrackballManipulator* cameraManipulator = new osgGA::TrackballManipulator;
	cameraManipulator->setAllowThrow(false);
	cameraManipulator->setAutoComputeHomePosition(false);
	cameraManipulator->setHomePosition(osg::Vec3d(7, 50, 0), osg::Vec3d(0, 50, 0), osg::Vec3d(0, 0, 1));
	cameraManipulator->setMinimumDistance(0.5, false);

	/*osgGA::FlightManipulator* flightManipulator = new osgGA::FlightManipulator;
	flightManipulator->setHomePosition(osg::Vec3d(7, 50, 0), osg::Vec3d(0, 50, 0), osg::Vec3d(0, 0, 1));*/

	viewer.setCameraManipulator(cameraManipulator);
	viewer.setUpViewInWindow(100, 100, 800, 600);

	// run the viewers frame loop
	int iResult = viewer.run();

	destroyVisibilityAndResourceManagers();

	return iResult;
}
