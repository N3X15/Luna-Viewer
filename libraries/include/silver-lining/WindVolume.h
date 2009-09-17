// Copyright (c) 2004-2009 Sundog Software, LLC. All rights reserved worldwide.

/** \file WindVolume.h
	\brief Defines an area of a given wind speed and direction.
*/

#ifndef WINDVOLUME_H
#define WINDVOLUME_H

namespace SilverLining
{
/** Defines an area of a given wind velocity and direction bounded by two altitudes.
	Passed into AtmosphericConditions::SetWind() to define wind that will affect cloud
	motion. */
class WindVolume
{
public:

    /** Default constructor. Creates a WindVolume with default settings of no wind from
    	zero to 100,000 meters above mean sea level. */
    WindVolume() : minAltitude(0), maxAltitude(100000), windSpeed(0), direction(0) {}

    /** Destructor. */
    virtual ~WindVolume() {}

    /** Set the minimum altitude affected by this object's wind settings.
    	\param metersMSL The minimum altitude of this WindVolume in meters above mean sea level.
    	\sa GetMinAltitude()
    */
    void   SetMinAltitude(double metersMSL)         {
        minAltitude = metersMSL;
    }

    /** Retrieves the minimum altitude, in meters above mean sea level, affected by this object.
    	\sa SetMinAltitude()
    */
    double GetMinAltitude() const                   {
        return minAltitude;
    }

    /** Set the maximum altitude affected by this object's wind settings.
    	\param metersMSL The maximum altitude of this WindVolume in meters above mean sea level.
    	\sa GetMaxAltitude()
    */
    void   SetMaxAltitude(double metersMSL)         {
        maxAltitude = metersMSL;
    }

    /** Retrieves the maximum altitude, in meters above mean sea level, affected by this object.
    	\sa SetMaxAltitude()
    */
    double GetMaxAltitude() const                   {
        return maxAltitude;
    }

    /** Set the wind velocity within this WindVolume, in meters per second.
    	\sa GetWindSpeed()
    */
    void   SetWindSpeed(double metersPerSecond)     {
        windSpeed = metersPerSecond;
    }

    /** Retrieves the wind velocity within this WindVolume, in meters per second.
    	\sa SetWindSpeed()
    */
    double GetWindSpeed() const                     {
        return windSpeed;
    }

    /** Sets the wind direction, in degrees East from North. This is the direction the wind is
    	blowing toward, not the direction it is coming from.
    	\sa GetDirection()
    */
    void   SetDirection(double degreesFromNorth)    {
        direction = degreesFromNorth;
    }

    /** Retrieves the wind direction, in degrees East from North. This is the direction the wind
    	is blowing toward, not the direction it is coming from.
    	\sa SetDirection()
    */
    double GetDirection() const                     {
        return direction;
    }

    /** Evaluates if a given altitude is affected by this WindVolume.
    	\param metersMSL The altitude to query on, in meters above mean sea level.
    	\return True if the given altitude is affected by this WindVolume object.
    */
    bool   Inside(double metersMSL) const
    {
        return (metersMSL > minAltitude && metersMSL <= maxAltitude);
    }

private:
    double minAltitude, maxAltitude;
    double windSpeed, direction;
};
}

#endif //WINDVOLUME_H
