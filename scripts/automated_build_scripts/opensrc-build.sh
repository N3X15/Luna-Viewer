#!/bin/sh

# This is the build script used by Linden Lab's autmated build system.
#

set -x

export INSTALL_USE_HTTP_FOR_SCP=true
export PATH=/bin:/usr/bin:$PATH
arch=`uname | cut -b-6`
here=`echo $0 | sed 's:[^/]*$:.:'`
year=`date +%Y`
branch=`svn info | grep '^URL:' | sed 's:.*/::'`
revision=`svn info | grep '^Revision:' | sed 's/.*: //'`
top=`cd "$here/../../.." && pwd`

[ x"$WGET_CACHE" = x ] && export WGET_CACHE=/var/tmp/parabuild/wget
[ x"$S3GET_URL" = x ]  && export S3GET_URL=http://viewer-source-downloads.s3.amazonaws.com/$year
[ x"$S3PUT_URL" = x ]  && export S3PUT_URL=https://s3.amazonaws.com/viewer-source-downloads/$year
[ x"$S3SYMBOL_URL" = x ]  && export S3SYMBOL_URL=https://s3.amazonaws.com/automated-builds-secondlife-com/binaries
[ x"$PUBLIC_URL" = x ] && export PUBLIC_URL=http://secondlife.com/developers/opensource/downloads/$year
[ x"$PUBLIC_EMAIL" = x ] && export PUBLIC_EMAIL=sldev-commits@lists.secondlife.com
# Make sure command worked and bail out if not, reporting failure to parabuild
fail()
{
  release_lock
  echo "BUILD FAILED" $@
  exit 1
}
  
pass() 
{ 
  release_lock
  echo "BUILD SUCCESSFUL"
  exit 0
}

# Locking to avoid contention with u-s-c
LOCK_CREATE=/usr/bin/lockfile-create
LOCK_TOUCH=/usr/bin/lockfile-touch
LOCK_REMOVE=/usr/bin/lockfile-remove
LOCK_PROCESS=

locking_available()
{
  test -x "$LOCK_CREATE"\
    -a -x "$LOCK_TOUCH"\
    -a -x "$LOCK_REMOVE"
}

acquire_lock()
{
  if locking_available 
  then
    if "$LOCK_CREATE" /var/lock/update-system-config --retry 99
    then
      "$LOCK_TOUCH" /var/lock/update-system-config &
      LOCK_PROCESS="$!"
    else
      fail acquire lock
    fi
  else
    true
  fi
}

release_lock()
{
  if locking_available
  then
    if test x"$LOCK_PROCESS" != x
    then
      kill "$LOCK_PROCESS"
      "$LOCK_REMOVE" /var/lock/update-system-config
    else
      echo No Lock Acquired >&2
    fi
  else
    true
  fi
}

get_asset()
{
  mkdir -p "$WGET_CACHE" || fail creating WGET_CACHE
  local tarball=`basename "$1"`
  test -r "$WGET_CACHE/$tarball" || ( cd "$WGET_CACHE" && curl --location --remote-name "$1" || fail getting $1 )
  case "$tarball" in
  *.zip) unzip -qq -d "$top" -o "$WGET_CACHE/$tarball" || fail unzip $tarball ;;
  *.tar.gz|*.tgz) tar -C "$top" -xzf  "$WGET_CACHE/$tarball" || fail untar $tarball ;;
  *) fail unrecognized filetype: $tarball ;;
  esac
}

s3_available()
{
  test -x "$helpers/s3get.sh" -a -x "$helpers/s3put.sh" -a -r "$helpers/s3curl.pl"
}

build_dir_Darwin()
{
  echo build-darwin-universal
}

build_dir_Linux()
{
  echo viewer-linux-i686-`echo $1 | tr A-Z a-z`
}

build_dir_CYGWIN()
{
  echo build-vc80
}

installer_Darwin()
{
  ls -1td "`build_dir_Darwin Release`/newview/"*.dmg 2>/dev/null | sed 1q
}

installer_Linux()
{
  ls -1td "`build_dir_Linux Release`/newview/"*.tar.bz2 2>/dev/null | sed 1q
}

installer_CYGWIN()
{
  d=`build_dir_CYGWIN Release`
  p=`sed 's:.*=::' "$d/newview/Release/touched.bat"`
  echo "$d/newview/Release/$p"
}

# deal with aborts etc..
trap fail 1 2 3 14 15

# Check location
cd "$here/../.."

test -x ../linden/scripts/automated_build_scripts/opensrc-build.sh\
 || fail 'The parent dir of your checkout needs to be named "linden"' 

