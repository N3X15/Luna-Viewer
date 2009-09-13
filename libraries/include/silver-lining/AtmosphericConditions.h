// Copyright (c) 2004-2009 Sundog Software, LLC
// All rights reserved worldwide.

/** \file AtmosphericConditions.h
	\brief Configures the time, location, atmosphere, and cloud cover.
*/

#ifndef ATMOSPHERIC_CONDITIONS_H
#define ATMOSPHERIC_CONDITIONS_H

#if defined(WIN32) || defined(WIN64)
#pragma warning(disable: 4786)
#endif

#include "WindVolume.h"
#include "Location.h"
#include "LocalTime.h"
#include "MillisecondTimer.h"

#include <vector>
#include <map>

namespace SilverLining
{
class CloudLayer;

/** A class to set the current simulation conditions for the sky.

	This class allows you to specify the simulated time, location, wind, visibility,
	haze, and cloud cover for your scene. You do so by instantiating your own
	AtmosphericConditions object, configuring it by calling its methods, and then passing
	it into your Atmosphere object via Atmosphere::SetConditions().

	To make changes to the AtmosphericConditions after it has been passed into
	Atmosphere::SetConditions(), call Atmosphere::GetConditions() and call methods on the
	AtmosphericConditions reference that it returns. Atmosphere::SetConditions() copies your
	AtmosphericConditions into the Atmosphere class, so changes made to the AtmosphericConditions
	used to initialize the Atmosphere will have no effect.
*/
class AtmosphericConditions
{
public:
    /** Default constructor. Creates an AtmosphericConditions object with default settings. */
    AtmosphericConditions();

    /** Destructor. Destroys all members, but does not destroy the CloudLayer objects passed
    	into AddCloudLayer(). */
    virtual ~AtmosphericConditions();

    /** Sets the simulated geographic location. This affects the sun and moon positions
    	relative to the horizon, and thereby also affects the lighting of the scene. Be
    	sure that the location specified is consistent with the time zone specified in the
    	LocalTime passed to SetTime(), or you will experience confusing results.

        \param location A reference to a Location object that embodies the simulated latitude
    			        and longitude.
    */
    void SetLocation(const Location& location);

    /** Gets the current simulated location.
    	\return A const reference to a Location object that may be queried for the current
    		    simulated geographic position.
    */
    const Location& GetLocation() const;

    /** Sets the simulated time of day in local time. You must specify if daylight savings time
        is currently being observed or not. Be sure that the time zone specified is consistent
    	with the simulated Location passed to SetLocation(), or you will experience confusing
    	results.

    	\param time A reference to a LocalTime object that embodies the simulated local time,
    	time zone, and daylight savings time observation.
    */
    void SetTime(const LocalTime& time);

    /** Retrieves the currently simulated time of day. Use this to obtain the local time, time
    	zone, and daylight savings time observance being simulated.

    	This will also reflect the passage of time simulated by activating
    	EnableTimePassage(). As such, it may differ from what was originally
    	passed into SetTime() if time passage is activated.

    	\return A const reference to a LocalTime object, which may be queried for the local time,
    	time zone, and DST information being simulated.*/
    const LocalTime& GetTime();

    /** Sets a volume of wind for moving clouds. You may call this multiple times to
        define different wind velocities and directions at different altitudes. Wind also
    	has some influence on cloud formation and the rendering of virga effects.

        \param windVolume A WindVolume to add to the simulation.
    */
    void SetWind(const WindVolume& windVolume);

    /** Clears all wind volumes previously set via SetWind. */
    void ClearWindVolumes();

    /** Retrieves the simulated wind at a given altitude.
        \param velocity Receives the simulated wind velocity in meters per second
        \param heading Receives the simulated wind direction in degrees from North
        \param altitude The altitude, in meters, for which you want wind information.
        */
    void GetWind(double& velocity, double& heading, double altitude) const;

    /** Apply wind to cloud layers for given time step. This is called by the Atmosphere class;
    	you do not need to call this directly!

    	\param dt Length of time to simulate wind over, in seconds.
    */
    void ApplyWind(double dt);

    /** Get a vector of all wind volumes being simulated.

    \return A const reference to the STL vector of wind volumes currently being simulated.
    */
    const std::vector<WindVolume>& GetWindVolumes() const {
        return windVolumes;
    }

