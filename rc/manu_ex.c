#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <time.h>

#include <wlioctl.h>
#include <wlutils.h>
#include "rc.h"

#define A_UCHAR  unsigned char
#define A_UINT8  unsigned char
#define A_UINT16 unsigned int
#define A_UINT32 unsigned long


/*----------------------------------------------------------------*/
/* Magic number, a quick test to see we're getting the desired struct */
#define P80211_IOCTL_MAGIC	(0x4a2d464dUL)
#define MSG_BUFF_LEN		4000
#define WLAN_DEVNAMELEN_MAX	16


#define CHANNEL_CW_INT              0x0001   // CW interference detected on the channel
#define CHANNEL_RADAR_INT           0x0002   // Radar interference detected on the channel
#define CHANNEL_BUSY                0x0004   // Busy, occupied directly or overlap from BSS on adjoining channel
#define CHANNEL_TURBO               0x0100   // Turbo Channel
#define CHANNEL_CCK                 0x0200   // CCK channel
#define CHANNEL_OFDM                0x0400   // OFDM channel
#define CHANNEL_2GHZ                0x0800   // 2 GHz spectrum channel.
#define CHANNEL_5GHZ                0x1000   // 5 GHz spectrum channel
#define CHANNEL_A       (CHANNEL_5GHZ|CHANNEL_OFDM)
#define CHANNEL_B       (CHANNEL_2GHZ|CHANNEL_CCK)
#define CHANNEL_PUREG   (CHANNEL_2GHZ|CHANNEL_OFDM)
#define CHANNEL_T       (CHANNEL_5GHZ|CHANNEL_OFDM|CHANNEL_TURBO)
#define CHANNEL_ALL     (CHANNEL_OFDM|CHANNEL_CCK|CHANNEL_5GHZ|CHANNEL_2GHZ|CHANNEL_TURBO)
#define CHANNEL_DEFAULT (CHANNEL_5GHZ|CHANNEL_OFDM) // default is normal 11a mode
#define MAX_REG_DOM_NAME_SIZE       10       // Max string size of reg dom name inc NULL

enum {
  FCC = 0,
  MKK,
  ETSI
};

/* Enumerated Regulatory Domain Information */
/* Enumerated Regulatory Domain Information */
enum EnumRd {
    /*
     * The following regulatory domain definitions are
     * found in the EEPROM. Each regulatory domain
     * can operate in either a 5GHz or 2.4GHz wireless mode or
     * both 5GHz and 2.4GHz wireless modes.
     * In general, the value holds no special
     * meaning and is used to decode into either specific
     * 2.4GHz or 5GHz wireless mode for that particular
     * regulatory domain.
     * 
     */
    NO_ENUMRD     = 0x00,
    FCC1_FCCA     = 0x10, // USA
    FCC1_WORLD    = 0x11, // Hong Kong

    FCC2_FCCA     = 0x20, // Canada
    FCC2_WORLD    = 0x21, // Australia
    FCC2_ETSIC    = 0x22,

    ETSI1_WORLD   = 0x37, 
    ETSI3_ETSIA   = 0x32, // France
    ETSI2_WORLD   = 0x35, // Germany
    ETSI3_WORLD   = 0x36, // Austria
    ETSI4_WORLD   = 0x30,
    ETSI4_ETSIC   = 0x38,
    ETSI1_ETSIB   = 0x39, // Special for ISRAEL	

    MKK1_MKKA     = 0x40, // Japan
    MKK1_MKKB     = 0x41, // Japan    
    APL4_WORLD    = 0x42, // Singapore
    APL3_FCCA     = 0x43,
    APL1_APLB     = 0x44, // China 
    APL2_WORLD    = 0x45, // Korea
    APL2_APLC     = 0x46,
    APL3_WORLD    = 0x47,

    APL1_WORLD    = 0x52, // Latin America
    APL1_FCCA     = 0x53, // Mexico
    APL1_APLA     = 0x54, // Chile
    APL1_ETSIC    = 0x55,

   
    /*
     * Super Domain Definitions
     */
    WOR0_WORLD    = 0x060,      /* World0 (WO0 SKU) */
    WOR1_WORLD    = 0x061,      /* World1 (WO1 SKU) */
    WOR2_WORLD    = 0x062,      /* World2 (WO2 SKU) */
    WOR3_WORLD    = 0x063,      /* World3 (WO3 SKU) */
    WOR4_WORLD    = 0x064,      /* World4 (WO4 SKU) */    
    APL5_WORLD    = 0x065,      /* Asia/Latin (AL1 SKU) */
    APL5_APLB     = 0x066,      /* China (CN1 SKU) */
 
