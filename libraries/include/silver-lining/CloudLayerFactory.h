// Copyright (c) 2004-2009 Sundog Software, LLC. All rights reserved worldwide.

/** \file CloudLayerFactory.h
	\brief Instantiates specific types of CloudLayer objects.
*/

#ifndef CLOUD_LAYER_FACTORY_H
#define CLOUD_LAYER_FACTORY_H

#include "CloudTypes.h"

namespace SilverLining
{
class CloudLayer;

/** A class factory for CloudLayer objects.

	Call CloudLayerFactory::Create to instantiate a CloudLayer object, which may then be configured,
	seeded, and submitted to the AtmosphericConditions. CloudLayer is a virtual base class, so this
	is the only mechanism for creating specific cloud layer types.
*/
class CloudLayerFactory
{
public:
    /** Instantiate a CloudLayer for a given cloud type. Currently supported #CloudTypes include:

    	\li CloudTypes::CIRROCUMULUS - High planar cloud puffs.
    	\li CloudTypes::CIRRUS_FIBRATUS - High, wispy cirrus clouds.
    	\li CloudTypes::NIMBOSTRATUS - Low stratus decks.
    	\li CloudTypes::CUMULONIMBUS_CAPPILATUS - A large thunderhead with lightning and rain.
    	\li CloudTypes::CUMULUS_MEDIOCRIS - Small, puffy clouds.
    	\li CloudTypes::CUMULUS_CONGESTUS - Larger cumulus clouds with flattened bottoms.
        \li CloudTypes::CUMULUS_MEDIOCRIS_INFINITE - A cumulus mediocris layer where the clouds wrap around
        the camera location at all times, to create the illusion of an infinitely large cloud layer.
        \li CloudTypes::CUMULUS_CONGESTUS_INFINITE - A cumulus congestus layer where the clouds wrap around
        the camera location at all times, to create the illusion of an infinitely large cloud layer.

    	\param layerType The type of cloud deck to create, must be of the #CloudTypes enumeration.
    	\return A pointer to a new CloudLayer for the specified type.
    */
    static CloudLayer *Create(CloudTypes layerType);
};
}

#endif