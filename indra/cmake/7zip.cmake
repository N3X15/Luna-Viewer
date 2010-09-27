# -*- cmake -*-
include(Prebuilt)

# These comments should go away when people decide what to do about packaging 7zip.
# Probably.
if (DARWIN OR LINUX)
	use_prebuilt_binary(7zip)
endif (DARWIN OR LINUX)