    /*  "Regulatory domains ending in a number (e.g. APL1,
     *   MKK1, ETSI4,etc) apply to 5GHz channel and power information.
     *   Reg. domains ending in a letter (e.g. APLA, FCCA, etc.)
     *   apply to 2.4GHz channel and power information."
     */

    /*
     * The following wireless modes are either 2.4 or 5GHz
     */
    APL1          = 0x0150,     /* LAT & Asia */
    APL2          = 0x0250,     /* LAT & Asia */
    APL3          = 0x0350,     /* Taiwan */
    APL4          = 0x0450,     /* Singapore */

    ETSI1         = 0x0130,     /* Europe & others */
    ETSI2         = 0x0230,     /* Europe & others */
    ETSI3         = 0x0330,     /* Europe & others */
    ETSI4         = 0x0430,     /* Europe & others */    
    ETSIA         = 0x0A30,     /* France */
    ETSIB	  = 0x0B30,     /* Israel */
    ETSIC         = 0x0C30,     /* Latin America */    
    

    FCC1          = 0x0110,     /* US & others */
    FCC2          = 0x0120,     /* Canada, Australia & New Zealand */
    FCCA          = 0x0A10,     

    APLA          = 0x0A50,     /* Chile */
    APLB          = 0x0B50,     /* China */
    APLC          = 0x0C50,     /* South Korea */    
    
    MKK1          = 0x0140,     /* Japan */
    MKKA          = 0x0A40,     /* Japan */

    WORLD         = 0x0199,
    DEBUG_REG_DMN = 0x01ff
};

#define TURBO_CHANNEL_SEPARATION (8)
#define OFDM5_CHANNEL_SEPARATION (4)
#define CCK_CHANNEL_SEPARATION   (1)

#define MHZ2IEEE_CH(mhz)        ((mhz - 5000) / 5)
#define FCC_OUTDOOR_FIRST_FREQ 5725

/* 5 GHz table settings */

typedef struct regDmnEnumChannelGroup {
    A_UINT16    lowChannel;   // Low Channel center in MHz
    A_UINT16    highChannel;  // High Channel center in MHz  
} REG_DMN_ENUM_CHANNEL_GROUP;

/* TODO: Add EIRP emission addition */
/* TODO: Define test groups for 2.4 */

#define MAX_CHANNEL_GROUPS 2

typedef struct regDmnEnumFreqTable {
    A_UINT16         regDmnEnum;
    A_UINT16         entries;
    A_UINT8          testGroup;
    A_UCHAR          name[8];
    REG_DMN_ENUM_CHANNEL_GROUP chanGroup[MAX_CHANNEL_GROUPS];
} REG_DMN_ENUM_FREQ_TABLE;

typedef struct CountryCodeToEnumRd {
    A_UINT16        regDmnEnum;
    const A_UCHAR   isoName[3];
    const A_UCHAR   name[16];
} COUNTRY_CODE_TO_ENUM_RD;
  


/*
 * Regulatory Domain Tables
 *
 * Tables have entries ordered with AP/Adhoc planting preference in mind.
 *  -Indoor channels are ordered before outdoor
 *  -Higher power level indoor channels are first
 */

