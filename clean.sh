#!/bin/sh
ipcs -s | grep root | awk ' { print $2 } ' | xargs ipcrm sem
