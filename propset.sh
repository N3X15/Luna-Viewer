#!/bin/bash
# Needs to be done, or loading the client WILL fail.
#

find . -type f -name "*.h" -print | while 
read filename 
do ( 
	echo $filename;
	flip -ub $filename
	svn propset svn:eol-style native $filename
	svn propset svn:keywords "Rev Id" $filename
) 
done
find . -type f -name "*.cpp" -print | while
read filename
do (
        echo $filename;
        flip -ub $filename
        svn propset svn:eol-style native $filename 
	svn propset svn:keywords "Rev Id" $filename    
)
done
find . -type f -name "*.xml" -print | while
read filename
do (
        echo $filename;
        flip -ub $filename
        svn propset svn:eol-style native $filename
)
done
find . -type f -name "*.txt" -print | while
read filename
do (
        echo $filename;
        flip -ub $filename
        svn propset svn:eol-style native $filename
)
done
find . -type f -name "*.ini" -print | while
read filename
do (
        echo $filename;
        flip -ub $filename
        svn propset svn:eol-style native $filename
)
done