static const REG_DMN_ENUM_FREQ_TABLE regDmnEnum5[] = {
    {DEBUG_REG_DMN, 2, FCC, "NONE", {
    {36 , 64}, 
    {149,165}}},

    {FCC1, 2, FCC, "FCC1", {   
    {36 , 64}, 
    {149,165}}},
    
    {FCC2, 2, FCC, "FCC2", {   
    {36 , 64}, 
    {149,165}}},

    {MKK1, 1, MKK, "MKK1", {
    { 34, 46}, 
    {  0,  0}}},

    {ETSI1, 2, ETSI, "ETSI1", {
    { 36, 64}, 
    {100,140}}},

    {ETSI2, 1, ETSI, "ETSI2", {
    { 36, 48}, 
    {  0,  0}}},
   

    {ETSI3, 1, ETSI, "ETSI3", {
    { 36, 64}, 
    {  0,  0}}},

    {ETSI4, 1, ETSI, "ETSI4", {
    { 36, 64}, 
    {  0,  0}}},
       
    
    {APL1, 1, FCC, "APL1", {
    {149,165}, 
    {  0,  0}}},
    
    {APL2, 1, FCC, "APL2", {
    {149,161}, 
    {  0,  0}}},
    
    {APL3, 2, FCC, "APL3", {
    { 52, 64}, 
    {149,165}}},
    
    {APL4, 2, FCC, "APL4", {
    { 36, 48}, 
    {149,165}}},
};

static const REG_DMN_ENUM_FREQ_TABLE regDmnEnum5Turbo[] = {
    {DEBUG_REG_DMN, 2, 0, "NONE", {
    {42, 58}, 
    {152,160}}},

    {FCC1, 2, FCC, "FCC1", {
    {42, 58},
    {152,160}}}      
};

#ifdef REMOVE
/* "Worldwide", 1-13
   "Thailand", "Israel", "Jordan", "China", "Japan", "USA", "Europe", "USA Low", "Japan High", "All" */
   
static const char *br_name[] = 
{
	"All",
	"Europe",
	"USA",
	"Japan",
	"Europe",
	"Europe",
	"USA",
	"Jordan",
	"Israel",
	"Europe"	
};
#endif /* REMOVE */

static const REG_DMN_ENUM_FREQ_TABLE regDmnEnum2[] = {
    {DEBUG_REG_DMN, 1, 0, "NONE", {
    {   1,   14},
    {   0,    0}}},

    {WORLD, 1, FCC, "WORLD", {
    {   1,   13},
    {   0,    0}}},

    {FCCA, 1, FCC, "FCCA", {
    {   1,   11},
    {   0,    0}}},

    {MKKA, 1, MKK, "MKKA", {
    {   1,   14},
    {   0,    0}}},
    
    {APLA, 1, FCC, "APLA", {
    {   1,   13},
    {   0,    0}}},
    
    {APLB, 1, FCC, "APLB", {
    {   1,   13},
    {   0,    0}}},
    
    {APLC, 1, FCC, "APLC", {
    {   1,   12},
    {   0,    0}}},
    
    {ETSIA, 1, ETSI, "ETSIA", {
    {  10,   13},
    {   0,    0}}},      
    
    {ETSIB, 1, ETSI, "ETSIB", {
    {   5,    7},
    {   0,    0}}}, 
    
    {ETSIC, 1, ETSI, "ETSIC", {
    {   1,   13},
    {   0,    0}}},
    
    
};

#define RDMAPPING_SIZE 32

static const A_UINT16 RDMapping[RDMAPPING_SIZE][3] =
{
   {NO_ENUMRD, DEBUG_REG_DMN, DEBUG_REG_DMN},           
   {FCC2_FCCA,     FCC2,    FCCA    },                     
   {FCC2_WORLD,    FCC2,    WORLD   },                     
   {FCC2_ETSIC,    FCC2,    ETSIC   },                     
                                                           
   {ETSI1_WORLD,   ETSI1,   WORLD   },                     
   {ETSI2_WORLD,   ETSI2,   WORLD   },                     
   {ETSI3_WORLD,   ETSI3,   WORLD   },                     
   {ETSI4_WORLD,   ETSI4,   WORLD   },                     
                                                           
   {ETSI3_ETSIA,   ETSI3,   ETSIA   },     
   {ETSI4_ETSIC,   ETSI4,   ETSIC   },                
                                                           
   {FCC1_WORLD,    FCC1,    WORLD   },                     
   {FCC1_FCCA,     FCC1,    FCCA    },                     
                                                           
   {APL1_WORLD,    APL1,    WORLD   },                     
   {APL2_WORLD,    APL2,    WORLD   },                     
   {APL3_WORLD,    APL3,    WORLD   },                     
   {APL4_WORLD,    APL4,    WORLD   },                     
                                                           
   {APL1_APLA,     APL1,    APLA    },                     
   {APL1_APLB,     APL1,    APLB    },                     
                                                           
   {APL1_FCCA,     APL1,    FCCA    },                     
   {APL3_FCCA,     APL3,    FCCA    },                     
                                                           
   {APL1_ETSIC,    APL1,    ETSIC   },                     
   {APL2_APLC,     APL2,    APLC    },                     
                                                           
   {MKK1_MKKA,     MKK1,    MKKA    },              
   {MKK1_MKKB,     MKK1,    MKKA    },              
   
   {ETSI1_ETSIB,   ETSI1,   ETSIB   },
                                                    
   /*                                               
    * The area below is reserved for super domains  
    */                                              
   {WOR0_WORLD,    WOR0_WORLD,   WOR0_WORLD    },   
   {WOR1_WORLD,    WOR1_WORLD,   WOR1_WORLD    },   
   {WOR2_WORLD,    WOR2_WORLD,   WOR2_WORLD    },   
   {WOR3_WORLD,    WOR3_WORLD,   WOR3_WORLD    },   
   {WOR4_WORLD,    WOR4_WORLD,   WOR4_WORLD    },   
   {APL5_WORLD,    APL5_WORLD,   APL5_WORLD    },   
   {APL5_APLB,     APL5_APLB,    APL5_APLB     }
};

