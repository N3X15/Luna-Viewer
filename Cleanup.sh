svn status | grep "\!" | cut -d" " -f8 | xargs svn delete
svn status | grep "\?" | grep ".h\$"   | grep "indra" |  cut -d" " -f8 | xargs svn add
svn status | grep "\?" | grep ".cpp\$" | grep "indra" |  cut -d" " -f8 | xargs svn add
svn status | grep "\?" | grep ".xml\$" | grep "indra" |  cut -d" " -f8 | xargs svn add
svn status | grep "\?" | grep ".txt\$" | grep "indra" |  cut -d" " -f8 | xargs svn add
svn status | grep "\?" | grep ".ini\$" | grep "indra" |  cut -d" " -f8 | xargs svn add
find -name "Thumbs.db" -print | xargs svn delete
. propset.sh