    /** Adds a cloud layer to the simulated atmospheric conditions. Cloud layers are created via
    	the CloudLayerFactory::Create() method, and won't become part of your scene until they
    	are added to a AtmosphericConditions class that is then passed into
    	Atmosphere::SetConditions(). The CloudLayer object will be destroyed along with this
    	AtmosphericConditions class; there's no need to keep this object around for cleanup.

    	\param layer A CloudLayer created via CloudLayerFactory::Create(). This CloudLayer should
    				 be configured and seeded (see CloudLayer::SeedClouds()) prior to being passed in.
    	\returns An integer handle to the cloud layer that may be used in calls to
    			 RemoveCloudLayer() and GetCloudLayer().
     */
    int AddCloudLayer(CloudLayer *layer);

    /** Removes a previously added cloud deck from the scene. The specified CloudLayer handle,
    	if found, will be removed from the simulation and its CloudLayer object deleted.

        \param layerHandle The integer handle previously returned from AddCloudLayer().
        \return true if the cloud deck was removed, false if the handle specified was not found.
    */
    bool RemoveCloudLayer(int layerHandle);

    /** Removes all cloud layers from the scene. All CloudLayer objects currently contained by
    	this class will be removed and deleted. */
    void RemoveAllCloudLayers();

    /** Obtain a pointer to a CloudLayer object. Given a cloud layer handle, obtain its underlying
    	CloudLayer object.

    	\param layerHandle The integer handle to a cloud layer previously returned by AddCloudLayer().
    	\param layer A pointer to a pointer to receive the CloudLayer object pointer requested.
    	\return false if the layerHandle does not specify a CloudLayer being managed by this class.
    */
    bool GetCloudLayer(int layerHandle, CloudLayer **layer);

    /** Obtain the stl map of CloudLayer objects. An accessor for a reference to the STL map of
    	CloudLayer objects managed by this AtmosphericConditions class. It is not const, and so
    	it may be directly manipulated. Use with care.*/
    std::map<int, CloudLayer*>& GetCloudLayers() {
        return cloudLayers;
    }

    /** Sets the "turbidity" of the simulated atmosphere. You can think of this as a measure
        of "haziness." 1.0 would be a perfectly clear day and is the minimum value. Some
        guidelines for setting this value:

        2 = very clear, range 50 km
        3 = clear, range 15 km
        7 = light haze, range 8 km

    	Setting turbidity is not an appropriate way to simulate fog; see Atmosphere::SetHaze()
    	for one method, or for dense fog simply clear the backbuffer to the fog color and pass
    	false to Atmosphere::BeginFrame() to suppress the sky rendering altogether.

    	Turbidity just lets you simulate the number of particles in the air. In practice it
    	will vary the color of the sky from a pure light blue to a hazy, darker, yellowish
    	color. Turbidity values below 1.8 or above 20.0 may lead to unpredictable results.

        \param \pTurbidity The ratio of scattering due to haze to scattering due to molecules.
        */
    void SetTurbidity(double pTurbidity) {
        turbidity = pTurbidity;
    }

    /** Get the simulated atmospheric turbidity. See SetTurbidity() for an explanation of
    	turbidity values. */
    double GetTurbidity() const {
        return turbidity;
    }

    /** Sets the simulated visibility in meters; this will affect the appearance of clouds in the
    	distance. Defaults to 30km. */
    void SetVisibility(double range) {
        visibility = range;
    }

    /** Retrieves the currently simulated visibility, in meters. */
    double GetVisibility() const {
        return visibility;
    }

	/** Explicitly sets exponential fog color and density, if you need to match an existing scene.
	If set, this will override the visibility effects from SetVisibility(). Color components are
	specified in the range 0-1. Density should be 1.0 / simulated visibility. If set, astronomical
	objects (the sun, moon, and stars) are not drawn, and clouds will be fogged as you specify.
	This is intended for real, thick fog, as opposed to SetVisibility(), which simulates extinction
	from atmospheric scattering. When using this, you'll usually want to call BeginFrame() with
	false in the first parameter to suppress drawing the sky box, and just clear your back buffer
	to the fog color instead. */
	void SetFog(double density, double r, double g, double b);

	/** Clears explicitly set fog from SetFog(), and reverts the fog to simulating the visibility
	specified in SetVisibility() instead. */
	void ClearFog();

	/** Retrieves the explicit fog (if any) set by SetFog().
	\param fogIsSet Returns true if SetFog has been called and ClearFog has not, meaning we are using fog
	with an explicitly defined application setting instead of simulating visibility.
	\param density The exponential fog density term set in SetFog()
	\param r The red component of the fog color specified in SetFog(). Ranges from 0-1.
	\param g The green component of the fog color specified in SetFog(). Ranges from 0-1.
	\param b The blue component of the fog color specified in SetFog(). Ranges from 0-1. */
	void GetFog(bool& fogIsSet, double& density, double &r, double &g, double &b);