static const COUNTRY_CODE_TO_ENUM_RD allCountries[] = {
    	{0x00, 		"DB", "NONE"         },    	 
    	{APL1_ETSIC,    "AR", "ARGENTINA"    },     
    	{FCC2_WORLD,    "AU", "AUSTRALIA"   },     
    	{ETSI4_WORLD,   "AT", "AUSTRIA"     },     
    	{ETSI4_WORLD,   "BE", "BELGUIM"     },     
    	{APL1_ETSIC,    "BR", "BRAZIL"      },     
    	{FCC2_FCCA,     "CA", "CANADA"      },     
    	{APL1_APLA,     "CL", "CHILE"       },     
    	{ETSI4_WORLD,   "CZ", "CZECH_REPUBLIC"},  
    	{APL1_WORLD,    "CN", "CHINA"       },     
    	{APL1_ETSIC,    "CO", "COLOMBIA"    },     
    	{ETSI4_WORLD,   "DK", "DENMARK"     },     
    	{APL2_WORLD,    "EC", "ECUADOR"     },     
    	{ETSI4_WORLD,   "FI", "FINLAND"     },     
    	{ETSI3_ETSIA,   "FR", "FRANCE"      },     
    	{ETSI4_WORLD,   "DE", "GERMANY"     },     
    	{ETSI4_WORLD,   "GR", "GREECE"      },     
    	{FCC1_WORLD,    "HK", "HONG_KONG"   },     
    	{ETSI4_ETSIC,   "HU", "HUNGARY"     },     
    	{ETSI4_WORLD,   "IS", "ICELAND"     },     
    	{ETSI4_WORLD,   "IE", "IRELAND"     },     
    	{ETSI2_WORLD,   "IT", "ITALY"       },     
    	{MKK1_MKKA,     "JP", "JAPAN"       },     
    	{APL2_APLC,     "KR", "KOREA_REPUBLIC"},  
    	{ETSI4_WORLD, 	"LI", "LIECHTENSTEIN"},   
    	{ETSI4_WORLD,   "LT", "LITHUANIA"   },     
    	{ETSI4_WORLD,   "LU", "LUXEMBOURG"  },     
    	{FCC1_WORLD,    "MO", "MACAU"       },     
    	{APL1_WORLD,    "MX", "MEXICO"      },     
    	{ETSI4_WORLD,   "MC", "MONACO"      },     
    	{ETSI1_WORLD,   "NL", "NETHERLANDS" },     
    	{FCC2_ETSIC,    "NZ", "NEW_ZEALAND" },     
    	{ETSI3_WORLD,   "NO", "NORWAY"      },     
    	{APL1_WORLD,    "PA", "PANAMA"      },     
    	{APL1_WORLD,    "PE", "PERU"        },     
    	{FCC1_WORLD,    "PH", "PHILIPPINES" },     
    	{ETSI2_WORLD,   "PL", "POLAND"      },     
        {ETSI4_WORLD,   "PT", "PORTUGAL"    },     
	{FCC1_FCCA,     "PR", "PUERTO_RICO" },     
	{APL4_WORLD,    "SG", "SINGAPORE"   },     
	{ETSI4_WORLD,   "ZA", "SOUTH_AFRICA"},     
	{ETSI4_WORLD,   "ES", "SPAIN"       },     
	{ETSI4_WORLD,   "SE", "SWEDEN"      },     
	{ETSI2_WORLD,   "CH", "SWITZERLAND" },     
	{APL3_WORLD,    "TW", "TAIWAN"      },     
	{APL2_WORLD,    "TH", "THAILAND"    },     
	{ETSI2_WORLD,   "TR", "TURKEY"      },     
	{ETSI4_WORLD, 	"GB", "UNITED_KINGDOM"},  
	{FCC1_FCCA,   	"US", "UNITED_STATES" },    
	{APL1_ETSIC,    "VE", "VENEZUELA"   },
	{ETSI1_ETSIB,   "IR", "ISRAEL"   }
};

