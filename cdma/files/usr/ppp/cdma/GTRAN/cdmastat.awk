BEGIN		{ print "ASUS time   : " strftime("%a %T %d.%m.%Y %Z") }
/GTIME:/	{ dt=mktime(substr($2,1,4)" "substr($2,5,2)" "substr($2,7,2)" "substr($2,9,2)" "substr($2,11,2)" "substr($2,13,2))
		  print "GTRAN time  : " strftime("%a %T %d.%m.%Y %Z", dt)
		  system("nvram set cdma_AsusTime='" strftime("%a %T %d.%m.%Y %Z")"'")
		  if (systime()<1000000000) system("date " strftime("%m%d%H%M%Y.%S",dt) ">/dev/null")
		  system("nvram set cdma_GtranTime='"strftime("%a %T %d.%m.%Y %Z", dt)"'") }
/GMR:/		{ sub(".*VER: ","")
		  print "Firmware    : " $0 }
/GSN:/		{ print "Serial      : " $2 }
/GTHYBM:/	{ I=$2==0?"CDMA only":$2==1?"HDR only":$2==2?"CDMA+HDR":"???"
		  print "Hybrid mode : " I }
/CBC:/		{ pwrStateIdx = substr($2, 1, 1); sub(".*,","",$2)
		  pwrState = pwrStateIdx == 0 ? $2"% (battery)" : pwrStateIdx == 1 ? $2"% (external power)" : pwrStateIdx == 2 ? "Not available" : pwrSrcIdx == 3 ? "Fault" : "???"
		  print "Power state : "pwrState }
/GTBYTE:/	{ I=O=$2; sub(",.*","",I); sub(".*,","",O)
		  I = sprintf("%0.3f", I/2048); lI = length(I)
		  O = sprintf("%0.3f", O/2048); lO = length(O)
		  format = lI >= lO ? "%"lI".3f MB\n" : "%"lO".3f MB\n"
		  printf("Data In     : "format, I)
		  printf("Data Out    : "format, O) }
/GTUSET:/	{ split(secs2dhms($2), dhms, ";")
		  print "This month  : " dhms[2]
		  print "Last month  : " dhms[3]
		  print "Time total  : " dhms[1] }
/GTSTAT:/	{ sub(",.*","",$2)
		  print "Signal      : -"$2"dB" 
		  system("nvram set cdma_RXLevel=-"$2"dB") }

/OK0D0A41542B4754444D0D0D0A/ { print "\n---=== Details ===---"
  tmpStr = "24 22 16 2 2 2 4 2 4 2 2 8 4 2 4 32 2 2 2 2 32 2 4 2 4 8 12 2"
  paramCnt = split(tmpStr, paramLen, " ")
  paramIdx = 43

  for(i = 1; i <= paramCnt; i++)
  {
    paramStr = paramStr substr($0, paramIdx, paramLen[i]) " "
    paramIdx += (paramLen[i] + 2)
  }

  split(paramStr, param, " ")

  hexTable = "0123456789ABCDEF"

  swv = hex2str(param[1])
  com_date = hex2str(param[2])
  com_time = hex2str(param[3])

  sess_state = param[17] == "00" ? "Inactive" : param[17] == "01" ? "AMP Setup" : param[17] == "02" ? "AT initiated" : param[17] == "03" ? "AN initiated" : param[17] == "04" ? "Open" : "???"
  at_state = param[18] == "00" ? "Inactive" : param[18] == "01" ? "Acquisition" : param[18] == "02" ? "Sync" : param[18] == "03" ? "Idle" : param[18] == "04" ? "Access" : param[18] == "05" ? "Connected" : "???"
  hdr_con = param[28] == "00" ? "Available" : param[28] == "01" ? "Unavailable" : "???"

  print "Software Version           : "swv
  print "Compile Date               : "com_date
  print "Compile Time               : "com_time
  print "Station Class Mark         : 0x"param[4]
  print "Mobile CAI Revision        : 0x"param[5]
  print "Mobile Model               : 0x"param[6]
  print "Firmware Revision          : 0x"param[7]
  print "Slot Cycle Index           : 0x"param[8]
  print "Demodulator ID             : 0x"param[9]
  print "Decoder ID                 : 0x"param[10]
  print "Interleaver ID             : 0x"param[11]
  print "ESN                        : "param[12]
  print "Center Frequency Channel   : 0x"param[13]
  print "BS Protocol Revision       : 0x"param[14]
  print "PN Offset                  : 0x"param[15]
  print "UATI State                 : "substr(param[16], 1, 8)" "substr(param[16], 9, 8)" "substr(param[16], 17, 8)" "substr(param[16], 25)
  print "Session State              : "sess_state
  print "AT State                   : "at_state
  print "Subnet Mask                : 0x"param[19]
  print "Color Code                 : 0x"param[20]
  print "Sector ID                  : "substr(param[21], 1, 8)" "substr(param[21], 9, 8)" "substr(param[21], 17, 8)" "substr(param[21], 25)
  print "Rx Automatic Gain Control  : 0x"param[22]
  print "Tx Automatic Gain Control  : 0x"param[23]
  print "Tx Gain Adjust             : 0x"param[24]
  print "Pilot Strength (Ec/Io)     : 0x"param[25]
  print "Current Sleep Signal       : 0x"param[26]
  print "PN Offset Array            : "substr(param[27], 1, 4)" "substr(param[27], 5, 4)" "substr(param[27], 9)
  print "HDR Connection             : "hdr_con
  
  system("nvram set cdma_CenterFreq=0x"param[13])
  system("nvram set cdma_PNOffset=0x"param[15])
  system("nvram set cdma_ColorCode=0x"param[20])
  system("nvram set cdma_SectorID='"substr(param[21], 1, 8)" "substr(param[21], 9, 8)" "substr(param[21], 17, 8)" "substr(param[21], 25)"'")
}

function hex2str(hex)
{
  result = ""
  m = length(hex)

  for(j = 1; j <= m; j+=2)
  {
    charCode = 16 * index(hexTable, substr(hex, j, 1))  + index(hexTable, substr(hex, j+1, 1)) - 17

    result = result sprintf("%c", charCode)
  }

  return result
}

function secs2dhms(secs)
{
  result = ""
  split(secs, s, ",")

  for(j in s)
  {
    d[j] = int(s[j]/86400); s[j] -= d[j]*86400
    h[j] = int(s[j]/3600);  s[j] -= h[j]*3600
    m[j] = int(s[j]/60);    s[j] -= m[j]*60

    result = d[j] == 1 ? result sprintf("%"length(d[1])"d day  ", d[j]) : result sprintf("%"length(d[1])"d days ", d[j])
    result = h[j] == 1 ? result " 1 hour  "    : result sprintf("%2d hours ", h[j])
    result = m[j] == 1 ? result " 1 minute  "  : result sprintf("%2d minutes ", m[j])
    result = s[j] == 1 ? result " 1 second;"   : result sprintf("%2d seconds;", s[j])
  }
  
  return result
}