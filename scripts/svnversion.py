#!/usr/bin/env python
#
import os
import re

def svnInfo(file):
    pattern = re.compile("Last Changed Rev: (\w+)")
    for line in file:
	match = pattern.search(line)
	if(match):
	    file.close()
	    return match.group(1)
    file.close()
    return None

# N3X15:  This is cheating, but it'll work for now.
def gitInfo():
    return os.popen("git log -n 1 --pretty=format:%ct")
    
def get_version():
    """Figure out svn version..."""
    rev = svnInfo(os.popen("git svn info"))
    if rev:
	return rev
    rev = svnInfo(os.popen("svn info"))
    if rev:
        return rev
    rev = gitInfo()
    return rev

if __name__ == "__main__":
    import sys
    print get_version()
