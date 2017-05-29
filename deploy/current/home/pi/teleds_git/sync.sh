#!/bin/sh

cd /home/pi/teleds_git/git
git fetch origin
git reset --hard origin/master
git clean -f -d

