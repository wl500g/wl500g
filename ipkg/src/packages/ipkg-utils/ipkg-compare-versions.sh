#!/bin/sh
set -e

# This is a little experiment to see how expensive it would be to
# compare versions in a shell script. This script is not done yet
# and the nasiest part is still left undone, (the fact that in Debian
# versions all letters compare less than all non-letters).
#
# It looks to me like version comprehension might be the feature that pushes 
# ipkg from /bin/sh to compiled C code...

if [ $# -lt 3 ]; then
	echo "
usage: ipkg-compare-versions v1 op v2
	where op in (<<, <=, =, >=, >>)
Return value is 0 if v1 op v2 is satisfied, 1 otherwise"
	exit 2
fi

v1=$1
op=$2
v2=$3

# Debian has a little historical problem with operators...
may_be_equal=0
case $op in
'>>')
	op="-gt"
;;
'<<')
	op="-lt"
;;
'>'|'>=')
	op="-gt"
	may_be_equal=1
;;
'<'|'<=')
	op="-lt"
	may_be_equal=1
;;
'=')
	may_be_equal=1
;;
*)
	echo "ipkg_compare_versions: Invalid operator \`$op' valid operators are (<<, <=, =, >=, >>)"
	exit 1
;;
esac

if [ $may_be_equal == 1 -a $v1 == $v2 ]; then
	exit 0;
elif [ $op == '=' ]; then
	exit 1;
fi

epoch1=`echo $v1 | sed -ne 's/:.*//p'`
v1=`echo $v1 | sed -e 's/^[^:]*://'`
epoch2=`echo $v2 | sed -ne 's/:.*//p'`
v2=`echo $v2 | sed -e 's/^[^:]*://'`

upstream1=`echo $v1 | sed -e '/-/s/\(.*\)-.*/\1/'`
debian_rev1=`echo $v1 | sed -ne 's/.*-//p'`
upstream2=`echo $v2 | sed -e '/-/s/\(.*\)-.*/\1/'`
debian_rev2=`echo $v2 | sed -ne 's/.*-//p'`

echo "$epoch1:$upstream1-$debian_rev1 $op $epoch2:$upstream2-$debian_rev2"

exit 3

[ -z $epoch1 ] && epoch1="0"
[ -z $epoch2 ] && epoch2="0"

if [ $epoch1 != $epoch2 ]; then
	exit `[ $epoch1 $op $epoch2 ]`
fi

exit 3