. doc/asset_urls.txt
get_asset "$SLASSET_ART"

update_version_files=

# Set up platform specific stuff
case "$arch" in

# Note that we can only build the "Release" variant for Darwin, because of a compiler bug:
# ld: bl out of range (-16777272 max is +/-16M)
#  from __static_initialization_and_destruction_0(int, int)at 0x033D319C
#  in __StaticInit of
#  indra/build-darwin-universal/newview/SecondLife.build/Debug/Second Life.build/Objects-normal/ppc/llvoicevisualizer.o
#  to ___cxa_atexit$island_2 at 0x023D50F8
#  in __text of
#  indra/build-darwin-universal/newview/SecondLife.build/Debug/Second Life.build/Objects-normal/ppc/Second Life
#  in __static_initialization_and_destruction_0(int, int)
#  from indra/build-darwin-universal/newview/SecondLife.build/Debug/Second Life.build/Objects-normal/ppc/llvoicevisualizer.o

Darwin)
  helpers=/usr/local/buildscripts/shared/latest
  variants="Release"
  cmake_generator="Xcode"
  fmod=fmodapi375mac
  fmod_tar="$fmod.zip"
  fmod_so=libfmod.a
  fmod_lib=lib
  target_dirs="libraries/universal-darwin/lib_debug
               libraries/universal-darwin/lib_release
               libraries/universal-darwin/lib_release_client"
  other_archs="$S3GET_URL/$branch/$revision/CYGWIN $S3GET_URL/$branch/$revision/Linux"
  symbolfiles=
  mail="$helpers"/mail.py
  all_done="$helpers"/all_done.py
  test -r "$helpers/update_version_files.py" && update_version_files="$helpers/update_version_files.py"
  libs_asset="$SLASSET_LIBS_DARWIN"
  ;;

CYGWIN)
  helpers=/cygdrive/c/buildscripts/shared/latest
  variants="Debug RelWithDebInfo Release"
  #variants="Release"
  cmake_generator="vc80"
  fmod=fmodapi375win
  fmod_tar=fmodapi375win.zip
  fmod_so=fmodvc.lib
  fmod_lib=lib
  target_dirs="libraries/i686-win32/lib/debug
               libraries/i686-win32/lib/release"
  other_archs="$S3GET_URL/$branch/$revision/Darwin $S3GET_URL/$branch/$revision/Linux"
  symbolfiles="newview/Release/secondlife-bin.pdb newview/Release/secondlife-bin.map newview/Release/secondlife-bin.exe"
  export PATH="/cygdrive/c/Python25:/cygdrive/c/Program Files/Cmake 2.6/bin":$PATH
  export PERL="/cygdrive/c/Perl/bin/perl.exe"
  export S3CURL="C:\\buildscripts\\shared\\latest\\hg\\bin\\s3curl.py"
  export SIGN_PY="C:\\buildscripts\\shared\\latest\\code-signing\\sign.py"
  export CURL="C:\\cygwin\\bin\\curl.exe"
  mail="C:\\buildscripts\\shared\\latest\\mail.py"
  all_done="C:\\buildscripts\\shared\\latest\\all_done.py"
  test -r "$helpers/update_version_files.py" && update_version_files="C:\\buildscripts\\shared\\latest\\update_version_files.py"
  libs_asset="$SLASSET_LIBS_WIN32"
  ;;

