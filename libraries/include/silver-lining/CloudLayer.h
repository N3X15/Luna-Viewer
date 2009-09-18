// Copyright (c) 2004-2008  Sundog Software, LLC. All rights reserved worldwide.

/** \file CloudLayer.h
	\brief Methods for configuration and initializing cloud decks.
*/

#ifndef CLOUDLAYER_H
#define CLOUDLAYER_H

#include "Atmosphere.h"
#include <vector>
#include <map>
#include <stdio.h>

namespace SilverLining
{
class Cloud;
class Frustum;
class Color;
class Vector3;
class Sky;
class LightningListener;

/** A collection of clouds of uniform classification organized into a "deck" of similar altitudes.

	CloudLayer is a bse class that cannot be directly instantiated; instead, it is returned by
	the class factory CloudLayerFactory. Call CloudLayerFactory::Create() to obtain a CloudLayer object
	for a specific cloud type.

	Once created, you'll want to set the CloudLayer's dimensions and density (coverage) using the
	SetBaseWidth(), SetBaseLength(), SetBaseAltitude(), SetThickness(), and SetDensity() methods.
	You will also need to position it using SetLayerPosition().

	Initially, a CloudLayer will not contain any clouds. Once specifying your preferences for the
	layer's dimensions and density, you must then call SeedClouds() to simulate cloud growth within
	the cloud deck. 

	Once your CloudLayer object is created, configured, and seeded, you may then pass it into
	AtmosphericConditions::AddCloudLayer() to add it to your scene.

    Once "seeded", the above methods to modify the layer's dimensions and density
    will have no effect - they are burned in at that point. You may, however, call SetLayerPosition()
    at runtime. To change the other properties of the cloud layer at runtime, you need to remove
    the cloud layer and re-create it.

	See SetBaseWidth() for information on important differences between different types of
	CloudLayers.
*/
class CloudLayer
{
public:
    /** Default constructor. This is a virtual base class, so this constructor is not directly
    invoked by your application. CloudLayer objects are instantiated by calling
    CloudLayerFactory::Create().
    */
    CloudLayer();

    /** Destructor. Removes and deletes all clouds added to this CloudLayer. */
    virtual ~CloudLayer();

    /** Enable rendering of this cloud layer.

    	New cloud layers are enabled by default.

    	Why would you want to explicitly enable and disable a cloud layer? Well, creating and
    	seeding cloud layers - especially cumulus cloud layers - is computationally expensive
    	and can take up to a second or two. If your application requires several cloud layers
    	that are displayed at different times, it may make sense to create them all when your
    	application initializes, and then just enable and disable them as needed, which is a
    	much faster operation.

        Optionally, you may fade cloud layers in or out over time to smoothly introduce them
        into your scene. Specify fadeTimeMS in order to enable this effect when enabling or
        disabling a cloud layer. Fading effects on cumulus layers are only supported on systems 
        that support vertex shaders.

    	\param enabled True if this cloud layer should be rendered during Atmosphere::EndFrame(),
    	               false if it should not be rendered.
		\param fadeTimeMS The amount of time, in milliseconds, for the cloud layer to fade in
					      or out.
    */
    void   SetEnabled(bool enabled, unsigned long fadeTimeMS = 0);

    /** Accesses if the CloudLayer is currently enabled for rendering.

    	\sa SetEnabled()
    */
    bool   GetEnabled() const						{
        return layerEnabled;
    }

