
if not "%1"=="VC80" if not "%1"=="VC71" goto error
set i=indra/newview
set _=indra/build-%1/newview/RelWithDebInfo/

cp -rv	%i%/app_settings/	%_%
cp	%i%/dronesettings.xml	%_%
cp -rv	%i%/character/		%_%
cp -rv	%i%/fonts/		%_%
cp -rv	%i%/lua/		%_%
cp -rv	%i%/mozilla-theme/	%_%
cp -rv	%i%/skins/		%_%
cp -rv	%i%/vivox-runtime/	%_%
cp	%i%/featuretable.txt	%_%
cp	%i%/gpu_table.txt	%_%
cp	%i%/licenses-win32.txt	%_%
cp	%i%/releasenotes.txt	%_%
goto end
:error
echo Argument 1 needs to be either VC80 or VC71.
:end