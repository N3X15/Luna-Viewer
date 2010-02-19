# -*- cmake -*-
include(Prebuilt)

set(OTR_LIBRARY otr)
if (NOT LINUX)
	set(GCRYPT_LIBRARY gcrypt.11)
	set(GPG-ERROR_LIBRARY gpg-error.0)
endif (NOT LINUX)
