#!/bin/sh

set -x

PWD=`pwd`
APP_NAME="my_app"
WEB_DIR=$PWD/../../../
WEB_DOC=$(WEB_DIR)web

ROUTE_FILE=$PWD/../test/route.txt
AUTH_FILE=$PWD/../test/auth.txt

sudo killall $APP_NAME

cp $APP_NAME $WEB_DIR
cp $ROUTE_FILE $WEB_DIR
cp $AUTH_FILE $WEB_DIR

if [ $1 = run ] ; then
	sudo $WEB_DIR/$APP_NAME --verbose --home $WEB_DIR $WEB_DOC &
fi
