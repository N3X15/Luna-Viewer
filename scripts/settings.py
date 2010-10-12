#!/usr/bin/env python
"""\
@file settings.py
@author N3X15 <nexisentertainment@gmail.com>
@date 2010-09-25
@brief Modify the settings files from the commandline.

$LicenseInfo:firstyear=2010&license=mit$

Copyright (c) 2008-2010 Luna Contributors

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

import sys
import os.path
import re

# Look for indra/lib/python in all possible parent directories ...
# This is an improvement over the setup-path.py method used previously:
#  * the script may blocated anywhere inside the source tree
#  * it doesn't depend on the current directory
#  * it doesn't depend on another file being present.

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

base_dir = add_indra_lib_path()

import copy
import optparse
import os
import platform
import pprint
import shutil
import tempfile
import urllib2
import urlparse

from indra.base import llsd
from indra.util import helpformatter

class Settings(object):
    "Dicks"
    def __init__(self,path):
        self._filename=path
        self.load()
        
    def load(self):
        self.stuff = llsd.parse(file(self._filename, 'rb').read())
    
    def save(self,filename):
        print ' * Writing state to %s...' % filename
        newstuff={}
        for name in sorted(self.stuff):
            newstuff[name]=self.stuff[name]
        self.stuff=newstuff
        file(filename, 'wb').write(llsd.format_pretty_xml(self.stuff))

    def copy(self,sFrom,key):
        self.stuff = sFrom.stuff[key]

    def setType(self,key,t):
        try:
            self.stuff[key]['Type']=t
            value=self.stuff[key]['Value']
            if t=='Boolean':
                if str(value).lower()=='true' or value==1:
                    value=1
                else:
                    value=0
            elif t=='S32' or t=='U32':
                value=int(value)
            elif t=='F32':
                value=float(value)
            elif t=='String':
                value=str(value)
            self.stuff[key]['Value']=value
        except KeyError:
            print "Key %s doesn't exist." % key

    def getType(self,key):
        try:
            return self.stuff[key]['Type']
        except KeyError:
            print "Key %s doesn't exist." % key

    def setPersist(self,key,on):
        try:
            self.stuff[key]['Persist']=on
        except KeyError:
            print "Key %s doesn't exist." % key

    def get(self,key):
        try:
            return self.stuff[key]['Value']
        except KeyError:
            print "Key %s doesn't exist." % key
        
    def set(self,key,value):
        try:
            self.stuff[key]['Value']=value
        except KeyError:
            """\
              <key>Comment</key>
              <string>Color of chat messages from friends</string>
              <key>Persist</key>
              <integer>1</integer>
              <key>Type</key>
              <string>Color4</string>
              <key>Value</key>
              <array>
                <real>0.447</real>
                <real>0.784</real>
                <real>0.663</real>
                <real>1.0</real>
              </array>
            """
            assumed_type=StringToLLSDType(value)

            if assumed_type is "String":
                d="[-+]?\d+"
                real="([-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?)"
                # Test for boolean.
                if value.lower() == 'true' or value.lower() == 'false':
                    if value.lower()=='true':
                        value=1
                    else:
                        value=0
                    assumed_type='BOOL'
                else:
                    # Test for a vector (<x,y,z>)
                    vm=re.match("<"+real+","+real+","+real+">",value)
                    if vm is not None:
                        # Yay, a vector.  Or RGB.  Let's assume Color3.
                        assumed_type="Color3"
                        value = vm.group(1,2,3)
                    else:
                        # Test for a rect
                        vm=re.match("<"+d+","+d+","+d+","+d+">",value)
                        if vm is not None:
                            assumed_type="Rect"
                            value = vm.group(1,2,3,4)
                        else:
                            # Test for a Color4
                            vm=re.match("<"+real+","+real+","+real+","+real+">",value)
                            if vm is not None:
                                assumed_type="Rect"
                                value = vm.group(1,2,3,4)
   
            print 'WARNING: Input for %s is assumed to be a %s. Please use --set-type to fix.' % (key,assumed_type)
            self.stuff[key]={
                'Comment':'Automatically generated by scripts/settings.py',
                'Persist':1,
                'Type':assumed_type,
                'Value':value
                }
        
    def remove(self,key):
        newstuff={}
        for name in sorted(self.stuff):
            if name!=key:
                newstuff[name]=self.stuff[name]
        self.stuff=newstuff
        

def parse_args():
    parser = optparse.OptionParser(
        usage="usage: %prog [options]",
        formatter = helpformatter.Formatter(),
        description="""This script fucks with settings.xml and company. :V
        
        by N3X15, for Luna