Linux)
  helpers=/var/opt/parabuild/buildscripts/shared/latest
  if [ x"$CXX" = x ]
  then
    if test -x /usr/bin/g++-4.1
	then
	  if test -x /usr/bin/distcc
	  then
	    export CXX="/usr/bin/distcc /usr/bin/g++-4.1"
	  else
	    export CXX=/usr/bin/g++-4.1
	  fi
	fi
  fi
  acquire_lock
  variants="Debug RelWithDebInfo Release"
  #variants="Release"
  cmake_generator="Unix Makefiles"
  fmod=fmodapi375linux
  fmod_tar="$fmod".tar.gz
  fmod_so=libfmod-3.75.so
  fmod_lib=.
  target_dirs="libraries/i686-linux/lib_debug
               libraries/i686-linux/lib_release
               libraries/i686-linux/lib_release_client"
  other_archs="$S3GET_URL/$branch/$revision/Darwin $S3GET_URL/$branch/$revision/CYGWIN"
  symbolfiles=
  mail="$helpers"/mail.py
  all_done="$helpers"/all_done.py
  test -r "$helpers/update_version_files.py" && update_version_files="$helpers/update_version_files.py"
  # Change the DISTCC_DIR to be somewhere that the parabuild process can write to
  if test -r /etc/debian_version
  then
    [ x"$DISTCC_DIR" = x ] && export DISTCC_DIR=/var/tmp/parabuild
    #case `cat /etc/debian_version` in
    #3.*) [ x"$DISTCC_HOSTS" = x ]\
    #     && export DISTCC_HOSTS="build-linux-1/3
    #                              station30/2,lzo" ;;
    #4.*) [ x"$DISTCC_HOSTS" = x ]\
    #     && export DISTCC_HOSTS="build-linux-6/2,lzo
    #                             build-linux-2/2,lzo
    #                             build-linux-3/2,lzo
    #                             build-linux-4/2,lzo
    #                             build-linux-5/2,lzo
    #                             build-linux-7/2,lzo
    #                             build-linux-8/2,lzo
    #                             build-linux-9/2,lzo" ;;
    #esac
    # Temp fix for Linux so that parabuild passes: use the new Linux build farm
    export hostname=`hostname -f`
    export phx_DISTCC_HOSTS="build-linux0.phx.lindenlab.com/2 build-linux1.phx.lindenlab.com/2 build-linux2.phx.lindenlab.com/2 build-linux3.phx.lindenlab.com/2 build-linux5.phx.lindenlab.com/2 build-linux5.phx.lindenlab.com/2 build-linux6.phx.lindenlab.com/2 "
    export dfw_DISTCC_HOSTS="build-linux7.dfw.lindenlab.com/2 build-linux8.dfw.lindenlab.com/2 build-linux9.dfw.lindenlab.com/2 build-linux10.dfw.lindenlab.com/2 build-linux11.dfw.lindenlab.com/2 build-linux12.dfw.lindenlab.com/2 build-linux13.dfw.lindenlab.com/2 build-linux14.dfw.lindenlab.com/2 build-linux15.dfw.lindenlab.com/2"
    case "$hostname" in
    *.dfw.*) export DISTCC_HOSTS="$dfw_DISTCC_HOSTS" ;;
    *.phx.*) export DISTCC_HOSTS="$phx_DISTCC_HOSTS" ;;
    esac
  fi

  libs_asset="$SLASSET_LIBS_LINUXI386"
  ;;

*) fail undefined $arch ;;
esac

get_asset "http://www.fmod.org/index.php/release/version/$fmod_tar"

case "$arch" in

Darwin)
  # Create fat binary on Mac...
  if lipo -create -output "../$fmod"/api/$fmod_lib/libfmod-universal.a\
     "../$fmod"/api/$fmod_lib/libfmod.a\
     "../$fmod"/api/$fmod_lib/libfmodx86.a
  then
    mv "../$fmod"/api/$fmod_lib/libfmod.a "../$fmod"/api/$fmod_lib/libfmodppc.a
    mv "../$fmod"/api/$fmod_lib/libfmod-universal.a "../$fmod"/api/$fmod_lib/libfmod.a
    echo Created fat binary
  else
    fail running lipo
  fi
  ;;

CYGWIN)
  # install Quicktime.  This will fail outside of Linden's network
  scripts/install.py quicktime
  ;;

esac

# Only run this if the script exists
if test x"$update_version_files" = x 
then
  echo "Private Build..." > indra/build.log
  [ x"$VIEWER_CHANNEL" = x ] && export VIEWER_CHANNEL="CommunityDeveloper"
else
  # By right, this should be in the branched source tree, but for now it will be a helper
  python "$update_version_files" --verbose --src-root=. --viewer > indra/build.log
  [ x"$VIEWER_CHANNEL" = x ] && export VIEWER_CHANNEL="Snowglobe Test Build"
fi

# First, go into the directory where the code was checked out by Parabuild
cd indra

# This is the way it works now, but it will soon work on a variant dependent way
for target_dir in $target_dirs
do
  mkdir -p "../$target_dir"
  cp -f "../../$fmod/api/$fmod_lib/$fmod_so"  "../$target_dir"
done
mkdir -p "../libraries/include"
cp -f "../../$fmod/api/inc/"*  "../libraries/include"

# Special Windows case
test -r "../../$fmod/api/fmod.dll" && cp -f "../../$fmod/api/fmod.dll" newview

# Now run the build command over all variants
succeeded=true

### TEST CODE - remove when done
### variants=
### echo "Artificial build failure to test notifications" > build.log
### succeeded=false
### END TEST CODE

