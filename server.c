#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "stipler.h"

char * merge_strings(char *s1,char *s2){
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  char *result;
  result =(char *) malloc((len1+len2+1)*sizeof(char));//+1 null karakteri için
  memcpy(result, s1, len1);
  memcpy(result+len1, s2, len2+1);//+1 null karakteri  için
  return result;
}

void receive_message(int client_socket){
  struct Msg m;
  FILE *f;
  char * fname;
  char tmp[16];

  if(recv(client_socket,&tmp,sizeof(tmp),0)==-1){
    perror("Receive Error");
    exit(-1);
  }

  strncpy(m.gonderen_id,tmp,sizeof(tmp));

  printf("gi:%s\n",tmp);
  printf("gi:%s\n",m.gonderen_id);
  recv(client_socket,&m.alan_id,sizeof(m.alan_id),0);
  printf("AI:%s\n",m.alan_id);
  recv(client_socket,&m.read_receipt,sizeof(m.read_receipt),0);
  printf("RR:%c\n",m.read_receipt);
  recv(client_socket,&m.mesaj,sizeof(m.mesaj),0);
  printf("Msg:%s\n",m.mesaj);
  fname = merge_strings(tmp,"-");
  sprintf(m.gonderen_id,"%s",tmp);
  fname = merge_strings(fname,m.alan_id);
  fname = merge_strings(fname,".txt");
  printf("FN:%s\n",fname);
  
  f=fopen(fname,"a+");
  fwrite(&m,sizeof(struct Msg),1,f);
  
  fclose(f);
  printf("Saved\n");
}


void search_ID(char* id){
	FILE *f;
	struct USER u;
	int a=0;
	f=fopen("users.txt","a+");
	fread(&u,sizeof(struct USER),1,f);
	while(!feof(f) && a==0){
		if(!strncmp(id,u.u_id,sizeof(id)))
			a=1;
		fread(&u,sizeof(struct USER),1,f);
	}
  if(a==0){
  	printf("Kullanici bilgilerini giriniz:");
  	scanf("%s %s",u.name,u.p_num);
  	sprintf(u.u_id,"%s",id);
  	fwrite(&u,sizeof(struct USER),1,f);
    fclose(f);
  	a=0;
  	char * fname;
  
  	fname = merge_strings(id,".txt");
  
  	f=fopen(fname,"a+");
  }
  fclose(f);
}
  
int main(int argc, char const *argv[]){
  
  struct sockaddr_in server_address,client;
  int server_socket;
  int client_socket,c;
  struct Msg message;
  struct USER user;
  ssize_t t;
  char rm_status;

  c = sizeof(struct sockaddr_in);

  server_socket=socket(AF_INET,SOCK_STREAM,0);
	perror("Socket Error ");
  
  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(9060);
  server_address.sin_addr.s_addr=INADDR_ANY;

  //printf("%d\n",server_socket );

  t = bind(server_socket,(struct sockaddr *) &server_address,sizeof(struct sockaddr_in));
 	perror("Bind Error ");
  
  //printf("%d\n",errno );


	t = listen(server_socket,5);
	  
	if(t == -1){
	 	perror("Listen Error ");
	 	exit(-1);
	}

	int a=1 ;
	while(a){  

		client_socket=accept(server_socket,(struct sockaddr *) &client,&c);
		  
	 	printf("GI:%s\n",user.u_id );

		if(recv(client_socket,&user.u_id,sizeof(user.u_id),0)==-1){
	  	perror("Receive Error");
	  	exit(-1);
	  }

	  else
	 		printf("Received\n");
	 	printf("GI:%s\n",user.u_id );


		search_ID(user.u_id);
		char *tmp = "as";//kayit tamamlanmadan menu'ye gecmesin
		if(send(client_socket,&tmp,sizeof(tmp),0) == -1){
	  	perror("Send Error");
	  	exit(-1);
		}
    recv(client_socket,&rm_status,sizeof(rm_status),0);
    printf("%c\n",rm_status );
    if(rm_status=='0')
			receive_message(client_socket);

	}	

  


	close(client_socket);
	
	return 0;
}




//int someInt = 368;
//char str[12];
//sprintf(str, "%d", someInt);