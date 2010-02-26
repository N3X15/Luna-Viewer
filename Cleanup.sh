svn status | lua scripts/FixSVN.lua
find -name "Thumbs.db" -print | xargs svn delete
. propset.sh
