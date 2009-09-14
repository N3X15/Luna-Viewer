Check the path inside SkyDrawable::initializeSilverLining to the SilverLining resources folder. You must change this path to where you installed SilverLining, or the sample will fail to run.

This sample is based on the osgViewer sample application, and has been tested with OSG 2.4 through 2.8.1.

It does not handle the case of multiple contexts, such as full-screen windows spanning multiple monitors. If you need to handle that situation, instantiate a seperate Atmosphere class for each context, and use the appropriate one at draw time.

For more information, see the SilverLining documentation for OpenSceneGraph integration.

As always, if you run into any trouble, don't hesitate to contact support@sundog-soft.com for assistance.