    /** Sets the simulated amount of nighttime light pollution, in watts per square meter.
        Defaults to zero. A reasonable value would be in the order of 0.01 */
    void SetLightPollution(double Wm2) {
        lightPollution = Wm2;
    }

    /** Retrieves the currently simulated light pollution, in watts per square meter. */
    double GetLightPollution() const {
        return lightPollution;
    }

    /** Simulates global precipitation of a specified type. Precipitation effects will display if the
        type is set to something other than NONE.

        Note, you may also set precipitation effects associated with a CloudLayer that only render when
        the camera is underneath a rain cloud with the similar CloudLayer::SetPrecipitation() method.

        For mixed precipitation, you may call SetPrecipitation multiple times with different precipitation
        types. To clear all precipitation, call SetPrecipitation with a type of CloudLayer::NONE. If you
        call this method multiple times for the same precipitation type, the intensity specified will
        overwrite the intensity previously specified for that type.

    	\param precipitationType The type of precipitation to simulate under this cloud layer - CloudLayer::NONE,
                    CloudLayer::RAIN, CloudLayer::WET_SNOW, CloudLayer::DRY_SNOW, or CloudLayer::SLEET.
    	\param precipitationRate The simulated rate of precipitation, in millimeters per hour. Reasonable ranges
    	might be between 1 for light rain or 20 for heavier rain. This value will be clamped to the value
    	specified by rain-max-intensity, snow-max-intensity, or sleet-max-intensity in
        resources/SilverLining.config, which is 30 by default.
    */
    void SetPrecipitation(int precipitationType, double precipitationRate);

    /** By default, SilverLining will "freeze" time at the time specified by
    AtmosphericConditions::SetTime(). If you want to simulate the passage of
    time, call EnableTimePassage with the enabled parameter set to true.

    Relighting the clouds is a relatively expensive operation, so for real
    time applications you probably won't want to relight the clouds every
    frame. The relightFrequencyMS parameter allows you to specify the interval,
    in milliseconds, between cloud relighting passes. The sky will continue
    to update in real time, along with the position of the sun, moon, and
    stars, between these intervals. If you have specified your own
    MillisecondTimer with SetMillisecondTimer(), the interval will be computed
    based on its concept of time.

    If the enabled parameter is false (the default,) or the relightFrequencyMS
    parameter is set to -1, cloud relighting will only happen in response to
    calls to AtmosphericConditions::SetTime().

    Clouds will move with respect to the simulated wind irregardless of calling
    this method.

    \param enabled True if you want to simulate the passage of time between
    calls to SetTime(); false if the simulated time should remain static.
    \param relightFrequencyMS The interval, in milliseconds, between cloud
    relighting passes. Set to -1 to prevent cloud relighting outside of calls
    to SetTime().
    */
    void EnableTimePassage(bool enabled, long relightFrequencyMS);

    /** By default, SilverLining will simulate the motion of clouds in the wind
    and the motion of astronomical objects (when EnableTimePassage() is active)
    by calling the system's millisecond timer. If you want to accelerate, slow,
    or reverse the passage of time, you may instead provide your own
    MillisecondTimer implementation, and pass it in here. See the documentation
    for the MillisecondTimer class for more details.

    \param timer The MillisecondTimer object to use for moving clouds and
    astronomical objects over time, relative to the LocalTime passed into
    SetTime(). Pass NULL to restore the default timer.
    */
    void SetMillisecondTimer(const MillisecondTimer *timer);

    /** Retrieves the MillisecondTimer previously set by SetMillisecondTimer(),
    or the default timer if SetMillisecondTimer() is unset or set to NULL. */
    const MillisecondTimer *GetMillisecondTimer() const {
        return timer;
    }

    // Methods for internal use only:
    void ApplyPrecipitation();
    bool WantsLightingUpdate();

private:
    Location    location;
    LocalTime   localTime, adjustedTime;

    double turbidity, visibility, lightPollution;

    const MillisecondTimer *timer;
    MillisecondTimer *defaultTimer;

    std::vector<WindVolume> windVolumes;
    std::map<int, CloudLayer*> cloudLayers;
    std::map<int, double> precipitationEffects;

    bool timePassage;
    unsigned long lastRelightPass, baseTimeMS;
    long relightFrequency;
    bool wantsLightingUpdate;
	bool overrideFog;
	double fogDensity, fogR, fogG, fogB;
};
}

#endif
