// Copyright (c) 2004-2008  Sundog Software, LLC. All rights reserved worldwide.

#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

/** \file Atmosphere.h
	\brief The main interface to SilverLining.
*/

#include "AtmosphericConditions.h"
#include "RandomNumberGenerator.h"
#include <map>

namespace SilverLining
{
// Forward declarations of internal classes.
class Sky;
class Ephemeris;
class AtmosphereFromSpace;

typedef void * ObjectHandle;

/** This class is the main interface to SilverLining.  The Atmosphere simulates the sky,
    clouds, and weather.

	To use an Atmosphere class, simply instantiate one using the default constructor, and
	then call the Initialize() method to set it up for either the OpenGL or DirectX rendering
	subsystems.

	You may initialize the atmospheric conditions (cloud decks, wind, time, location, etc.)
	by populating an AtmosphericConditions class and passing this to the SetConditions method
	prior to rendering.

	Then, within your main rendering loop, as soon as you have set the view and projection
	matrices for your scene to reflect the current camera position, call the BeginFrame() method.
	This will draw the background of the sky and perform any lighting computations required for
	the atmosphere.

	At this point, you may query GetSunOrMoonPosition() and GetSunOrMoonColor() to obtain the
	tone-mapped directional lighting information for your outdoor scene, and GetAmbientColor() to
	get the tone-mapped ambient skylight for your scene.

	Finally, at the end of your rendering loop, call EndFrame(). This will draw all of the clouds
	within the scene in back-to-front order.
	*/
class Atmosphere
{
public:
    /** Constructor. Creates an atmosphere object with default settings. You must specify
    	the user name and license key provided with your SilverLining license purchase in
    	order to instantiate an Atmosphere. An invalid name / key combination will result in
    	a dialog box informing the user that this software is unlicensed, and in the application
    	terminating after two minutes of use.

        Warning! Displaying a dialog box while in full-screen mode will cause some DirectX
        applications to crash. To prevent the licensing dialog box on unlicensed SDK's from
        causing a crash, instantiate your Atmosphere object before entering full-screen mode.

    	\param userName The user name, exactly as provided to you in the licensing information
    				    received with your SilverLining license purchase. A null-terminated C
    					string.
    	\param licenseKey The license key code, exactly as provided to you with your license
    					  purchase. Null-terminated C string.

    */
    Atmosphere(const char *userName, const char *licenseKey);

    /** Destructor. This will clean up all cloud objects owned by the atmospheric conditions. */
    ~Atmosphere();

    /** Returns the version number of SilverLining (ie, 1.0) */
    float GetVersionNumber() const;

    /** Configures the simulated cloud, wind, time, and location. The cloud, wind, time, and
    	location settings are all contained inside an AtmosphericConditions object. Pass in
    	an AtmosphericConditions class configured the way you like it, after first calling Initialize
    	on the Atmosphere class, and before rendering your scene. Any previous conditions will
    	be overwritten by this call.

    	Note, this passes in a const reference to your AtmosphericConditions class. It is copied
    	internally, and you're free to dispose of your AtmosphericConditions object once you
    	have passed it into SetConditions. */
    void SetConditions(const AtmosphericConditions& conditions);

    /** Accessor to the current cloud, wind, time, and location settings. Returns a const
    	reference to an AtmosphericConditions class that contains the settings for the current
    	simulation. */
    const AtmosphericConditions& GetConditions() const;

    /** Retrieve the normalized direction of the dominant directional light source.
    	This method will return a normalized direction vector pointing to the dominant light source,
    	which may be the moon after sunset. */
    void GetSunOrMoonPosition(float *x, float *y, float *z) const;

	/** Retrieve the normalized direction of the dominant directional light source.
    	This method will return a normalized direction vector pointing to the dominant light source,
    	which may be the moon after sunset. The direction is relative to the center of the Earth in a
    	geocentric coordinate system, where the z axis points from the center of the
    	Earth to the North Pole, and x points toward the prime meridian.*/
    void GetSunOrMoonPositionGeographic(float *x, float *y, float *z) const;