    /** Sets the size of the CloudLayer along the X axis. The cloud layer must be at least as
    	big as the size of individual clouds within it. The size of the cloud layer may have
    	important performance and visual impacts, so it should be chosen with care. Here are some
    	general guidelines for setting the cloud layer dimensions:

    	\li Cumulus congestus and mediocris: The number of clouds created is proportional to the
    	square of the layer dimensions, so performance will degrade quickly if unreasonably high
    	dimensions are chosen. A balance must be found between performance (not having
    	too many clouds to render at real-time speed) and visual acceptability (having enough
    	clouds to fill the scene.) Dimensions of around 30 km or so generally fulfill both demands,
    	but the number of clouds also depends on the cloud layer density (see SetDensity()).

    	\li Stratus: Stratus decks extend to the horizon, and the width and length specified are
    	actually ignored.

    	\li Cumulonimbus: Due to the large size of cumulonimbus clouds and their isolated organization
    	along "squall lines", it's not realistic to have a rectangular deck of cumulonimbus clouds.
    	Instead, the size of the cumulonimbus cloud layer will reflect the size of a single
    	cumulonimbus cloud that fills the entire layer. Choose dimensions that reflect the size of
    	the cloud itself, typically on the order of 5 km.

    	\li Cirrus: Cirrus decks are rendered as a high, textured plane, but it does not extend
    	infinitely like stratus decks do. A large enough dimension should be chosen to fill the
    	sky of your scene at the deck's altitude; typically, 100,000 meters is appropriate.
    */
    void   SetBaseWidth(double meters)              {
        baseWidth = meters;
    }

    /** Retrieves the dimension of the CloudLayer along the X axis.
    	\sa SetBaseWidth()
    */
    double GetBaseWidth() const                     {
        return baseWidth;
    }

    /** Sets the size of the CloudLayer along the Z axis. See SetBaseWidth() for guidance on
    appropriate values for different types of CloudLayers. */
    void   SetBaseLength(double meters)             {
        baseLength = meters;
    }

    /** Retrieves the dimension of the CloudLayer along the Z axis.
    	\sa SetBaseLength()
    */
    double GetBaseLength() const                    {
        return baseLength;
    }

    /** Sets the altitude above sea level of the bottom of the CloudLayer. Here's some guidance
    	on realistic settings (in meters) for various CloudLayer types:

    	\li Cumulus congestus, cumulus mediocris: Typically found fairly low; between 1000-2000m.
    	\li Cumulonimbus: Generally extends to the ground if it's raining. 400m is an appropriate setting.
    	\li Stratus: Found very low; typically 500-2000 m.
    	\li Cirrus: Found very high; 6000 m or higher.
    */
    void   SetBaseAltitude(double meters)           {
        baseAltitude = meters;
    }

    /** Retrieves the altitude above sea level of the CloudLayer botton in meters. */
    double GetBaseAltitude() const                  {
        return baseAltitude;
    }

    /** Sets the thickness of the CloudLayer in meters. "Thickness" has different meanings for
    	different CloudLayer types:

    	\li Cumulus congestus, mediocris: The thickness specifies the variance in the altitude of
    	the bottoms of each individual cloud within the CloudLayer. The actual height of each cloud
    	is determined procedurally as part of the cloud growth model, and is not affected by
    	SetThickness(). So, setting the thickness to 0 would result in a layer of clouds that all
    	start at the same base altitude; setting a thickness of 100 would provide for some variance
    	for increased realism. In reality, cloud layers rest on top of an air mass that cause most
    	of the clouds to have flattened bottoms that start at roughly the same altitude.

    	\li Cumulonimbus: The CloudLayer dimensions specify the dimension of the single cloud within it,
    	in this case. SetThickness() will specify the height of the cumulonimbus tower. 3000m would
    	be an example of a resonable value to set.

    	\li Stratus: The thickness in this case specifies the actual thickness of the stratus deck,
    	that is, the distance between the top and bottom of the stratus layer. Stratus clouds vary
    	in thickness; 1000-2000 m are reasonable values. The thickness of the stratus deck will not
    	impact performance, unlike cumulus decks.

    	\li Cirrus: The thickness is ignored for cirrus decks; cirrus decks are modeled as infinitely
    	thin layers of ice crystals at high altitude.
    */
    void   SetThickness(double meters)              {
        thickness = meters;
    }

    /** Retrieves the thickness of the CloudLayer in meters. See SetThickness() for descriptions of
    	what this value represents for various cloud types. */
    double GetThickness() const                     {
        return thickness;
    }

