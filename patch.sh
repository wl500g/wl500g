#!/bin/bash

USAGE="Usage: $0 [-cdRZ] <dir> patches to apply"

#	Options:
#		c	Enable detection of already applied patches
#		d	Turn on debug
#		R	Revert
#		Z	Set timestamp

if [ $# -lt 2 ]; then
	echo $USAGE
	exit 1
fi

smart_cont=no
dbg_output=/dev/null
flg_test=-R; unset flg_apply; unset flg_opt

while getopts cdrRTZ f
do
	case $f in
	    c)    smart_cont=yes;;
	    d)	  dbg_output=patch_debug.log;;
	    r|R)  unset flg_test; flg_apply=-R;;
	    T|Z)  flg_opt=-Z;;
	    \?) echo $USAGE; exit 1;;
	esac
done
shift `expr $OPTIND - 1`

DIR=$1

shift

LC_COLLATE=C
echo -n "" > $dbg_output

if [ "$smart_cont" = "yes" ]; then
# reverse patch order
    for a in $*; do
	RPATCHES="$a $RPATCHES"
    done

    echo "Looking for last applied patch"

    for a in $RPATCHES; do
	if [ ! -f $a ]; then
		echo $a missing!
		exit 1
	fi
	# check for patches which was already applied
	# NB! it cannot detect situation in that sequence of patches
	#     modify same source file
	if patch -d $DIR --dry-run -p1 -f $flg_test < $a > /dev/null ; then
		echo $a already applied
		break
	else
		FPATCHES="$a $FPATCHES"
	fi
    done
else
    FPATCHES="$*"
fi

[ -z "$FPATCHES" ] && exit 0

if [ -z "$flg_apply" ]; then
    echo "Patching $DIR with $FPATCHES"
else
    echo "Reverting $DIR with $FPATCHES"
fi
echo "" >> $dbg_output

for a in $FPATCHES; do 
	echo $a
	echo "\$ apply $a:" >> $dbg_output
	echo "\$ +++++++++++++++++++++++++++++++" >> $dbg_output
	# try to apply patches
	if patch -d $DIR --dry-run -p1 -f $flg_apply < $a >> $dbg_output ; then
		patch -d $DIR -p1 -f -E $flg_apply $flg_opt < $a || exit 1
	else
		echo "Can not apply $a cleanly"
		exit 1
	fi
done

exit 0
