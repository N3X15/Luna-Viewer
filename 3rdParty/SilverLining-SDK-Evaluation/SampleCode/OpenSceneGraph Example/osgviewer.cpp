/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This application is open source and may be redistributed and/or modified
 * freely and without restriction, both in commericial and non commericial applications,
 * as long as this copyright notice is maintained.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include "SilverLining.h"
#include "SkyDrawable.h"
#include "CloudsDrawable.h"
#include "AtmosphereReference.h"
#include "SilverLiningProjectionMatrixCallback.h"

#include <iostream>

void integrateSilverLining(osg::ref_ptr<osg::Node> sceneGraphRoot, osgViewer::Viewer& viewer)
{
    // No need for OSG to clear the color buffer, the sky will fill it for you.
    viewer.getCamera()->setClearMask(0);

	// Instantiate an Atmosphere and associate it with this camera. If you have multiple cameras
	// in multiple contexts, be sure to instantiate seperate Atmosphere objects for each.
	SilverLining::Atmosphere *atm = new SilverLining::Atmosphere("Your user name", "Your license code");

    // Add the sky (calls Atmosphere::BeginFrame and handles initialization once you're in
    // the rendering thread)
    osg::Geode *skyGeode = new osg::Geode;
	SkyDrawable *skyDrawable = new SkyDrawable(&viewer);

    // ***IMPORTANT!**** Check that the path to the resources folder for SilverLining in SkyDrawable.cpp
    // SkyDrawable::initializeSilverLining matches with where you installed SilverLining.

    skyGeode->addDrawable(skyDrawable);
    skyGeode->setCullingActive(false);

    /* If you let OSG auto-calculate the near and far clip planes; it'll exclude the
       sky box and clouds. One solution is to set the near and far clip planes explicitly
	   like this:
    */
/*
    viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    double fovy, aspect, zNear, zFar;
    viewer.getCamera()->getProjectionMatrixAsPerspective(fovy, aspect, zNear, zFar);
    viewer.getCamera()->setProjectionMatrixAsPerspective(fovy, aspect, 10, 100000);
*/
 /*
       or, you can use the included projection matrix callback to intercept how OSG computes
	   the near and far clip planes and take SilverLining's objects into account, like this:
 */

	SilverLiningProjectionMatrixCallback *cb = new SilverLiningProjectionMatrixCallback(
		atm, viewer.getCamera());
    viewer.getCamera()->setClampProjectionMatrixCallback(cb);
    cb->setSkyDrawable(skyDrawable);
    

	AtmosphereReference *ar = new AtmosphereReference;
	ar->atmosphere = atm;
	viewer.getCamera()->setUserData(ar);

    // Use a RenderBin to enforce that the sky gets drawn first, then the scene, then the clouds
    skyGeode->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");

    // Add the models
    sceneGraphRoot.get()->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

    // Add the clouds (note, you need this even if you don't want clouds - it calls
    // Atmosphere::EndFrame() )
    osg::Geode *cloudsGeode = new osg::Geode;
    cloudsGeode->addDrawable(new CloudsDrawable(&viewer));
    cloudsGeode->getOrCreateStateSet()->setRenderBinDetails(99, "RenderBin");
    cloudsGeode->setCullingActive(false);

    viewer.getSceneData()->asGroup()->addChild(skyGeode);
    viewer.getSceneData()->asGroup()->addChild(cloudsGeode);
}

int main(int argc, char** argv)
{
    // Our code doesn't handle multiple displays; you need to associate an Atmosphere with each context in
    // that case. So, we'll force a single window / context.
    putenv("OSG_WINDOW=20 30 1024 768");

    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" is the standard OpenSceneGraph example which loads and visualises 3d models.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
    arguments.getApplicationUsage()->addCommandLineOption("--image <filename>","Load an image and render it on a quad");
    arguments.getApplicationUsage()->addCommandLineOption("--dem <filename>","Load an image/DEM and render it on a HeightField");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display command line parameters");
    arguments.getApplicationUsage()->addCommandLineOption("--help-env","Display environmental variables available");
    arguments.getApplicationUsage()->addCommandLineOption("--help-keys","Display keyboard & mouse bindings available");
    arguments.getApplicationUsage()->addCommandLineOption("--help-all","Display all command line, env vars and keyboard & mouse bindings.");
    arguments.getApplicationUsage()->addCommandLineOption("--SingleThreaded","Select SingleThreaded threading model for viewer.");
    arguments.getApplicationUsage()->addCommandLineOption("--CullDrawThreadPerContext","Select CullDrawThreadPerContext threading model for viewer.");
    arguments.getApplicationUsage()->addCommandLineOption("--DrawThreadPerContext","Select DrawThreadPerContext threading model for viewer.");
    arguments.getApplicationUsage()->addCommandLineOption("--CullThreadPerCameraDrawThreadPerContext","Select CullThreadPerCameraDrawThreadPerContext threading model for viewer.");

    // if user request help write it out to cout.
    bool helpAll = arguments.read("--help-all");
    unsigned int helpType = ((helpAll || arguments.read("-h") || arguments.read("--help"))? osg::ApplicationUsage::COMMAND_LINE_OPTION : 0 ) |
                            ((helpAll ||  arguments.read("--help-env"))? osg::ApplicationUsage::ENVIRONMENTAL_VARIABLE : 0 ) |
                            ((helpAll ||  arguments.read("--help-keys"))? osg::ApplicationUsage::KEYBOARD_MOUSE_BINDING : 0 );
    if (helpType)
    {
        arguments.getApplicationUsage()->write(std::cout, helpType);
        return 1;
    }

    osgViewer::Viewer viewer(arguments);

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    if (arguments.argc()<=1)
    {
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }

    // set up the camera manipulators.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
        keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
        keyswitchManipulator->addMatrixManipulator( '3', "Drive", new osgGA::DriveManipulator() );
        keyswitchManipulator->addMatrixManipulator( '4', "Terrain", new osgGA::TerrainManipulator() );

        std::string pathfile;
        char keyForAnimationPath = '5';
        while (arguments.read("-p",pathfile))
        {
            osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(pathfile);
            if (apm || !apm->valid())
            {
                unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                keyswitchManipulator->addMatrixManipulator( keyForAnimationPath, "Path", apm );
                keyswitchManipulator->selectMatrixManipulator(num);
                ++keyForAnimationPath;
            }
        }

        viewer.setCameraManipulator( keyswitchManipulator.get() );
    }

    // add the state manipulator
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );

    // add the thread model handler
    viewer.addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    viewer.addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    // add the record camera path handler
    viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    viewer.addEventHandler(new osgViewer::LODScaleHandler);

    // load the data
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
        std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
        return 1;
    }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    viewer.setSceneData( loadedModel.get() );

	integrateSilverLining(loadedModel, viewer);

    viewer.realize();

    return viewer.run();

}
