"""@file llversion.py
@brief Utility for parsing llcommon/llversion${server}.h
       for the version string and channel string
       Utility that parses svn info for branch and revision

$LicenseInfo:firstyear=2006&license=mit$

Copyright (c) 2006-2009, Linden Research, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
$/LicenseInfo$
"""

import re, sys, os, commands
import StringIO
import ConfigParser
from indra.base import config, llsd

# Methods for gathering version information from indra/Version

def get_src_root():
    indra_lib_python_indra_path = os.path.dirname(__file__)
    return os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(os.path.realpath(indra_lib_python_indra_path)))))))

def get_version_file_contents():
    filepath = os.path.join(get_src_root(),'indra', 'Version')
    file = open(filepath,"r")
    file_str = file.read()
    file.close()
    return file_str

def get_version(version_type, revision=0):
    config_parser = ConfigParser.ConfigParser()
    file_str = os.linesep.join(['[versions]', get_version_file_contents()])
    try:
        config_parser.readfp(StringIO.StringIO(file_str))

        if version_type == 'viewer':
            version = config_parser.get('versions', 'VERSION_VIEWER')
        elif version_type == 'server':
            version = config_parser.get('versions', 'VERSION_SERVER')
        else:
            raise Exception('Invalid version type "%s"' % version_type)
        if revision:
            return '.'.join([version, revision])
        else:
            return '.'.join([version, get_svn_revision()])
    except:
        raise
        # raise Exception("Unable to parse version file.")

def get_channel(version_type):
    if version_type == 'viewer':
        settings_file = open(os.path.join(
            get_src_root(), 'indra', 'newview', 'app_settings', 'settings.xml'))
        data = llsd.parse(settings_file.read())
        settings_file.close()
        return data['VersionChannelName']['Value']
    
    config.load()
    return config.get('channel', 'Second Life Server')

def get_viewer_version(revision=0):
    return get_version('viewer', revision)

def get_server_version(revision=0):
    return get_version('server', revision)

def get_viewer_channel():
    return get_channel('viewer')

def get_server_channel():
    return get_channel('server')

# Methods for gathering subversion information
def get_svn_status_matching(regular_expression):
    # Get the subversion info from the working source tree
    # *TODO: This does not work if the user only has TortoiseSVN installed.
    status, output = commands.getstatusoutput('svn info %s' % get_src_root())
    # Return revision zero if not in svn -- CG 2009-09-17
    m = regular_expression.search(output)
    if not m:
        #print "Failed to parse svn info output, resultfollows:"
        #print output
        return '0'
    return m.group(1)

def get_svn_branch():
    branch_re = re.compile('URL: (\S+)')
    return get_svn_status_matching(branch_re)

def get_svn_revision():
    last_rev_re = re.compile('Last Changed Rev: (\d+)')
    return get_svn_status_matching(last_rev_re)


