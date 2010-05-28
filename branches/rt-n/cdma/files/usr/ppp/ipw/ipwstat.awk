/CBC:/		{ split($2, a, ",")
                  gsub("\"", "", a[3])
                  print "Battery  : " a[2] "%" 
                  print "Power    : " a[3]
                }
/CSQ:/          { split($2, a, ",")
		  print "Signal   : " (-113+a[1]*2) "dBm" 
		  print "Noise    : " a[4] "dBm"
		  print "S/N      : " a[3]-a[4] "dBm" }
/CGATT:/	{ split($2, a, ",")
		  sub(".*: *", "")
		  print "Network  : " $0 
		  print "Frequency: " a[1]/1000 "MHz"
	 	}
/COPS:/         { sub(".*: *", "")
                  split($0, a, ",")
                  gsub("\"", "", a[3])
                  print "Operator : " a[3]
		}
/CGMI:/		{ sub(".*: *", "")
		  printf "Model    : %s, ", $0 }
/CGMM:/		{ sub(".*: *", "")
		  print $0 }
/CGMR:/		{ sub(".*: *", "")
		  print "Firmware : " $0 }
/CGSN:/		{ print "IMEI     : " $2 }
/CIMI:/		{ print "IMSI     : " $2 }
/CPIN:/		{ print "Security : " $2 " " $3}
