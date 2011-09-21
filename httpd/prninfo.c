#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <bcmnvram.h>
#include <bcmconfig.h>
#include "lp.h"

#ifdef DEBUG
# define PRINT(fmt, args...) fprintf(stderr, fmt, ## args)
#else
# define PRINT(fmt, args...)
#endif

void readPrnID();


static void deCR(char *str)
{
	int i, len;

	len = strlen(str);
	for (i=0; i<len; i++)
	{
		if (*(str+i) == '\r' || *(str+i) == '\n' || *(str+i) == '"')
		{
			*(str+i) = 0;
			break;
		}
	}
}

static void readUsbPrnID(char *prninfo)
{
    char                                mfr[32];
    char                                model[64];
    int                                 fd;
    struct parport_splink_device_info   prn_info;

    fd = open(LP_DEV(0), O_RDWR);
    if (fd < 0 && errno != ENOENT) // Someone is opening the usb lp0
    {
        FILE *fp = fopen("/proc/usblp/lp0", "r");

        if (fp != NULL)
        {
            char buf[1024];
            char *token;

            memset(mfr , 0 , sizeof(mfr));
            memset(model , 0 , sizeof(model));

            while ( fgets(buf, sizeof(buf), fp) != NULL )
            {
                if (buf[0] == '\n')
                {
		    PRINT("skip empty line\n");
                    continue;
                }

                if (strncmp(buf , "Manufacturer=" , strlen("Manufacturer=")) == 0)
                {
                    token= buf + strlen("Manufacturer=");
                    PRINT("Manufacturer token %s",token);

                    memccpy(mfr , token , '\n' , 31);
					deCR(mfr);
                }

                if (strncmp(buf , "Model=" , strlen("Model="))   == 0)
                {
                    token= buf + strlen("Model=");
                    PRINT("Model token %s",token);

                    memccpy(model   , token , '\n' , 63);
					deCR(model);
                }
            }

            fclose(fp);

            sprintf( prninfo , "%s %s", mfr , model );
            PRINT("PRINTER MODEL %s\n",prninfo);
        }
    }
    else if (fd > 0)
    {
        if (ioctl(fd, LPGETID, &prn_info) < 0)
        {
            perror("ioctl");
        }
        else
        {
            PRINT("manufacturer: %s\n"
        	  "model: %s\n"
        	  "class: %s\n"
		  "description: %s\n",
		  prn_info.mfr ,prn_info.model, prn_info.class_name,
		  prn_info.description);

            memccpy(mfr , prn_info.mfr , 1 , 32);
            memccpy(model , prn_info.model , 1 , 32);

            sprintf( prninfo , "%s %s", mfr , model );
        }

        close(fd);
    }
}


#define ONLINE_STRING "On-Line"
#define OFFLINE_STRING "Off-Line"

static void readLpdStatus(void)
{
    FILE    *fp;
    char    buf[64];
    static char *ukeyword = "PRINTER_USER=\"";
    static char *skeyword = "PRINTER_STATUS=\"";
    char    *token;
    char    user[32];
    char    status[32];

    fp = fopen("/var/state/printstatus.txt", "r");

    memset(user, 0, sizeof(user));
    memset(status, 0, sizeof(status));

    if (fp != NULL)
    { 
            while ( fgets(buf, sizeof(buf), fp) != NULL )  
            {
                if(buf[0] == '\n')
                {
                    continue;
                }

                if(strncmp(buf , ukeyword , strlen(ukeyword)) == 0)
                {
                    token= buf + strlen(ukeyword);
		    PRINT("User token %s",token);

		    deCR(token);
		    strcpy(user, token);
                }
                else if(strncmp(buf , skeyword, strlen(skeyword)) == 0)
                {
                    token= buf + strlen(skeyword);
		    PRINT("Status token %s",token);

		    deCR(token);
		    strcpy(status, token);
                }
            }
	    fclose(fp);
    }
    else strcpy(status, ONLINE_STRING);

    nvram_set("printer_status_t", status);
    nvram_set("printer_user_t", user);
}

/* update current status of printer */
void readPrnID()
{
    char    prninfo[256];

    memset( prninfo , 0 , sizeof(prninfo) );

    readUsbPrnID(prninfo);

    /* update status of printer */
    if (strlen(prninfo) == 0)
    {
		nvram_set("printer_model_t", "");
		nvram_set("printer_status_t", "");
		nvram_set("printer_user_t", "");
    }
    else
    {
		nvram_set("printer_model_t", prninfo);
		readLpdStatus();
    }
}