static const REG_DMN_ENUM_FREQ_TABLE *           
wlanGetCcTable(A_UINT16 channelFlags)            
{                                                
    if (channelFlags & CHANNEL_2GHZ)             
    {                                            
        return (&regDmnEnum2[0]);                
    }                                            
    else if (channelFlags & CHANNEL_5GHZ) {      
        if (channelFlags & CHANNEL_TURBO)        
        {                                        
            return(&regDmnEnum5Turbo[0]);        
        } else {
            return(&regDmnEnum5[0]);
        }
    }
    else {
        // either 2 or 5 GHz flags must be set
        //ASSERT(0);
        return NULL;
    }
}

static int
wlanGetCcEntries(A_UINT16 channelFlags)
{
    if (channelFlags & CHANNEL_2GHZ) {
        return ARRAY_SIZE(regDmnEnum2);
    }
    else if (channelFlags & CHANNEL_5GHZ) {
        if (channelFlags & CHANNEL_TURBO) {
            return ARRAY_SIZE(regDmnEnum5Turbo);
        } else {
            return ARRAY_SIZE(regDmnEnum5);
        }
    }
    else {
        // either 2 or 5 GHz flags must be set
        //ASSERT(0);
        return 0;
    }
}

static void ParseReg(char *regDmnName, A_UINT16 *regDmnCode, char *country)
{
    char reg[5];
        
    if (strlen(regDmnName) >= 4) // With Country Code
    {
    	strncpy(reg, regDmnName, 4);
    	reg[4] = 0;    	
    	sscanf(reg, "%x", regDmnCode);
    	if (country != NULL)
    	   strcpy(country, regDmnName+4);    	 
    }
    else
    {
    	*regDmnCode = 0x00;
    	regDmnName = NULL;
    }
}


#ifdef REMOVE
/***********************************************************
 * RefreshBRCountry
 *
 * Print out the channel list based only on the given reg
 * domain and turbo mode
 */
static void
RefreshBRCountry(char *regDmnName, char *country, char *country_code)
{
    A_UINT16 regDmnEnum, channelFlags, regDmnCode;
    int i, regDmnEnumEntries;
    const REG_DMN_ENUM_FREQ_TABLE *pCcTable;    
    A_UINT16 lowChannel = 24, highChannel = 220;    
   

    /* Add by ChenI to translate regulatory domain code */
    ParseReg(regDmnName, &regDmnCode, country_code);   
    
    dprintf("Domain Code: %x\n", regDmnCode);             
    regDmnEnum = regDmnCode;
    
    for (i = 0; i < RDMAPPING_SIZE; i++)
    {
    	if (RDMapping[i][0] == regDmnCode)
    	   break;
    }
    
    if (i == RDMAPPING_SIZE) i = 0;
    	    
    regDmnEnum = RDMapping[i][2];
    lowChannel = 1;
    highChannel = 14;
    channelFlags = CHANNEL_B;	
        
    pCcTable = wlanGetCcTable(channelFlags);
    regDmnEnumEntries = wlanGetCcEntries(channelFlags);
    
    /* Search through Regulatory Domain (Enum) table for a match */
    for (i = 0; i < regDmnEnumEntries; i++) 
    {
        if (pCcTable[i].regDmnEnum == regDmnEnum) 
        {
            dprintf("%s\n",br_name[i]);
	    strcpy(country, br_name[i]);
            return;                        
        }
    }
    strcpy(country, "WorldWide");
    return;    
}
#endif /* REMOVE */

