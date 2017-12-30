#include <stdio.h>
#include <stdlib.h>
#include "stipler.h"


int main(int argc, char const *argv[])
{
	FILE *f;
	struct USER u;

		f= fopen(argv[1],"rb");
		fread(&u,sizeof(struct USER),1,f);
		while(!feof(f)){
			printf("%s - %s - %s\n",u.u_id,u.p_num,u.name);
			fread(&u,sizeof(struct USER),1,f);
		}		
	
		fclose(f);

	return 0;
}