for variant in $variants
do
  build_dir=`build_dir_$arch $variant`
  rm -rf "$build_dir"
  get_asset "$libs_asset" # Thus plunks stuff into the build dir, so have to restore it now.
  # This is the way it will work in future
  #for target_dir in $target_dirs
  #do
  #  mkdir -p "$build_dir/$target_dir"
  #  cp "../../$fmod/api/$fmod_lib/$fmod_so"  "$build_dir/$target_dir"
  #done
  #mkdir -p "$build_dir/libraries/include"
  #cp "../../$fmod/api/inc/"*  "$build_dir/libraries/include"
  echo "==== $variant ====" >> build.log
  if ./develop.py \
    --unattended \
    --incredibuild \
    -t $variant \
    -G "$cmake_generator" \
   configure \
    -DVIEWER_CHANNEL:STRING="$VIEWER_CHANNEL"\
    -DVIEWER_LOGIN_CHANNEL:STRING="$VIEWER_CHANNEL"\
    -DPACKAGE:BOOL=ON >>build.log 2>&1
  then
    if ./develop.py\
         --unattended\
         --incredibuild \
         -t $variant\
         -G "$cmake_generator" \
       build prepare >>build.log 2>&1
    then
      if ./develop.py\
           --unattended\
           --incredibuild \
           -t $variant\
           -G "$cmake_generator" \
         build package >>build.log 2>&1
	  then
        # run tests if needed
        true
	  else
	    succeeded=false
	  fi
    else
      succeeded=false
    fi
  else
    succeeded=false
  fi
done

# check statuis and upload results to S3
subject=
if $succeeded
then
  package=`installer_$arch`
  test -r "$package" || fail not found: $package
  package_file=`echo $package | sed 's:.*/::'`
  if s3_available
  then
    echo "$PUBLIC_URL/$branch/$revision/$package_file" > "$arch"
    echo "$PUBLIC_URL/$branch/$revision/good-build.$arch" >> "$arch"
    "$helpers/s3put.sh" "$package" "$S3PUT_URL/$branch/$revision/$package_file"    binary/octet-stream\
       || fail Uploading "$package"
    "$helpers/s3put.sh" build.log  "$S3PUT_URL/$branch/$revision/good-build.$arch" text/plain\
       || fail Uploading build.log
    "$helpers/s3put.sh" "$arch"    "$S3PUT_URL/$branch/$revision/$arch"            text/plain\
       || fail Uploading token file
    for symbolfile in $symbolfiles
    do
      targetfile="`echo $symbolfile | sed 's:.*/::'`"
      "$helpers/s3put.sh" "$build_dir/$symbolfile" "$S3SYMBOL_URL/$revision/$targetfile" binary/octet-stream\
        || fail Uploading "$symbolfile"
    done
    if python "$all_done"\
          curl\
         "$S3GET_URL/$branch/$revision/$arch"\
          $other_archs > message
    then
      subject="Successful Build for $branch ($revision)"
    fi
  else
    true s3 is not available
  fi
else
  if s3_available
  then
    "$helpers/s3put.sh" build.log "$S3PUT_URL/$branch/$revision/failed-build.$arch" text/plain\
       || fail Uploading build.log
    subject="Failed Build for $branch ($revision) on $arch"
    cat >message <<EOF
Build for $branch ($revision) failed for $arch.
Please see the build log for details:

$PUBLIC_URL/$branch/$revision/failed-build.$arch

EOF
  else
    true s3 is not available
  fi
fi

# We have something to say...
if [ x"$subject" != x ]
then
  # Extract change list since last build
  if [ x"$PARABUILD_CHANGE_LIST_NUMBER" = x ]
  then
    echo "No change information available" >> message
  elif [ x"$PARABUILD_PREVIOUS_CHANGE_LIST_NUMBER" = x ]
  then
    ( cd .. && svn log --verbose --stop-on-copy --limit 50 ) >>message
  else
    if [ "$PARABUILD_PREVIOUS_CHANGE_LIST_NUMBER" -lt "$PARABUILD_CHANGE_LIST_NUMBER" ]
	then
	  range=`expr 1 + "$PARABUILD_PREVIOUS_CHANGE_LIST_NUMBER"`:"$PARABUILD_CHANGE_LIST_NUMBER"
	else
	  range="$PARABUILD_CHANGE_LIST_NUMBER"
	fi
    ( cd .. && svn log --verbose -r"$range" ) >>message
  fi
  # $PUBLIC_EMAIL can be a list, so no quotes
  python "$mail" "$subject" $PUBLIC_EMAIL <message
fi

if $succeeded
then
  pass
else
  fail
fi

