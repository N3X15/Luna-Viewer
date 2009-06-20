cd indra
..\scripts\install.py --uninstall
rm -rf *.pdb/
cd ..
C:\cygwin\bin\find -name ".svn" -exec rm -rf {} +
C:\cygwin\bin\find -name "build-*" -exec rm -rf {} +
C:\cygwin\bin\find -name "*.bak" -exec rm -rf {} +
pause