/***********************************************************
 * RefreshChannelList
 *
 * Print out the channel list based only on the given reg
 * domain and turbo mode
 */
static void
RefreshChannelList(char *regDmnName, A_UINT16 current, A_UINT16 chanList[])
{
    A_UINT16 regDmnEnum, channelFlags, regDmnCode;
    int i, j, regDmnEnumEntries;
    const REG_DMN_ENUM_FREQ_TABLE *pCcTable;
    A_UINT16 channelSpread, searchChannel, firstChannel;
    A_UINT16 lowChannel = 24, highChannel = 220;    
    A_UINT16 maxRdChannel = 220;
    A_UINT16 chanIdx = 0;


    /* Add by ChenI to translate regulatory domain code */
    ParseReg(regDmnName, &regDmnCode, NULL);   
    
    dprintf("Domain Code: %x\n", regDmnCode);
             
    regDmnEnum = regDmnCode;
    
    for (i = 0; i < RDMAPPING_SIZE; i++)
    {
    	if (RDMapping[i][0] == regDmnCode)
    	   break;
    }
    
    if (i == RDMAPPING_SIZE) i = 0;
    	
    regDmnEnum = RDMapping[i][2];
    lowChannel = 1;
    highChannel = 14;
    channelFlags = CHANNEL_B;
	
    pCcTable = wlanGetCcTable(channelFlags);
    regDmnEnumEntries = wlanGetCcEntries(channelFlags);
    channelSpread = CCK_CHANNEL_SEPARATION;
    

    firstChannel = 0;
    /* Search through Regulatory Domain (Enum) table for a match */
    for (i = 0; i < regDmnEnumEntries; i++) {
        if (pCcTable[i].regDmnEnum == regDmnEnum) {
            for (j = 0; j < pCcTable[i].entries; j++) 
            {                   	     	
                /* Find this channel's entry */
                for (searchChannel = pCcTable[i].chanGroup[j].lowChannel;
                    searchChannel <= pCcTable[i].chanGroup[j].highChannel;
                    searchChannel += channelSpread)
                {   
                    if ((searchChannel >= lowChannel) && (searchChannel <= highChannel) &&   (searchChannel <= maxRdChannel))
                    {                    	
			chanList[chanIdx++] = searchChannel;
			chanList[chanIdx] = -1;
						
			if (firstChannel == 0)
			{
			     firstChannel = searchChannel;
			}   
			
			if (current != 999)
			{
			    if (current == searchChannel)
			    {
			       dprintf("%d", current);	
			       return; 	
			    } 
			}    
			else
			{
                            dprintf("%d\n", searchChannel);                    
                        }    
                    }
                }
            }
            
            if (current != 999)
            {
            	 /* Find the middle channel for default value */           	 
            	 if (chanIdx != 0)
            	       firstChannel = chanList[chanIdx/2];

                 dprintf("%d\n", firstChannel);                 
            }    
            return;
        }
    }
}


void convert_country(void)
{
    A_UINT16 chanList[16];
    char chanListStr[64];
    char reg[32];
    int i;

    strcpy(reg, nvram_safe_get("regulation_domain"));

    if (strlen(reg)>4) // With Country Code
    {
	nvram_set("wl_country_code", reg + 4);
    }
    else nvram_set("wl_country_code", "ALL");
 	
    RefreshChannelList(nvram_safe_get("regulation_domain"), 999, chanList);

    i = 0;

    *chanListStr = '\0';
    while (chanList[i] != -1)
    {
         if (i == 0) sprintf(chanListStr, "%d", chanList[i]);
	 else sprintf(chanListStr, "%s %d", chanListStr, chanList[i]);
	 i++;
	 //printf("chan :%s\n", chanListStr);
    }
    nvram_set("wl_chan_list", chanListStr);
}

#define MAXBUF 32