	/** Retrieve the normalized direction of the dominant directional light source.
    	This method will return a normalized direction vector pointing to the dominant light source,
    	which may be the moon after sunset. The direction is in equatorial coordinates; where x points
    	toward the vernal equinox (where the equator and ecliptic intersect), z points
    	through the north pole.*/
    void GetSunOrMoonPositionEquatorial(float *x, float *y, float *z) const;

    /** Retrieve the normalized direction of the sun's light source. This method will return a
    	normalized direction vector pointing to the sun, which may be below the horizon. */
    void GetSunPosition(float *x, float *y, float *z) const;

    /** Retrieve the normalized direction of the moon's light source. This method will return a
    	normalized direction vector pointing to the moon, which may be below the horizon. */
    void GetMoonPosition(float *x, float *y, float *z) const;

    /** Retrieve the direction of the sun, relative to the center of the Earth in a
    	geocentric coordinate system, where the z axis points from the center of the
    	Earth to the North Pole, and x points toward the prime meridian. */
    void GetSunPositionGeographic(float *x, float *y, float *z) const;

    /** Retrieve the direction of the moon, relative to the center of the Earth in a
    	geocentric coordinate system, where the z axis points from the center of the
    	Earth to the North Pole, and x points toward the prime meridian. */
    void GetMoonPositionGeographic(float *x, float *y, float *z) const;

    /** Retrieve the direction of the sun in equatorial coordinates; where x points
    	toward the vernal equinox (where the equator and ecliptic intersect), z points
    	through the north pole. */
    void GetSunPositionEquatorial(float *x, float *y, float *z) const;

    /** Retrieve the direction of the moon in equatorial coordinates; where x points
    	toward the vernal equinox (where the equator and ecliptic intersect), z points
    	through the north pole. */
    void GetMoonPositionEquatorial(float *x, float *y, float *z) const;

    /** Returns the color of the dominant directional light source. This method will return a color
    	suitable for lighting your scene based on the natural lighting conditions. The value is
    	tone-mapped, so the high dynamic range between night and day and they way the human eye
    	perceives it is modeled. For example, at noon this value is likely white, but at night
    	it may be suprisingly bright if a full moon is out. At sunset or sunrise, there may be an
    	orange, pink, or reddish hue due to the scattering of sunlight through the atmosphere -
    	which is also simulated. This scattering is sensitive to the turbidity setting specified
    	in the AtmosphericConditions class.

    	This method only simulates natural light sources passing through the atmosphere: the sun,
    	moon, starlight, planetary light, airglow, galactic light, and zodiacal light. At night,
    	it may be appropriate to add in a little extra to simulate city lights for urban or suburban
    	scenes.

    	If your camera position is below a thick stratus cloud deck, the effects on lighting will
    	be simulated as well.

    	\param r A pointer to a float that will be populated with the red directional light color,
    			 ranging from 0 - 1.0.
    	\param g A pointer to a float that will be populated with the green directional light color,
    			 ranging from 0 - 1.0.
    	\param b A pointer to a float that will be populated with the blue directional light color,
    			 ranging from 0 - 1.0.
    */
    void GetSunOrMoonColor(float *r, float *g, float *b) const;


    /** Returns the color of the sun's light source. This method will return a color
    	suitable for lighting your scene based on the natural lighting conditions. The value is
    	tone-mapped, so the high dynamic range between night and day and they way the human eye
    	perceives it is modeled. At sunset or sunrise, there may be an
    	orange, pink, or reddish hue due to the scattering of sunlight through the atmosphere -
    	which is also simulated. This scattering is sensitive to the turbidity setting specified
    	in the AtmosphericConditions class.

    	If your camera position is below a thick stratus cloud deck, the effects on lighting will
    	be simulated as well.

    	\param r A pointer to a float that will be populated with the red directional light color,
    			 ranging from 0 - 1.0.
    	\param g A pointer to a float that will be populated with the green directional light color,
    			 ranging from 0 - 1.0.
    	\param b A pointer to a float that will be populated with the blue directional light color,
    			 ranging from 0 - 1.0.
    */
    void GetSunColor(float *r, float *g, float *b) const;

