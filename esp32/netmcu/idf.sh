#!/bin/sh

if ! command -v idf.py >/dev/null 2>&1
then
   if test -f "$IDF_TOOLS_EXPORT_CMD" 
   then
    echo "variable exists: IDF_TOOLS_EXPORT_CMD=<$IDF_TOOLS_EXPORT_CMD>"
    . "$IDF_TOOLS_EXPORT_CMD" 
   elif test -n "$IDF_PATH" -a -f "$IDF_PATH/export.sh" 
   then
     echo "variable exists: IDF_PATH=<$IDF_PATH>"
     cd "$IDF_PATH" && . "./export.sh"
   else
     echo "using path <$HOME/esp/esp-idf>"
     cd "$HOME/esp/esp-idf" && . "./export.sh"
   fi
fi

if [ $? -eq 0 ]; then
  idf.py $@
else
     echo "Could not export ESP-IDF"
     exit 1
fi


