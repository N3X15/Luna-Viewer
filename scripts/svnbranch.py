#!/usr/bin/env python
#
import os
import re

def svnInfo(file):
# URL: https://redirect.modularsystems.sl:8443/svn/Emerald/branches/snowglobe
# Repository Root: https://redirect.modularsystems.sl:8443/svn/Emerald

    urlMatch = None
    repMatch = None
    urlPattern = re.compile("URL: (\S+)")
    repPattern = re.compile("Repository Root: (\S+)")
    for line in file:
	if not urlMatch:
	    urlMatch = urlPattern.search(line)
	if not repMatch:
	    repMatch = repPattern.search(line)
	if(urlMatch and repMatch):
	    tmp = urlMatch.group(1).replace(repMatch.group(1), "")
	    tmp = tmp.strip("/")
	    file.close()
	    return tmp
    file.close()
    return None
	
    
def get_version():
    """Figure out svn version..."""
    rev = svnInfo(os.popen("git svn info"))
    if rev:
	return rev
    rev = svnInfo(os.popen("svn info"))
    return rev

if __name__ == "__main__":
    import sys
    print get_version()

