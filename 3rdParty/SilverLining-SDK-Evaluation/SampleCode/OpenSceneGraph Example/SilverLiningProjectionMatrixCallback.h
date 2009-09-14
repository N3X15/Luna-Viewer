#pragma once

#include "SilverLining.h"
#include <osg/Camera>
#include <osg/CullSettings>
#include "SkyDrawable.h"

/** This class intercepts OSG's method for automatically computing the near and far clip planes, taking into
    account SilverLining's objects in the scene (sky box, clouds, and possibly an atmospheric limb.) **/
class SilverLiningProjectionMatrixCallback : public osg::CullSettings::ClampProjectionMatrixCallback 
{
public:
    SilverLiningProjectionMatrixCallback(SilverLining::Atmosphere *atm, osg::Camera *cam) : osg::CullSettings::ClampProjectionMatrixCallback(),
        atmosphere(atm), camera(cam), skyDrawable(0), minFar(30000)
    {
        stockCullVisitor = osgUtil::CullVisitor::create();
    }

	void setSkyDrawable(SkyDrawable *sd) {skyDrawable = sd;}

protected:
    bool clampProjectionMatrixImplementation(osg::Matrixf& projection, 
        double& znear, double& zfar) const
    {
       double cloudNear, cloudFar;
       computeNearFarClouds(cloudNear, cloudFar);
       if (cloudNear < znear) znear = cloudNear;
       if (cloudFar > zfar) zfar = cloudFar;
	   if (zfar < minFar) zfar = minFar; 

       bool ret = stockCullVisitor->clampProjectionMatrixImplementation(projection, znear, zfar);

	   if (skyDrawable)
	   {
			skyDrawable->setSkyboxSize(((zfar - znear) * 0.5 + znear) * 2.0);
	   }

       return ret;
    }

    bool clampProjectionMatrixImplementation(osg::Matrixd& projection, 
        double& znear, double& zfar) const
    {
       double cloudNear, cloudFar;
       computeNearFarClouds(cloudNear, cloudFar);
       if (cloudNear < znear) znear = cloudNear;
       if (cloudFar > zfar) zfar = cloudFar;
	   if (zfar < minFar) zfar = minFar;

       bool ret = stockCullVisitor->clampProjectionMatrixImplementation(projection, znear, zfar);

	   if (skyDrawable)
	   {
	       skyDrawable->setSkyboxSize(((zfar - znear) * 0.5 + znear) * 2.0);
	   }

       return ret;
    }

    void computeNearFarClouds(double &znear, double &zfar) const
    {
        const double fudge = 5000.0;
        double minDist = 1E10; double maxDist = -1E10;
        std::vector<SilverLining::ObjectHandle> &objects = atmosphere->GetObjects();

        osg::Vec3d camPos, dirVector, upVector;
        camera->getViewMatrixAsLookAt(camPos, dirVector, upVector); 

        osg::Vec3d up = camPos;
        up.normalize();
        osg::Vec3d down = up * -1.0;

		double dToOrigin = camPos.length();

		bool hasLimb = atmosphere->GetConfigOptionBoolean("enable-atmosphere-from-space");
		if (hasLimb)
		{
			// Compute distance to edge of atmospheric limb
			double earthRadius = atmosphere->GetConfigOptionDouble("earth-radius-meters");
			double atmosphereHeight = earthRadius +
				+ atmosphere->GetConfigOptionDouble("atmosphere-height");
			double atmosphereThickness = atmosphere->GetConfigOptionDouble("atmosphere-scale-height-meters")
				+ earthRadius;
			if (dToOrigin > atmosphereThickness) // Bail if limb not rendered
			{
				double dToLimbEdge = sqrt(dToOrigin * dToOrigin + atmosphereHeight * atmosphereHeight);
				maxDist = dToLimbEdge;
				double dToLimbBase = sqrt(dToOrigin * dToOrigin + earthRadius * earthRadius);
				minDist = dToLimbBase;
			}
		}

        std::vector<SilverLining::ObjectHandle>::iterator it;
        for (it = objects.begin(); it != objects.end(); it++)
        {
            float ox, oy, oz;
            atmosphere->GetObjectPosition(*it, ox, oy, oz);
            osg::Vec3d oPos(ox, oy, oz);
            double oHeight = oPos.length();
            osg::Vec3d testVec;
            if (dToOrigin > oHeight)
            {
                testVec = camPos + down * 100.0; // 100 for floating point precision problems
            }
            else
            {
                testVec = camPos + up * 100.0;
            }
            
            const double farFarAway = 500000;

            double dist = atmosphere->GetObjectDistance(*it, testVec.x(), testVec.y(), testVec.z(),
                (float)camPos.x(), (float)camPos.y(), (float)camPos.z());

            if (dist < farFarAway) // an intersection was found
            {
                if (dist < minDist) minDist = dist;
                if (dist > maxDist) maxDist = dist;
            }
        }

        double fovy, ar, zn, zf;
        camera->getProjectionMatrixAsPerspective(fovy, ar, zn, zf);
        double halfFovx = (fovy * ar) * 0.5;
        minDist *= cos(halfFovx * (3.14159265 / 180.0));

        znear = minDist - fudge;
        zfar = maxDist + fudge;
    }

    osgUtil::CullVisitor *stockCullVisitor;
    SilverLining::Atmosphere *atmosphere;
    osg::Camera *camera;
	SkyDrawable *skyDrawable;
	double minFar;
};