    /** Sets the percentage of the sky that should be covered by this CloudLayer, from 0 - 1.0.
    	For cumulus congestus and cumulus mediocris decks, this will affect the number of clouds
    	generated during SeedClouds(). For stratus decks, setting a value less than 1.0 will result
    	in a "broken" cloud deck, which results in extremely high performance characteristics for
    	variable cloud clover, but is much less visually realistic than a cumulus congestus deck.
    	This value is ignored for cirrus and cumulonimbus cloud decks, which always represent a
    	single cloud.

    	\param coveragePercentage The amount of the sky that should be covered by clouds, from 0 - 1.0.
    */
    void   SetDensity(double coveragePercentage)    {
        density = coveragePercentage;
    }

    /** Retrieves the cloud coverage set for this CloudLayer.

    	\return The cloud coverage of this deck from 0 - 1.0.
    	\sa SetDensity()
    */
    double GetDensity() const                       {
        return density;
    }

    /** Specifies the location of the center of the CloudLayer. Sets the "East" and "South" coordinates
    	of the CloudLayer's center position. For a right-handed coordinate system where Z points in,
    	you'd pass in the X and Z coordinates of where you want the cloudlayer to be centered over,
    	for example. A right-handed coordinate system where Z points up would pass in X and -Y instead.
    	Cloud positions may move over time in response to
    	simulated wind (see AtmosphericConditions::SetWind()). The "up" coordinates are determined
    	by SetBaseAltitude().

        If you are using a custom set of basis vectors, be sure that you have called
        Atmosphere::SetUpVector() and Atmosphere::SetRightVector() before calling this method.

    	Calling SetLayerPosition will reset any accumulated wind drift as well.

        SetLayerPosition() is the one method on a CloudLayer that you may change at runtime, after
        the CloudLayer has been seeded and added into the scene. It will also pick up any
        changes you have made to the cloud's base altitude. Modifying other properties of the
        CloudLayer after it's been seeded require removing the CloudLayer from your
        AtmosphericConditions, and re-creating the layer.

    	\param eastCoord The east coordinate of the CloudLayer's center position, in meters.
    	\param southCoord The south coordinate of the CloudLayer's center position, in meters.
    */
    void   SetLayerPosition(double eastCoord, double southCoord);

    /** Retrieves the east and south coordinates of the CloudLayer's center, in meters.

    	\sa SetLayerPosition()
    */
    void   GetLayerPosition(double& east, double& south) const   {
        east = layerX;
        south = layerZ;
    }

    enum PrecipitationTypes
    {
        NONE,
        RAIN,
        DRY_SNOW,
        WET_SNOW,
        SLEET,
        NUM_PRECIP_TYPES
    };

    /** Simulates precipitation under this cloud layer of a specified type. Any time the camera is under
    	a cloud of this layer, precipitation effects will display if the type is set to something other than
    	NONE.

        Note, you may also set global precipitation effects that are not associated with a CloudLayer
        with the similar AtmosphericConditions::SetPrecipitation() method.

        For mixed precipitation, you may call SetPrecipitation multiple times with different precipitation
        types. To clear all precipitation, call SetPrecipitation with a type of NONE. If you call this method
        multiple times for the same precipitation type, the intensity specified will overwrite the intensity
        previously specified for that type.

    	\param type The type of precipitation to simulate under this cloud layer - NONE, RAIN, WET_SNOW,
                    DRY_SNOW, or SLEET.
    	\param intensity The simulated rate of precipitation, in millimeters per hour. Reasonable ranges
    	might be between 1 for light rain or 20 for heavier rain. This value will be clamped to the value
    	specified by rain-max-intensity, snow-max-intensity, or sleet-max-intensity in 
        resources/SilverLining.config, which is 30 by default.

    	 \sa HasPrecipitationAtPosition()
         \sa GetPrecipitation()
    */
    void   SetPrecipitation(int type, double intensity);

    /** Retrieves a map of precipitation types to intensities in millimeters per hour that are
        associated with this cloud layer. Precipitation types include RAIN, WET_SNOW, DRY_SNOW,
        or SLEET.

        \sa SetPrecipitation()
        \sa HasPrecipitationAtPosition()
    */
    const std::map<int, double>& GetPrecipitation() const {return precipitationEffects;}

