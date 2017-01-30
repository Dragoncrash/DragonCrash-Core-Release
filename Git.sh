#!/usr/bin/bash

echo "Use this script to push relevant directories to GitHub."
echo "Make sure this script is run from the dev folder"

#Add paths to this variable
PATHS=" Git.sh DragonCrash/* Code/DragonCrash/* Gems/Environment_Tile_System_v2/* Gems/DragonCrashCollectibles/*"


if [ "$#" -eq 1 ]; then
	git add $PATHS
	git commit -m "$1"
	git push --all
else
	echo "Usage: bash Git.sh <String to be used as the Git commit message in quotes>"
fi