    /** Returns the color of the moon's light source. This method will return a color
    	suitable for lighting your scene based on the natural lighting conditions. The value is
    	tone-mapped, so the high dynamic range between night and day and they way the human eye
    	perceives it is modeled. For example, at night it may be suprisingly bright if a full moon
    	is out. This scattering is sensitive to the turbidity setting specified in the
    	AtmosphericConditions class.

    	This method only simulates natural light sources passing through the atmosphere: the sun,
    	moon, starlight, planetary light, airglow, galactic light, and zodiacal light. At night,
    	it may be appropriate to add in a little extra to simulate city lights for urban or suburban
    	scenes.

    	If your camera position is below a thick stratus cloud deck, the effects on lighting will
    	be simulated as well.

    	\param r A pointer to a float that will be populated with the red directional light color,
    			 ranging from 0 - 1.0.
    	\param g A pointer to a float that will be populated with the green directional light color,
    			 ranging from 0 - 1.0.
    	\param b A pointer to a float that will be populated with the blue directional light color,
    			 ranging from 0 - 1.0.
    */
    void GetMoonColor(float *r, float *g, float *b) const;

    /** Returns the color of the ambient "skylight". This color is suitable for use as an ambient
    	light color for your scene. When used together with the directional light position and color
    	returned by GetSunOrMoonPosition and GetSunOrMoonColor, an accurate simulated model of
    	natural lighting conditions may be obtained.

    	The color returned by this method is arrived at by simulating the light scattered over the
    	sky above your simulated location at your simulated time, commonly referred to as "skylight."
    	This light is then tone-mapped to account for high dynamic range in the same manner as
    	the directional light. This color is also affected by the presence of thick cloud decks
    	above the camera position.

    	\param r A pointer to a float to receive the red channel of the ambient color, from 0 -1.0.
    	\param g A pointer to a float to receive the green channel of the ambient color, from 0 -1.0.
    	\param b A pointer to a float to receive the blue channel of the ambient color, from 0 -1.0.
    */
    void GetAmbientColor(float *r, float *g, float *b) const;

    /** Returns the average color of the sky at the horizon. Based on the specified view direction
    	and field of view, this method will return the average color of the sky in the scene at
    	the horizon. This is often an effective choice of a fog color for your scenes, since it
    	will blend well with the sky in the distance - thereby covering up outdoor scenes that lack
    	sufficient terrain to extend to the horizon. Even for scenes that do render to the horizon,
    	this is a good color for fog just for simulating haze and atmospheric perspective.

    	Note that during sunrise and sunset when a red glow surround the sun, this color may
    	vary widely depending on the yawDegrees parameter.

    	\param yawDegrees The camera's yaw value, in degrees east from north. The horizon color
    					  will be calculated by averaging the field of view's horizon color about
    					  this direction.
    	\param r A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    	\param g A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    	\param b A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    */
    void GetHorizonColor(float yawDegrees, float *r, float *g, float *b) const;

    /** Returns the average color of the sky at the horizon. Based on the current camera orientation
    	and field of view, this method will return the average color of the sky in the scene at
    	the horizon. This is often an effective choice of a fog color for your scenes, since it
    	will blend well with the sky in the distance - thereby covering up outdoor scenes that lack
    	sufficient terrain to extend to the horizon. Even for scenes that do render to the horizon,
    	this is a good color for fog just for simulating haze and atmospheric perspective.

    	Note that during sunrise and sunset when a red glow surround the sun, this color may
    	vary widely depending on the orientation of the camera.

    	\param r A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    	\param g A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    	\param b A pointer to a float to receive the red channel of the horizon color, from 0 - 1.0
    */
    void GetHorizonColor(float *r, float *g, float *b) const;

    /** Returns whether SilverLining would like to suggest fog settings. SilverLining depends on
    	fog effects to simulate being inside a stratus or broken stratus cloud deck, or to simulate
    	reduced visibility due to the presence of rain or snow at the camera's location. If this method
    	returns true, then you should call GetFogSettings in order to help determine the appropriate
    	fog configuration for your scene in order to preserve these atmospheric effects.*/
    bool GetFogEnabled() const;

