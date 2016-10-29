#!/bin/sh

CURRENT_DIR=`pwd`

export DOCROOT="$CURRENT_DIR/.devel/docroot"
export DATABASE_NAME="docs_devel"
export DATABASE_USER="docs_devel"
export DATABASE_PASS="docs_devel"
export DATABASE_SERVER="127.0.0.1"

export ACCOUNT_MAIL="admin@zen.ci"
export ACCOUNT_USER="admin"
export ACCOUNT_PASS="123"
export SITE_MAIL="noreply@zen.ci"
export SITE_NAME="localcopy docs.zen.ci"
export B="php $CURRENT_DIR/.devel/b/b.php"
export ZENCI_DEPLOY_DIR="$CURRENT_DIR"
export DEPLOY_DIR="$CURRENT_DIR"

if [ ! -d "$DOCROOT" ]; then
  mkdir -p $DOCROOT
  
  #prepare database access
  mysqladmin -uroot create $DATABASE_NAME
  mysql -u root mysql -e "CREATE USER '"$DATABASE_USER"'@'localhost';"
  mysql -u root mysql -e "GRANT ALL ON $DATABASE_NAME.* TO '"$DATABASE_USER"'@'localhost' IDENTIFIED BY '"$DATABASE_PASS"';"
  
  echo "$DATABASE_PASS" > /tmp/$DOMAIN.pass
  export DATABASE_PASS_FILE="/tmp/$DOMAIN.pass"
  
  #clone backdrop-contrib/b
  cd "$CURRENT_DIR/.devel/"
  git clone https://github.com/backdrop-contrib/b.git
  
  #init
  sh $CURRENT_DIR/scripts/deploy_init.sh
  cd $DOCROOT
  ln -s $DEPLOY_DIR/scripts/routing.php ./
  cd $CURRENT_DIR
else
  #update B project
  echo "Update B code to latest"
  cd "$CURRENT_DIR/.devel/b"
  git pull
fi

sh $CURRENT_DIR/scripts/deploy_update.sh

cd $DOCROOT
php -S localhost:8080 routing.php
