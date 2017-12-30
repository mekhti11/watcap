#include <stdio.h>
#include <stdlib.h>
#include "stipler.h"

int main(int argc, char const *argv[])
{
		FILE *f;
		struct Msg u;

		f= fopen("arramgelenmessages.txt","r");
		while(fread(&u,sizeof(struct Msg),1,f))
			printf("%s - %s - %s - %c\n",u.gonderen_id,u.alan_id,u.mesaj,u.read_receipt);
					
	
		fclose(f);
	return 0;
}