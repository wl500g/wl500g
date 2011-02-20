#include <stdio.h>
#include <nvcommons5.h>

char * strsep_with_validation(char **ptr,char *sep,char * retchar)	
{ 
	char * strptr ;	
	char *cptr;

	if (*ptr == NULL)
		return retchar;	

	strptr  =strsep(ptr,sep);	
	if (strptr != NULL) {

		/* change eny last \n to NULL */	
		cptr = strchr(strptr,'\r');;
		if (cptr != NULL)
			*cptr =' '; 


		if (*strptr == 0x00)		
			return retchar;	
	
	}else if (strptr == NULL)
		return retchar;
	
	return	strptr;		
}
				
