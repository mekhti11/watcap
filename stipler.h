#ifndef __stipler_h__
#define __stipler_h__

struct Msg{
	char gonderen_id[8];
	char alan_id[8];
	char mesaj[256];
	char read_receipt;  	
};

struct USER{
	char u_id[8];
	char p_num[16];
	char name[32];
};

struct STR{
	int client_socket;
	//char id[8];
};

#endif