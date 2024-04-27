#!/bin/bash
TARGET_DIR="$HOME/Senku2-64"
TARGET_FILE="senku2"
TARGET_TITLE="Senku II"
# Set language
case $1 in
  fr) country="LANGUAGE=fr_FR.UTF-8" ;;
  en) country="LANGUAGE=en_US.UTF-8" ;;
  es) country="LANGUAGE=es_ES.UTF-8" ;;
  *) echo "Usage: $0 fr|en, english by default" ; 
    country="LANGUAGE=en_US.UTF-8" ;;
esac
# Go to work directory
cd $TARGET_DIR
# Launch 
if ! [ -f $TARGET_DIR/$TARGET_FILE ] ; then
  echo "File $TARGET_FILE does not exist."
  exit 0
else
  echo -e "\n--== Launch $TARGET_TITLE ==--\n"
  echo "$country"
  eval $country $TARGET_FILE
  exit 1
fi