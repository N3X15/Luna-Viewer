#!/usr/bin/python
"""\
@file update_version_files.py
@brief Update all of the various files in the repository to a new 
version number, instead of having to figure it out by hand

$LicenseInfo:firstyear=2006&license=viewergpl$

Copyright (c) 2006-2009, Linden Research, Inc.

Second Life Viewer Source Code
The source code in this file ("Source Code") is provided by Linden Lab
to you under the terms of the GNU General Public License, version 2.0
("GPL"), unless you have obtained a separate licensing agreement
("Other License"), formally executed by you and Linden Lab.  Terms of
the GPL can be found in doc/GPL-license.txt in this distribution, or
online at http://secondlifegrid.net/programs/open_source/licensing/gplv2

There are special exceptions to the terms and conditions of the GPL as
it is applied to this Source Code. View the full text of the exception
in the file doc/FLOSS-exception.txt in this software distribution, or
online at
http://secondlifegrid.net/programs/open_source/licensing/flossexception

By copying, modifying or distributing this software, you acknowledge
that you have read and understood your obligations described above,
and agree to abide by those obligations.

ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
COMPLETENESS OR PERFORMANCE.
$/LicenseInfo$
"""

import sys
import os.path

def add_indra_lib_path():
    root = os.path.realpath(__file__)
    # always insert the directory of the script in the search path
    dir = os.path.dirname(root)
    if dir not in sys.path:
        sys.path.insert(0, dir)

    # Now go look for indra/lib/python in the parent dies
    while root != os.path.sep:
        root = os.path.dirname(root)
        dir = os.path.join(root, 'indra', 'lib', 'python')
        if os.path.isdir(dir):
            if dir not in sys.path:
                sys.path.insert(0, dir)
            return root
    else:
        print >>sys.stderr, "This script is not inside a valid installation."
        sys.exit(1)

add_indra_lib_path()

import getopt, os, re, commands
from indra.util import llversion

svn = os.path.expandvars("${SVN}")
if not svn or svn == "${SVN}": svn = "svn"

def usage():
    print "Usage:"
    print sys.argv[0] + """ [options]

Options:
  --version
   Specify the version string to replace current version.
  --skip-on-branch
   Specify a regular expression against which the current branch
   is matched. If it matches, then leave version strings alone.
   Use this to avoid changing version strings on release candidate
   builds.
  --server
   Update llversionserver.h only with new version
  --viewer
   Update llversionviewer.h only with new version
  --channel
   Specify the viewer channel string to replace current channel.
  --server_channel
   Specify the server channel string to replace current channel.
  --src-root
   Location of source tree
  --verbose
  --help
   Print this message and exit.

Common Uses:
   # Update server and viewer build numbers to the current SVN revision:
   update_version_files.py

   # Update build numbers unless we are on a release branch:
   update_version_files.py --skip-on-branch='^Branch_'

   # Update server and viewer version numbers explicitly:
   update_version_files.py --version=1.18.1.6     
                               
   # Update just the viewer version number explicitly:
   update_version_files.py --viewer --version=1.18.1.6     

   # Update just the server build number to the current SVN revision:
   update_version_files.py --server
                               
   # Update the viewer channel
   update_version_files.py --channel="First Look Puppeteering"
                               
   # Update the server channel
   update_version_files.py --server_channel="Het Grid"
   
"""
def _getstatusoutput(cmd):
    """Return Win32 (status, output) of executing cmd
in a shell."""
    if os.path.sep != "/":
        # stupid #%#$$ windows
        cmd = 'cmd.exe /c "'+cmd+'"'
    pipe = os.popen(cmd, 'r')
    text = pipe.read()
    sts = pipe.close()
    if sts is None: sts = 0
    if text[-1:] == '\n': text = text[:-1]
    return sts, text

re_map = {}

#re_map['filename'] = (('pattern', 'replacement'),
#                      ('pattern', 'replacement')
re_map['indra/llcommon/llversionviewer.h'] = \
    (('const S32 LL_VERSION_MAJOR = (\d+);',
      'const S32 LL_VERSION_MAJOR = %(VER_MAJOR)s;'),
     ('const S32 LL_VERSION_MINOR = (\d+);',
      'const S32 LL_VERSION_MINOR = %(VER_MINOR)s;'),
     ('const S32 LL_VERSION_PATCH = (\d+);',
      'const S32 LL_VERSION_PATCH = %(VER_PATCH)s;'),
     ('const S32 LL_VERSION_BUILD = (\d+);',
      'const S32 LL_VERSION_BUILD = %(VER_BUILD)s;'),
     ('const char \* const LL_CHANNEL = "(.+)";',
      'const char * const LL_CHANNEL = "%(VIEWER_CHANNEL)s";'))
re_map['indra/newview/res/viewerRes.rc'] = \
    (('FILEVERSION [0-9,]+',
      'FILEVERSION %(VER_MAJOR)s,%(VER_MINOR)s,%(VER_PATCH)s,%(VER_BUILD)s'),
     ('PRODUCTVERSION [0-9,]+',
      'PRODUCTVERSION %(VER_MAJOR)s,%(VER_MINOR)s,%(VER_PATCH)s,%(VER_BUILD)s'),
     ('VALUE "FileVersion", "[0-9.]+"',
      'VALUE "FileVersion", "%(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s"'),
     ('VALUE "ProductVersion", "[0-9.]+"',
      'VALUE "ProductVersion", "%(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s"'))

