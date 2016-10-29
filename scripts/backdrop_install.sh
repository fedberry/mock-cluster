#!/bin/sh

echo "Installing backdrop to " . $DOCROOT

# Go to domain directory.
cd $DOCROOT

#download latest backdrop CMS.
$B -y dl backdrop

mv backdrop/* ./
mv backdrop/.htaccess ./

rm -rf backdrop

# set config directory.
cat >> settings.php  <<_EOF
\$config_directories['active'] = '$DOCROOT/files/config/active';
\$config_directories['staging'] = '$DOCROOT/files/config/staging';
_EOF


# Install Backdrop.
DATABASE_PASS=`cat $DATABASE_PASS_FILE`
$B si --account-mail=$ACCOUNT_MAIL --account-name=$ACCOUNT_USER --account-pass="$ACCOUNT_PASS" --site-mail=$SITE_MAIL --site-name="$SITE_NAME" --db-url=mysql://$DATABASE_USER:$DATABASE_PASS@$DATABASE_SERVER/$DATABASE_NAME --root=$DOCROOT