#ifdef REMOVE
void sync_mac(char *devname, char *mac)
{
	int cmd, result, i, j;
	char buf[MAXBUF], macstr[32], s[3];
	unsigned char t;	
	srom_rw_t *srom;

	if (strlen(mac) != 17) return;

	//printf("dev: %s, mac : %s\n", devname, mac);

	s[2] = 0;

	for (i = 0; i < 6; i++)
	{
		strncpy(s, mac+i*3, 2);
		t = (unsigned char)strtoul(s, NULL, 16);
		if (i%2 == 0) macstr[i+1] = t;
		else macstr[i-1] = t ;
	}

	srom = buf;
	srom->byteoff = 72;
	srom->nbytes = 6;
	
	if ((result = wl_ioctl(devname, WLC_GET_SROM, buf, MAXBUF)) == 0)
	{
		for (i = 0; i < 6; i++)
		{
			//printf(" %x %x \n", buf[8+i], macstr[i]);
		}

		if (memcmp(macstr, buf+8, 6) != 0)
		{			
			memcpy(buf+8, macstr, 6);
			result = wl_ioctl(devname, WLC_SET_SROM, buf, MAXBUF);				
#ifdef WL500GX_REMOVE
			/* setup afterburner flag */
			wsrom_main(devname, 114, 0x20f);
#else
			wlan_update();
#endif
			return;	
		}
	}
	return;
}
#endif /* REMOVE */

int rsrom_main(const char *devname, unsigned int pos, int pflag)
{
	int result;
	unsigned short *oval;
	char buf[MAXBUF];
	srom_rw_t *srom;
	unsigned short val;

	srom = (srom_rw_t *)buf;
	srom->byteoff = pos;
	srom->nbytes = 2; //sizeof(val);
	
	if ((result = wl_ioctl(devname, WLC_GET_SROM, buf, MAXBUF)) == 0)
	{
		oval = (unsigned short *)(buf + 8);
		val = (unsigned short)*oval;
	}
	else val = 0;

	if (pflag) printf("srom[%x] : %x\n", pos, val);
	return val;
}

int wsrom_main(const char *devname, unsigned int pos, unsigned short val)
{
	int result;
	unsigned short *oval;
	char buf[MAXBUF];
	srom_rw_t *srom;
	/* Usage srom [postion] [val in 2 byte] */
	dprintf("write %s srom[%x] : %x\n", devname, pos, val);	
	srom = (srom_rw_t *)buf;
	srom->byteoff = pos;
	srom->nbytes = 2; //sizeof(val);
	
	if ((result = wl_ioctl(devname, WLC_GET_SROM, buf, MAXBUF)) == 0)
	{
		oval = (unsigned short *)(buf + 8);

		if (*oval != val)
		{
			memcpy(buf+8, &val, sizeof(val));
			result = wl_ioctl(devname, WLC_SET_SROM, buf, MAXBUF);			}
	}
	return 0;
}

int write_mac(const char *devname, const char *mac)
{
	int result, i;
	char buf[MAXBUF], macstr[32], s[3];
	unsigned char t;	
	srom_rw_t *srom;

	if (strlen(mac) != 17) return -1;

	//printf("dev: %s, mac : %s\n", devname, mac);

	s[2] = 0;

	for (i = 0; i < 6; i++)
	{
		strncpy(s, mac+i*3, 2);
		t = (unsigned char)strtoul(s, NULL, 16);
		if (i%2 == 0) macstr[i+1] = t;
		else macstr[i-1] = t ;
	}

	srom = (srom_rw_t *)buf;
	srom->byteoff = 72;
	srom->nbytes = 6;

	memcpy(buf+8, macstr, 6);
	result = wl_ioctl(devname, WLC_SET_SROM, buf, MAXBUF);
	return result;
}


int
wlan_update()
{
	unsigned short val;

	/* update eerpom for driver 3.90.x.x */
	if (nvram_match("productid", "WL500g"))
	{
		val = rsrom_main("eth2", 104, 0);
		if (val == 0x003c) wsrom_main("eth2", 104, 0x004c);// ver 1.6
		else if (val == 0x0035) wsrom_main("eth2", 104, 0x0046);     // ver 2.2
#if 0
		else wsrom_main("eth2", 104, 0x0046);
#endif
		wsrom_main("eth2", 114, 0x0249);
	}
	else if (nvram_match("productid", "WL500g.Deluxe"))
	{	
		// ver 2.2
		wsrom_main("eth1", 104, 0x0046);
		wsrom_main("eth1", 114, 0x0249); 
	}
	return 0;
}