# Trailing ',' in top level tuple is special form to avoid parsing issues with one element tuple
re_map['indra/newview/Info-SecondLife.plist'] = \
    (('<key>CFBundleVersion</key>\n\t<string>[0-9.]+</string>',
      '<key>CFBundleVersion</key>\n\t<string>%(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s</string>'),)

re_map['indra/newview/Info-Snowglobe.plist'] = \
    (('<key>CFBundleVersion</key>\n\t<string>[0-9.]+</string>',
      '<key>CFBundleVersion</key>\n\t<string>%(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s</string>'),)

# This will probably only work as long as InfoPlist.strings is NOT UTF16, which is should be...
re_map['indra/newview/English.lproj/InfoPlist.strings'] = \
    (('CFBundleShortVersionString = "Second Life version [0-9.]+";',
      'CFBundleShortVersionString = "Second Life version %(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s";'),
     ('CFBundleGetInfoString = "Second Life version [0-9.]+',
      'CFBundleGetInfoString = "Second Life version %(VER_MAJOR)s.%(VER_MINOR)s.%(VER_PATCH)s.%(VER_BUILD)s'))


version_re      = re.compile('(\d+).(\d+).(\d+).(\d+)')
svn_branch_re   = re.compile('^URL:\s+\S+/([^/\s]+)$', re.MULTILINE)
svn_revision_re = re.compile('^Last Changed Rev: (\d+)$', re.MULTILINE)

def main():
    script_path = os.path.dirname(__file__)
    src_root = script_path + "/../"
    verbose = False

    opts, args = getopt.getopt(sys.argv[1:],
                               "",
                               ['version=',
                                'channel=',
                                'server_channel=',
                                'skip-on-branch=',
                                'src-root=',
                                'verbose',
                                'viewer',
                                'help'])
    update_viewer = False
    new_version = None
    new_viewer_channel = None
    skip_on_branch_re = None
    for o,a in opts:
        if o in ('--version'):
            new_version = a
        if o in ('--skip-on-branch'):
            skip_on_branch_re = re.compile(a)
        if o in ('--channel'):
            new_viewer_channel = a
        if o in ('--src-root'):
            src_root = a
        if o in ('--verbose'):
            verbose = True
        if o in ('--server'):
            update_server = True
        if o in ('--viewer'):
            update_viewer = True
        if o in ('--help'):
            usage()
            return 0

    if not(update_viewer):
        update_viewer = True

    # Get current channel/version from llversion*.h
    try:
        viewer_channel = llversion.get_viewer_channel()
        viewer_version = llversion.get_viewer_version()
    except IOError:
        print "Viewer version file not present, skipping..."
        viewer_channel = None
        viewer_version = None
        update_viewer = False

    if verbose:
        print "Source Path:", src_root
        if viewer_channel != None:
            print "Current viewer channel/version: '%(viewer_channel)s' / '%(viewer_version)s'" % locals()
        print

    # Determine new channel(s)
    if new_viewer_channel != None and len(new_viewer_channel) > 0:
        viewer_channel = new_viewer_channel

    # Determine new version(s)
    if new_version:
        m = version_re.match(new_version)
        if not m:
            print "Invalid version string specified!"
            return -1
        if update_viewer:
            viewer_version = new_version
    else:
        # Assume we're updating just the build number
        cl = '%s info "%s"' % (svn, src_root)
        status, output = _getstatusoutput(cl)
        if verbose:
            print
            print "svn info output:"
            print "----------------"
            print output

        branch_match = svn_branch_re.search(output)
        revision_match = svn_revision_re.search(output)
        if not branch_match or not revision_match:
            print "Failed to execute svn info, output follows:"
            print output
            return -1
        branch = branch_match.group(1)
        revision = revision_match.group(1)
        if skip_on_branch_re and skip_on_branch_re.match(branch):
            print "Release Candidate Build, leaving version files untouched."
            return 0
        if update_viewer:
            m = version_re.match(viewer_version)
            viewer_version = m.group(1)+"."+m.group(2)+"."+m.group(3)+"."+revision

    if verbose:
        if update_viewer:
            print "Setting viewer channel/version: '%(viewer_channel)s' / '%(viewer_version)s'" % locals()
        print

    # split out version parts
    if viewer_version != None:
        m = version_re.match(viewer_version)
        VER_MAJOR = m.group(1)
        VER_MINOR = m.group(2)
        VER_PATCH = m.group(3)
        VER_BUILD = m.group(4)

    # For readability and symmetry with version strings:
    VIEWER_CHANNEL = viewer_channel

    # Iterate through all of the files in the map, and apply the
    # substitution filters
    for filename in re_map.keys():
        try:
            # Read the entire file into a string
            full_fn = src_root + '/' + filename
            file = open(full_fn,"r")
            file_str = file.read()
            file.close()

            if verbose:
                print "Processing file:",filename
            for rule in re_map[filename]:
                repl = rule[1] % locals()
                file_str = re.sub(rule[0], repl, file_str)

            file = open(full_fn,"w")
            file.write(file_str)
            file.close()
        except IOError:
            print "File %(filename)s not present, skipping..." % locals()
    return 0

main()

