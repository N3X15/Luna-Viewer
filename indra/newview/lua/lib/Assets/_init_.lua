print "Loading asset functions..."

AssetType={
		-- Used for painting the faces of geometry.
		-- Stored in typical j2c stream format
		TEXTURE = 0,

		-- Used to fill the aural spectrum.
		SOUND = 1, 

		-- Links instant message access to the user on the card. eg, a
		-- card for yourself, a card for linden support, a card for
		-- the guy you were talking to in the coliseum.
		CALLINGCARD = 2,

		-- Links to places in the world with location and a screen
		-- shot or image saved. eg, home, linden headquarters, the
		-- coliseum, or destinations where we want to increase
		-- traffic.
		LANDMARK = 3,

		-- Valid scripts that can be attached to an object. eg. open a
		-- door, jump into the air.
		SCRIPT = 4,

		-- A collection of textures and parameters that can be worn
		-- by an avatar.
		CLOTHING = 5,

		-- Any combination of textures, sounds, and scripts that are
		-- associated with a fixed piece of geometry. eg, a hot tub, a
		-- house with working door.
		OBJECT = 6,

		-- Just text
		NOTECARD = 7,

		-- A category holds a collection of inventory items. It's
		-- treated as an item in the inventory, and therefore needs a
		-- type.
		CATEGORY = 8,

		-- A root category is a user's root inventory category. We
		-- decided to expose it visually, so it seems logical to fold
		-- it into the asset types.
		ROOT_CATEGORY = 9,

		-- The LSL is the brand spanking new scripting language. We've
		-- split it into a text and bytecode representation.
		LSL_TEXT = 10,
		LSL_BYTECODE = 11,
		
		-- uncompressed TGA texture
		TEXTURE_TGA = 12,

		-- A collection of textures and parameters that can be worn
		-- by an avatar.
		BODYPART = 13,

		-- This asset type is meant to only be used as a marker for a
		-- category preferred type. Using this, we can throw things in
		-- the trash before completely deleting.
		TRASH = 14,

		-- This is a marker for a folder meant for snapshots. No
		-- actual assets will be snapshots, though if there were, you
		-- could interpret them as textures.
		SNAPSHOT_CATEGORY = 15,

		-- This is used to stuff lost&found items into
		LOST_AND_FOUND = 16,

		-- uncompressed sound
		SOUND_WAV = 17,

		-- uncompressed image, non-square, and not appropriate for use
		-- as a texture.
		IMAGE_TGA = 18,

		-- compressed image, non-square, and not appropriate for use
		-- as a texture.
		IMAGE_JPEG = 19,

		-- animation
		ANIMATION = 20,

		-- gesture, sequence of animations, sounds, chat, wait steps
		GESTURE = 21,

		-- simstate file
		SIMSTATE = 22,

		-- Inventory symbolic link
		LINK = 24,

		-- Inventory folder link
		LINK_FOLDER = 25,

		-- +*********************************************+
		-- |  TO ADD AN ELEMENT TO THIS ENUM:            |
		-- +*********************************************+
		-- | 1. INSERT BEFORE COUNT                   |
		-- | 2. INCREMENT COUNT BY 1                  |
		-- | 3. ADD TO LLAssetType::mAssetTypeNames      |
		-- | 4. ADD TO LLAssetType::mAssetTypeHumanNames |
		-- +*********************************************+

		COUNT = 26,

		NONE = -1
}