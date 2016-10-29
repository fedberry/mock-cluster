#!/bin/sh

if [ "$1" == "" ]; then 
  echo "Usage: $0 [repo] [branch]"
  exit 1
fi

repo="$1"

branch=""
if [ "$2" != "" ]; then 
  branch="$2"
fi

GITHUBDIR="$HOME/github/backdrop-contrib"
CONTRIB_DEPLOY_DIR="$GITHUBDIR/$repo/$branch"

if [ "$branch" == "" ]; then
  CONTRIB_DEPLOY_DIR="$GITHUBDIR/$repo/default"
fi

if [ -d "$CONTRIB_DEPLOY_DIR" ]; then
  echo "UPDATE $repo"
  cd $CONTRIB_DEPLOY_DIR
  git pull
  if [ "$branch" != "" ]; then
    git checkout $branch
  fi
else
  echo "DOWNLOAD $repo"
  mkdir -p $CONTRIB_DEPLOY_DIR
  cd $CONTRIB_DEPLOY_DIR
  git clone -q https://github.com/backdrop-contrib/$repo.git .

  if [ "$branch" != "" ]; then
    git checkout $branch
  fi
fi

INFOFILE=`find $CONTRIB_DEPLOY_DIR -name *.info|tail -n 1`
TYPE=`cat $INFOFILE|grep ^type|awk -F= '{print$2}'|xargs`

TARGETFOLDER="";

if [ "$TYPE" == "module" ]; then
  TARGETFOLDER="$DOCROOT/modules/contrib"
fi

if [ "$TYPE" == "layout" ]; then
  TARGETFOLDER="$DOCROOT/layouts/contrib"
fi

if [ "$TYPE" == "theme" ]; then
  TARGETFOLDER="$DOCROOT/themes/contrib"
fi

if [ "$TARGETFOLDER" == "" ]; then
  echo "Unknown type"
  exit 1;
fi 

if [ ! -L "$TARGETFOLDER/$repo" ] ; then
  ln -s $CONTRIB_DEPLOY_DIR $TARGETFOLDER/$repo
fi
