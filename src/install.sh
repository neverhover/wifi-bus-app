#!/bin/sh

set -x

PWD=`pwd`
APP_NAME="bus_web"
SRC_NAME="webserver"
WEB_DIR=$PWD/app_public/webserver
WEB_DOC=$WEB_DIR/web

ROUTE_FILE=$PWD/../test/route.txt
AUTH_FILE=$PWD/../test/auth.txt

sudo killall $APP_NAME

cp $PWD/exe/$ARCH_NAME/webserver/$SRC_NAME $WEB_DIR/$APP_NAME


if [ $1 = run ] ; then
	sudo $WEB_DIR/$APP_NAME --verbose --home $WEB_DIR $WEB_DOC &
fi
