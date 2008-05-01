#/bin/sh

echo "syncing tiddlywikis"
rsync -rLz --partial --del --exclude backups wiki/ lumiera.org:/var/www/wiki

echo "syncing doxygen documentation"
rsync -rz --partial --del doc/devel/html/ lumiera.org:/var/www/doxy

