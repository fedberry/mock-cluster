#!/bin/sh

echo "Clean QA settings when removed"

#remove database
mysqladmin -uroot -f drop $DATABASE_NAME
mysql -u root mysql -e "DROP USER '"$DATABASE_USER"'@'localhost';"

#remove apache config
rm -f $HOME/conf.d/$DOMAIN.conf

#remove DOCROOT
chmod -R 755 $DOCROOT/files
rm -rf $DOCROOT
sudo apachectl restart

