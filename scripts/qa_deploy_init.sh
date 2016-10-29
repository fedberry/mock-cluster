#!/bin/sh

#prepare database access
mysqladmin -uroot create $DATABASE_NAME
mysql -u root mysql -e "CREATE USER '"$DATABASE_USER"'@'localhost';"
mysql -u root mysql -e "GRANT ALL ON $DATABASE_NAME.* TO '"$DATABASE_USER"'@'localhost' IDENTIFIED BY '"$DATABASE_PASS"';"

echo "$DATABASE_PASS" > /tmp/$DOMAIN.pass
export DATABASE_PASS_FILE="/tmp/$DOMAIN.pass"

#prepare DOCROOT
mkdir $DOCROOT

#prepare apache config and restart it.
cat $HOME/conf.d/template|sed 's/{$DOMAIN}/'$DOMAIN'/g' > $HOME/conf.d/$DOMAIN.conf
sudo apachectl restart

#install drupal
sh $ZENCI_DEPLOY_DIR/scripts/backdrop_install.sh

echo "Full site path: $DOCROOT"

# Go to domain directory.
cd $DOCROOT

echo "Link documentation"
ln -s $ZENCI_DEPLOY_DIR/Documentation $DOCROOT/files/Documentation


echo "Prepare directories for contrib"

mkdir -p $DOCROOT/modules/contrib
mkdir -p $DOCROOT/themes/contrib
mkdir -p $DOCROOT/layouts/contrib
mkdir -p $DOCROOT/libraries/contrib

echo "Prepare files directory"
mkdir -p $DOCROOT/files/private


cd $DOCROOT/files
ln -s $ZENCI_DEPLOY_DIR/files ./github


cp $ZENCI_DEPLOY_DIR/settings/config/config.htaccess $DOCROOT/files/config/.htaccess

cd $DOCROOT/modules
ln -s $ZENCI_DEPLOY_DIR/modules ./custom

cd $DOCROOT/themes
ln -s $ZENCI_DEPLOY_DIR/themes ./custom

cd $DOCROOT/layouts
ln -s $ZENCI_DEPLOY_DIR/layouts ./custom

cd $DOCROOT/libraries
ln -s $ZENCI_DEPLOY_DIR/libraries ./custom