    /** Returns whether a precipitation type other than NONE will be simulated at the given camera position.
    	If you're under a cloud and precipitation has been assigned to this cloud layer using
    	SetPrecipitation(), this will return true. The specific effect may be retrieved with
    	GetPrecipitation().

    	\param x The position, in world coordinates, for which you wish to test for precipitation effects.
    	\param y The position, in world coordinates, for which you wish to test for precipitation effects.
    	\param z The position, in world coordinates, for which you wish to test for precipitation effects.
    */
    virtual bool   HasPrecipitationAtPosition(double x, double y, double z) const;

    /** Returns if this CloudLayer supports the generation of shadow maps. Cumulus cloud decks
    	have the ability to generate a texture map suitable for simulating shadows in your
    	application's terrain and other objects. You may call this method to discover if your
    	cloud deck has this capability. Shadow maps are not supported under DirectX9 if the
        "render-offscreen" setting in resources/silverlining.config is enabled, or under DirectX10.

    	\sa GenerateShadowMaps() BindShadowMap()*/
    virtual bool SupportsShadowMaps()				{
        return false;
    }

    /** For CloudLayers that support shadow maps (see SupportsShadowMaps()), request that a
    	shadow map and matrix is generated each frame. This shadow map texture and matrix may then
    	be retrieved within your rendering loop using the BindShadowMap() method.
    */
    virtual void GenerateShadowMaps(bool enable)	{ enable; }

    /** Bind the CloudLayer's shadow map texture to a given texture unit and retrieve the
    	light source's (projection * view) matrix. The shadow map texture will contain black
    	at shadowed locations and white at unshadowed locations. Mapping world coordinates in
    	your scene to the appropriate shadow map texture coordinates may be achieved by using
    	the 4x4 projection * view matrix for the light source that generated the shadow map,
    	which is passed back into the array of 16 doubles passed in as the second parameter. See
    	the sample code for an example of using shadow maps within an application.

        This method will fail under DirectX10, as there is no fixed-function pipeline for
        SilverLining to bind to.

    	\param textureStage The texture unit to bind the shadow map texture to.
    	\param m A pointer to 16 doubles which will receive the row-major projection * view
    			 matrix of the light source that generated the shadow map.
        \return True if the shadow map was successfully bound to the requested texture unit.

    	\sa GenerateShadowMaps() SupportsShadowMaps()
    */
    virtual bool BindShadowMap(int textureStage, double *m) const {
        textureStage; m;
        return false;
    }

    /** Generates clouds within the cloud layer. SeedClouds() may only be called after configuring
    	the CloudLayer by calling the SetBaseWidth(), SetBaseLength(), SetBaseAltitude(),
    	SetThickness(), SetDensity(), and SetLayerPosition() methods. When called, it will
    	invoke a cloud growth model to fill the cloud layer with clouds to meet the specified
    	properties of the CloudLayer.

    	Once a cloud layer is seeded, you may use the Save() method to save it to disk
    	if you'd like to be able to quickly restore it with the Restore() method, instead
    	of recreating the layer from scratch every time.

    	No clouds will appear within your CloudLayer unless SeedClouds() is called!

    	\param atm A const reference to the Atmosphere object that will ultimately contain this
    	CloudLayer.

    	\sa Save()
    	\sa Restore();
    */
    virtual void SeedClouds(const Atmosphere& atm) = 0;

    /** Adds a LightningListener - derived class to the list of LightningListeners to receive
    	notifications of lightning strikes created by this CloudLayer. The CloudLayer will
    	not destroy the LightningListener; its memory management is up to the caller.

    	\sa ClearLightningListeners()
    */
    void AddLightningListener(LightningListener *listener);

    /** Clears the list of LightningListener objects that receive notifications of lightning
    	strikes from this CloudLayer. It does not delete the LightingListener objects.

    	\sa AddLightningListener()
    */
    void ClearLightningListeners();

