BEGIN { is_syscalltable = 0 }
{
	if ( $1 == ".macro" && $2 == "syscalltable" ) {
		is_syscalltable = 1;
	} else if ( is_syscalltable == 1 ) {
		if ( $1 == ".endm" ) {
			is_syscalltable = 0;
		} else {
			if ( $1 == "sys" ) {
				print "\"" $2 "\",";
			} else if ( $1 == "mille" ) {
				print "\"" $2 "\", /* mille loop? */";
			} else {
				print $0;
			}
		}
	}
}