    /** Returns exponential fog settings appropriate for your scene. If GetFogEnabled() returns
    	true, then call GetFogSettings() to obtain suggested fog values required to preserve
    	in-cloud effects and precipitation effects that depend on fog. The r, g, and b colors
    	returned are the color of the fog itself modulated by the directional light color.

    	\param density A fog density appropriate for use in GL_FOG_DENSITY in exponential fog
    				   mode. ie, \f$f = e^{-(density \cdot z)}\f$
    	\param r A pointer to a float to receive the red component of the fog color, from 0 - 1.0.
    	\param g A pointer to a float to receive the green component of the fog color, from 0 - 1.0.
    	\param b A pointer to a float to receive the blue component of the fog color, from 0 - 1.0.
    	*/
    void GetFogSettings(float *density, float *r, float *g, float *b) const;

    /** Causes the sky to blend toward a specified "haze color" toward the horizon. Although
    it does simulate a layer of colored fog, it's most practical application to allow for
    exact blending against a fog color used for terrain, in order to obscure the horizon line.
    For applications that do not render terrain all the way to the horizon, this is a must.
    GetHorizonColor() may be used for an approximate match in the absence of an artificial
    layer of haze; it is more physically accurate.

    The haze color passed in is not lit; you must pre-multiply the color yourself. The skybox
    will blend toward the exact color passed in at the horizon, night or day. You could abuse
    this to create a glow effect at the horizon from city lights, for example.

    By default, hazeDepth is set to 0, thereby disabling the haze effects. If the viewpoint
    is within a cloud, the fog effects from the cloud will drawn in the sky in lieu of haze.

    \param hazeR The red component of the color to blend toward at the horizon.
    \param hazeG The red component of the color to blend toward at the horizon.
    \param hazeB The red component of the color to blend toward at the horizon.
    \param hazeDepth The simulated height of the haze volume at ground level, in world units.
    \param hazeDensity The fog density parameter of the exponential fog equation.
    */
    void SetHaze(float hazeR, float hazeG, float hazeB, double hazeDepth,
                 double hazeDensity);

    /** Retrieves the haze parameters set previously by SetHaze(). See SetHaze() for a
    description of the parameters. */
    void GetHaze(float& hazeR, float& hazeG, float& hazeB, double& hazeDepth,
                 double& hazeDensity);

	/** Sets the value used for gamma correction of the display. Defaults to the sky-box-gamma setting. 
		1.8 works well. Higher values will yield lighter skies and natural light. */
	void SetGamma(double gamma);

	/** Retrieves the value being used for display gamma correction. 
		\sa SetGamma()
	*/
	double GetGamma() const;

    /** Force SilverLining to recompute all cloud lighting. Under normal operation, SilverLining
    	only recomputes cloud lighting when the simulated time, location, or cloud cover changes,
    	or when the camera moves a significant amount relative to each cloud. If you wish to force
    	a lighting computation for all clouds to take place on the next frame, call this method.

    	There is a performance impact for calling this. Under normal circumstances, there is no
    	reason to call this method. */
    void ForceLightingRecompute() {
        invalid = true;
    }

    /** Return a reference to the current simulated conditions. Use this to change the current
    	simulated time, location, etc., by manipulating the AtmosphericConditions object owned
    	by the Atmosphere class directly. */
    AtmosphericConditions *GetConditions() {
        return &conditions;
    }

    enum
    {
        OPENGL = 0,
        DIRECTX9,
		DIRECTX10
    };

    enum InitializeErrors
    {
        E_NOERROR = 0,
        E_RESOURCES_PATH_INVALID,
        E_CONFIGURATION_FILE_NOT_FOUND,
        E_CANT_LOAD_RENDERER_DLL,
        E_CANT_INITIALIZE_RENDERER_SUBSYSTEM,
        E_CANT_LOAD_METABALL_TEXTURE
    };

