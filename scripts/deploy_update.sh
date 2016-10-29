#!/bin/sh

echo "Full site path: $DOCROOT"

# Go to domain directory.
cd $DOCROOT

#copy config
cp $ZENCI_DEPLOY_DIR/settings/config/*.json $DOCROOT/files/config/active/
sed -i -e "s|DEPLOY_DIR|$ZENCI_DEPLOY_DIR|g" $DOCROOT/files/config/active/github_pages.settings.json


echo "Process contrib"
for project in `cat $ZENCI_DEPLOY_DIR/settings/contrib.list`; do
  downloaded=`$B pml |grep $project`
  if [ "$downloaded" == "" ];then
    if [ "$project" == "radix_layouts" ];then 
      # See issue: https://github.com/backdrop-contrib/radix_layouts/issues/9
      $B -y dl $project --type=layout
    else
      $B -y dl $project
    fi
  fi
done

echo "Enable Modules"

for module in `cat $ZENCI_DEPLOY_DIR/settings/modules.enable`; do
  installed=`$B pml |grep $module| grep '[v]'`
  
  if [ "$installed" == "" ];then
    echo "Enable $module"
    $B -y --root="$DOCROOT" en $module
  fi
done

$B -y --root="$DOCROOT" cc all

echo "Import Docs"

$B -y --root="$DOCROOT" github-pages-update