""")
    parser.add_option(
        '--sort', 
        action='store_true',
        default=False,
        dest='sort',
        help='Sort shit')
    
    parser.add_option(
        '--target-file', 
        type='string',
        default=os.path.join(base_dir, 'indra','newview','app_settings','settings.xml'),
        dest='filename',
        help='The file getting raped')
    
    parser.add_option(
        '--source-file', 
        type='string',
        default='',
        dest='src_filename',
        help='The file we --cp stuff from')

    parser.add_option(
        '--set',
        type='string',
        default='',
        dest='set_llsd',
        help='key=value;key2=value...')
        
    parser.add_option(
        '--rm',
        type='string',
        default='',
        dest='remove_keys',
        help='Removes a key (comma-separated list)')
        
    parser.add_option(
        '--cp',
        type='string',
        default='',
        dest='copy_keys',
        help='Copies keys from --source-file to --target-file.  (comma-separated list)')
        
    parser.add_option(
        '--mv',
        type='string',
        default='',
        dest='move_keys',
        help='Moves keys from --source-file to --target-file.  (comma-separated list)')

    parser.add_option(
        "--import-from",
        type="string",
        default='',
        dest='import_from',
        help='Compare settings and bring over new keys')

    parser.add_option(
        "--key-replace",
        type="string",
        default='',
        dest='key_replace',
        help='(format: OLD=NEW;OLD2=NEW2) Find any instances of OLD and replace it with NEW in key names.')
    
    parser.add_option(
        '--find',
        type='string',
        default='',
        dest='find_key',
        help='Finds keys matching the indicated substring')

    parser.add_option(
        '--find-action',
        type="string",
        default='print',
        dest='find_action',
        help='What to do after finding a key? (print, remove, copy)')

    parser.add_option(
        '--set-type',
        type="string",
        default='',
        dest='set_type',
        help="Set KEY to TYPE (KEY=TYPE)")

    parser.add_option(
        '-P','--full-print',
        action="store_true",
        default=False,
        dest='fullprint',
        help="Print the type and value of a key when using --find.")

    parser.add_option(
        '-G','--get',
        type="string",
        default=False,
        dest='get_value',
        help="Print the value of a key.")
    
    return parser.parse_args()
    
# Ugly as sin, but it should work.
def StringToType(input):
    try:
        return int(input)
    except ValueError:
        try:
            return float(input)
        except ValueError:
            try:
                return long(input)
            except ValueError:
                return input
                
def StringToLLSDType(input):
    try:
        int(input)
        return "S32"
    except ValueError:
        try:
            float(input)
            return "F32"
        except ValueError:
            return "String"

if __name__ == '__main__':
    import sys

    try:
        options, args = parse_args()
        hurr = Settings(options.filename)
        changed=False

        if options.sort:
            changed=True
                
        if len(options.import_from) > 0:
            new = Settings(options.import_from)
            newstuff = hurr.stuff
            print 'Importing settings from %s...' % options.import_from
            for key in sorted(new.stuff):
                if key not in newstuff:
                    print " * NEW: %s" % key
                    newstuff[key]=new.stuff[key]
                    changed=True
                else:
                    if newstuff[key] != new.stuff[key]:
                        print " * CHANGED: %s" % key
            hurr.stuff=newstuff
            
        if len(options.set_llsd) > 0:
            for kvp in options.set_llsd.split(";"):
                chunks = kvp.split('=')
                key = chunks[0]
                value = StringToType(chunks[1])
                print ' * Setting %s=%s...' % (key,value)
                hurr.set(key,value)
                changed=True
            
        if len(options.remove_keys) > 0:
            for key in options.remove_keys.split(";"):
                print ' * Removing %s...' % key
                hurr.remove(key)
                changed=True

        # --key-replace=Phoenix=Ascent;Beer=BongWater
        ReplaceRules={}
        if len(options.key_replace) > 0:
            for chunk in options.key_replace.split(";"):
                chunkparts=chunk.split('=')
                old = chunkparts[0]
                new = chunkparts[1]
                newstuff = hurr.stuff
                for key in sorted(hurr.stuff):
                    newkey = key.replace(old,new)
                    if(key!=newkey):
                        if newkey in newstuff:
                            print ' ! Overwriting %s with new data.' % newkey
                        print ' * Renaming ',key,'->',newkey
                        newnewstuff={}
                        for name in sorted(newstuff):
                            if name!=key:
                                newnewstuff[name]=newstuff[name]
                            else:
                                newnewstuff[newkey]=newstuff[name]
                        newstuff=newnewstuff
                hurr.stuff=newstuff
                changed=True
                
        if len(options.set_type) > 0:
            for chunk in options.set_type.split(";"):
                chunkparts=chunk.split('=')
                key = chunkparts[0]
                typ = chunkparts[1]
                print ' * Setting %s to type %s...' % (key,typ)
                hurr.setType(key,typ)
                changed=True
                
        if len(options.find_key) > 0:
            for key in hurr.stuff:
                if re.search(options.find_key,key) is not None:
                    if options.find_action=='remove':
                        print ' * Removing %s!' % key
                        hurr.remove(key)
                        changed=True
                    else:
                        if options.fullprint:
                            print '%s: %s = %s' % (key.rjust(40),hurr.getType(key).ljust(10),hurr.get(key))
                        else:
                            print key
                            
        #Save changes
        if changed:
            hurr.save(options.filename)
        
#   except AttributeError:
#       print >> sys.stderr, 'Error: malformatted file: ', name
#       sys.exit(1)

    except IndexError:
        print >> sys.stderr, ('Usage: %s ../settings.xml' % sys.argv[0])