    /** Call this immediately after constructing your scene's Atmosphere and initializing your
    	graphics subsystem (OpenGL, DirectX9, or DirectX10). This method will configure SilverLining to use
    	OpenGL or DirectX, and in the case of DirectX, allows you to pass in a required pointer
    	to your IDirect3DDevice9 or ID3D10Device object.

    	Note, DirectX 9 users must create their IDirect3DDevice9 using the
    	D3DPRESENTFLAG_LOCKABLE_BACKBUFFER flag. SilverLining depends on the ability to read back
    	a small number of pixels from the back buffer when it computes its lighting effects.
    	Passing in a device without a lockable back buffer will result in black clouds. You also
    	must NOT create your device with the D3DCREATE_PUREDEVICE flag; SilverLining must be able
    	to read your current render states and transforms in order to restore them to what they
    	were when we finish our own drawing.

    	OpenGL users may pass 0 for the environment parameter. It is ignored for OpenGL.

    	You must also pass in a path to the Resources directory, which contains the art resources,
    	data files, and shaders required for SilverLining to run. You may name and redistribute
    	this directory however you wish, but SilverLining needs to know where it is and what
    	it's called.

    	\param renderer Pass the enumerated constant \c SilverLining::OPENGL or
    					\c SilverLining::DIRECTX9 or \c SilverLining::DIRECTX10

    	\param resourceDirectoryPath A null-terminated string that specifies a path to
    	the application's redistributed "Resources" directory, including the directory name
    	itself and the trailing backslash. For example, "..\\Resources\\". If you pass NULL,
    	the default path is ".\\Resources".

    	\param rightHanded Pass true if you're using a right-handed coordinate system, false for
    	a left-handed coordinate system. OpenGL typically uses right-handed; DirectX can use either.

    	\param environment Only required for DIRECTX9 or DIRECTX10 renderers; a pointer to your 
		IDirect3DDevice9 or ID3D10Device.

    	\return An error code from the Atmosphere::InitializeErrors enumeration, or E_NOERROR. See
        the troubleshooting section of the documentation for further guidance if you encounter an 
        error.
    */
    int Initialize(int renderer, const char *resourceDirectoryPath, bool rightHanded, void *environment);

    /** Sets the assumption of what direction is "up". Defaults to (0, 1, 0) if unset. If the vector (x, y, z)
    	is not a unit vector, it is normalized before being stored. Cannot be called prior to
    	Atmosphere::Initialize(). Must be called in conjunction with SetRightVector().

        If you're not using a default Up vector, be sure to call this prior to positioning
        any clouds.

    	\sa SetRightVector()
    	\sa GetUpVector()
    	*/
    void SetUpVector(double x, double y, double z);

    /** Returns the direction that SilverLining assumes is "up" as a unit vector. */
    void GetUpVector(double& x, double& y, double& z);

    /** Sets the assumption of what direction is "right". Defaults to (1, 0, 0) if unset. If the vector (x, y, z)
    	is not a unit vector, it is normalized before being stored. Cannot be called prior to
    	Atmosphere::Initialize(). Must be called in conjunction with SetUpVector().

        If you're not using a default right vector, be sure to call this prior to positioning
        any clouds.

    	\sa SetUpVector()
    	\sa GetRightVector()
    	*/
    void SetRightVector(double x, double y, double z);

    /** Returns the direction that SilverLining assumes is "right" as a unit vector. */
    void GetRightVector(double& x, double& y, double& z);

	/** Call this at the beginning of each rendering loop, prior to calling BeginFrame(). The
		matrix passed in should represent the transform for your camera, as a 4x4 matrix of
		doubles. Be sure to also call SetProjectionMatrix() */
	void SetCameraMatrix(double *cameraMatrix);

	/** Call this at the beginning of each rendering loop, prior to calling BeginFrame(). The
		matrix passed in should represent the projection matrix for your scene, as a 4x4 matrix
		of doubles. Be sure to also call SetCameraMatrix() */
	void SetProjectionMatrix(double *projectionMatrix);

    /** Call this at the beginning of your rendering loop. At the start of each frame in your scene,
    	first call SetCameraMatrix() and SetProjectionMatrix(). Then, call BeginFrame(). This will 
		draw the skybox for the simulated time and location, including the sun, moon, stars, and planets, 
		and perform any necessary lighting calculations.

    	It is important that the camera and projection matrices are set to reflect your scene's current
    	camera prior to calling BeginFrame(). Versions of SilverLining prior to 1.7 would extract
		these matrices automatically, but beginning with 1.7 you MUST call SetCameraMatrix() and
		SetProjectionMatrix(). We made this change in order to support DirectX 10, which has no
		fixed function pipeline for us to query for these matrices.

    	\param drawSky Pass false to suppress drawing of the skybox, but still perform any lighting
    				   calculations required.
        \param geocentricMode If set to true, the stars, planets, sun, and moon will be
    	drawn in a geocentric coordinate frame, where the X axis points through the
    	prime meridian, Z points through the North Pole, and the origin is at the center
    	of the Earth. This is generally only useful for space-based viewpoints. Normally,
    	you'll leave this set to false so that astronomical objects will be drawn in local
    	horizon coordinates.
        \param skyBoxDimension Sets an explicit dimension in world units for the length of
        a face of the sky box. Useful to prevent clipping of the skybox in systems that
        dynamically adjust the near and far clip planes; you can specify a value each frame
        that fits within them. Most applications can just leave this set to 0, in which case
        it will draw the sky box with the default size specified in SilverLining.config 
        (1000)
        \return true if the skybox was successfully drawn.
    */
    bool BeginFrame(bool drawSky, bool geocentricMode = false, double skyBoxDimension = 0);