    /** Saves this cloud deck's clouds to a file for fast restoring later. This is faster than
    	calling SeedClouds to generate the deck at runtime, and also allows a way to reproduce
    	the same cloud decks by bypassing the randomness of SeedClouds.

    	\param filePath The fully qualified path to the file you wish to save this cloud deck's
    	configuration to.

    	\sa SeedClouds()
    	\sa Restore()

    	\return True if the file was successfully written.
    */
    virtual bool Save(const char *filePath) const;

    /** Restores a cloud deck from a file previously generated with the Save() method. It is not
    	necessary to explicitly configure the cloud's dimensions, position, and density, or to
    	call SeedClouds(), when Restore() is called on a newly instantiated cloud layer.
    	Restore() offers a way to quickly load a known good cloud layer without recreating it
    	from scratch every time using SeedClouds(). It will create the layer more quickly than
    	SeedClouds(), but you'll lose the randomness of the clouds within the layer by using
    	this method.

    	\param atm A const reference to the Atmosphere object that will ultimately contain this
    	CloudLayer.
    	\param filePath The fully qualified path to the file previously created with Save().

    	\sa Save()
    	\sa SeedClouds()

    	\return True if the file was successfully restored; false if the file could not be found
    	or was an invalid file format.
    */
    virtual bool Restore(const Atmosphere& atm, const char *filePath);

    /** Exports each individual cloud into a numbered .WRL file within the
    	specified directory path. Each .WRL file will be a simple collection
    	of translated spheres that represent the metaballs that make up
    	each cloud.

    	These .WRL files may then be tweaked using a 3D graphics package, and
    	then re-imported by placing them in resources/clouds/(cloudtype)/.
    	(cloudtype) is the config file prefix for the settings for the given
    	cloud layer type in resources/SilverLining.config, for example
    	cumulus-congestus or cumulonimbus.

    	If a given cloud type has .WRL files in its matching resources/clouds/
    	subdirectory, SilverLining will randomly select these files as the
    	layer is initialized in SeedClouds(), instead of generating the clouds
    	algorithmically from a cellular automata.

    	This will currently only produce output for cumulus cloud decks.

    	\sa SeedClouds()
    */
    virtual bool ExportToVRML(const char *dirPath);

    // The following methods are for internal use only, and should not be called by
    // application developers:
    void   MoveClouds(double x, double y, double z);
    void   WrapClouds();
    void   SetIsInfinite(bool inf) { isInfinite = inf; }
    void   AddCloud(Cloud *cloud) {  clouds.push_back(cloud); }
    void   LightningNotify(const Vector3& pos);
    virtual void ProcessAtmosphericEffects(Sky *s) { s; }
    virtual bool Draw(const Frustum *f, int pass, const Vector3 *lightPos, const Color *lightColor,
                      bool invalid, bool wantsLightingUpdate, unsigned long now);
    virtual void GetSortPosition(double& x, double& y, double& z);
	virtual void ProcessFade(unsigned long now);
    virtual void ApplyPrecipitation();

protected:
    virtual bool DrawSetup(int pass, const Vector3 *lightPos, const Color *lightColor) = 0;
    virtual bool EndDraw(int pass) = 0;
    virtual bool SaveClouds(FILE *f) const;
    virtual bool RestoreClouds(const Atmosphere& atm, FILE *f) = 0;

	enum FadeModes
	{
		NO_FADE,
		FADE_IN,
		FADE_OUT
	};

	double fade;
	unsigned long fadeTime, fadeStart;
	FadeModes fadeMode;
    double layerX, layerZ;
    double baseWidth, baseLength;
    double baseAltitude, thickness;
    double density;
    bool   layerEnabled;
    bool   forceLightingUpdate;
    bool   firstFrameDrawn;
    bool   isInfinite;

    std::map<int, double> precipitationEffects;
    std::vector<Cloud*> clouds;
    std::vector<LightningListener *> lightningListeners;
    std::vector<Cloud *> drawList;
};
}

#endif