    /** Call this at the end of your rendering loop. After drawing all of your scene's objects, call
    	the EndFrame() method. This will draw all of the scene's clouds from back to front, if the
    	drawClouds parameter is set to true. If there are translucent objects in your scene that should
    	be drawn in front of the clouds, you'll want to draw those after calling EndFrame().

    	Alternately, you may obtain handles to each cloud object independently and sort them with the
    	other translucent objects in your scene. If you set drawClouds to false, EndFrame() will build
    	up a list of translucent objects to render for this frame, but not actually render them. You
    	may then access each individual cloud object with the GetObjects() method, sort them against your
    	other translucent objects using the GetObjectDistance() method to obtain their sort distances, and
    	then actually draw them using DrawObject().

    	For applications where the scene is generally beneath the clouds, setting drawClouds to true
    	and then drawing your own translucent objects after EndFrame() is generally adequate.

    	\param drawClouds Set to true if you want EndFrame to actually draw all of the clouds, sorted
    	from back to front. Set to false if you just want to build up a list of clouds to draw but not
    	actually draw them; this list may be obtained with GetObjects().

    	\return true if the clouds were successfully drawn.
    */
    bool EndFrame(bool drawClouds = true);

    /** Display the framerate in the bottom right of the viewport. Call to enable or disable a
    	display of the framerate, in frames per second, in the bottom right hand corner of the
    	screen. Note, this is disabled in the DirectX 9 renderer due to compatibility problems. 
        The framerate displayed is a moving average over 300 frames. */
    void ShowFramerate(bool framerateOn) {
        framerate = framerateOn;
    }

    /** Return the current framerate, based on the rendering time of the previous frame, in
    	frames per second. */
    double GetFramerate() const {
        return framerateValue;
    }

    /** If you called EndFrame() with the drawClouds parameter set to false, no clouds will actually be
    drawn. It's then your responsibility to obtain a list of cloud objects to draw, and draw them yourself.
    GetObjects() provides you with this list. This list will be empty until EndFrame() has been called for
    the current frame. If you call EndFrame() with the drawClouds parameter set to true, this list will
    also be empty, since the objects have already been drawn.

    \sa EndFrame()
    \sa GetObjectDistance()
    \sa DrawObject()
    */
    std::vector<ObjectHandle>& GetObjects() const;

    /** If you are managing your own cloud drawing, you'll need to sort them from back to front along
    with the other objects in your scene. This method will return the distance from a given position
    you are sorting from to the object in question.

    \param obj An object handle obtained from the list returned by GetObjects(), after calling EndFrame(false).
    \param obj2X The X coordinate of the object you are currently comparing this one against.
    \param obj2Y The Y coordinate of the object you are currently comparing this one against.
    \param obj2Z The Z coordinate of the object you are currently comparing this one against.
    \param sortPosX The X coordinate of the viewpoint location you are sorting translucent objects against.
    \param sortPosY The Y coordinate of the viewpoint location you are sorting translucent objects against.
    \param sortPosZ The Z coordinate of the viewpoint location you are sorting translucent objects against.
    \return The distance to the object from the sort position specified, taking into account internal depth
    biases.

    \sa EndFrame()
    \sa DrawObject()
    \sa GetObjects()
    */
    float GetObjectDistance(ObjectHandle obj, float obj2X, float obj2Y, float obj2Z,
                            float sortPosX, float sortPosY, float sortPosZ);

    /** If you are managing your own cloud drawing, you'll need to sort them from back to front along
    with the other objects in your scene. This method will return the distance from a given position
    you are sorting from to the object in question.

    \param obj An object handle obtained from the list returned by GetObjects(), after calling EndFrame(false).
    \param obj2 The object you are comparing this object against.
    \param sortPosX The X coordinate of the viewpoint location you are sorting translucent objects against.
    \param sortPosY The Y coordinate of the viewpoint location you are sorting translucent objects against.
    \param sortPosZ The Z coordinate of the viewpoint location you are sorting translucent objects against.
    \return The distance to the object from the sort position specified, taking into account internal depth
    biases.

    \sa EndFrame()
    \sa DrawObject()
    \sa GetObjects()
    */
    float GetObjectDistance(ObjectHandle obj, ObjectHandle obj2, float sortPosX, float sortPosY, float sortPosZ);

    /** Obtains the position, in world coordinates, of a given ObjectHandle obtained from the list returned
    from GetObjects(). */
    void GetObjectPosition(ObjectHandle obj, float& x, float& y, float& z);

    /** If you are managing your own cloud object drawing by calling EndFrame(false), then your sorted list
    of objects may drawn by calling DrawObject() on each object.

    Your rendering state must be set for rendering translucent objects. Specifically, you must enable
    blending with a blend equation of ONE, INVSRCALPHA. Lighting must be off, depth reads enabled,
    depth writes disabled, fog must be off, and 2D texturing enabled.

    \param obj An object handle obtained from the list returned by GetObjects(), after calling EndFrame(false).

    \sa EndFrame()
    \sa GetObjects()
    \sa GetObjectDistance()
    */
    void DrawObject(ObjectHandle obj);

    /** Reloads the SilverLining.config file in the resources directory. This won't
    immediately affect existing clouds in the scene, but will affect new clouds
    created going forward. Mostly this is useful for advanced users who want to
    tweak the default config file settings without having to shut down and restart
    their application every time. Requires that Atmosphere::Initialize() was called
    previously. */
    void ReloadConfigFile();

    /** Sets a specific SilverLining.config value. Will overwrite whatever was specified.
    Subject to the same caveats listed for ReloadConfigFile(). */
    void SetConfigOption(const char *key, const char *value);

    /** Retrieves a specific SilverLining.config value as a string. */
    const char *GetConfigOptionString(const char *key) const;

    /** Retrieves a specific SilverLining.config value as a double. */
    double GetConfigOptionDouble(const char *key) const;

	/** Retrieves a specific SilverLining.config value as a double. */
    bool GetConfigOptionBoolean(const char *key) const;

	/** Forces the ephemeris model to recalculate the position of the sun, moon, and stars for the
	currently simulated time and location. This happens automatically when BeginFrame() is called; this is
	provided only if you need to compute the astronomical positions in response to a change in the simulated 
	time and location prior to calling BeginFrame(). */
	void UpdateEphemeris();

    /** If you wish to override SilverLining's default random number generator, which is based on the
    standard library's rand() function, you may do so by extending the RandomNumberGenerator base class,
    instantiating your extended class, and passing in a pointer to your class here. Your class will
    be deleted along with the Atmosphere object. */
    static void SetRandomNumberGenerator(const RandomNumberGenerator* rng);

    static const RandomNumberGenerator *GetRandomNumberGenerator();

    void ShowOffscreenBuffer(bool debugOn) {
        debugMode = debugOn;
    }

    static const char *GetResourcePath() {
        return resourcePath;
    }

private:
    Sky *sky;
    Ephemeris *ephemeris;
    static const RandomNumberGenerator *randomNumberGenerator;
    void *context;
    AtmosphereFromSpace *atmosphereFromSpace;
    AtmosphericConditions conditions;
    bool debugMode, framerate;
    double framerateValue;
    double atmosphereHeight;
    unsigned long lastTime;
    bool invalid;
    static char *resourcePath;
	double camera[16];
	double projection[16];
	bool hasCameraMatrix, hasProjectionMatrix;

    void DrawClouds();
    void ShadeClouds();
    void CalculateFramerate();
    void DisplayFramerate();
};
}

#endif