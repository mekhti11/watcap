#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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
void list_messages(char * id){
  struct Msg m;
  FILE *f;
  char * fname;
  char * status;
  fname=merge_strings(id,"gidenmessages.txt");
  f=fopen(fname,"r");
  fread(&m,sizeof(struct Msg),1,f);
  while(!feof(f)){
    if(m.read_receipt=='0')
      status="Okunmadi";
    else
      status="Okundu"; 
    printf("%s - %s - %s\n",m.alan_id,m.mesaj,status);
    fread(&m,sizeof(struct Msg),1,f);
  }
  fclose(f);
}

void change_message_status(char * id,char * path){
  struct Msg m,mesajlar[100];
  FILE *f;
  char * fname;
  int i=0,j;
  
  fname=merge_strings(id,"gelenmessages.txt");
  
  f=fopen(fname,"r");
  
  fread(&mesajlar[i],sizeof(struct Msg),1,f);
  while(!feof(f)){
    i++;
    fread(&mesajlar[i],sizeof(struct Msg),1,f);
  }
  fclose(f);
  
  f=fopen(fname,"w");
  for(j=0;j<i;j++){
    if(!strncmp(mesajlar[j].gonderen_id,path,sizeof(path))&&mesajlar[j].read_receipt=='0'){
      mesajlar[j].read_receipt='1';
  
  //    fwrite(&mesajlar[j],sizeof(struct Msg),1,f);
  
    }

    fwrite(&mesajlar[j],sizeof(struct Msg),1,f);
  
  }
  fclose(f);

  i=0;
 
  fname=merge_strings(path,"gidenmessages.txt");
  f=fopen(fname,"r");
  
  fread(&mesajlar[i],sizeof(struct Msg),1,f);
  while(!feof(f)){
    i++;
    fread(&mesajlar[i],sizeof(struct Msg),1,f);
  }
  fclose(f);
  
  f=fopen(fname,"w");
  for(j=0;j<i;j++){
    
    if(!strncmp(mesajlar[j].alan_id,id,sizeof(id))&&mesajlar[j].read_receipt=='0'){
      mesajlar[j].read_receipt='1';
      //fwrite(&mesajlar[j],sizeof(struct Msg),1,f);
    }
    
    fwrite(&mesajlar[j],sizeof(struct Msg),1,f);
  }
  fclose(f);

}

void check_Messages(char * id,int client_socket){
  struct Msg m;
  FILE *f;
  char * fname;
  char secim,altsecim,status;
  char path[8]; 
  char mesaj[256];
  
  recv(client_socket,&secim,sizeof(secim),0);
  
  if(secim=='1'){
    recv(client_socket,&altsecim,sizeof(altsecim),0);
    if(altsecim=='1'){
      int id_dizi[16];
      fname=merge_strings(id,"gelenmessages.txt");
      
      f=fopen(fname,"r");
      fread(&m,sizeof(struct Msg),1,f);
      int a=0;
      while(!feof(f)){
        if(m.read_receipt=='0')
          a++;
          fread(&m,sizeof(struct Msg),1,f);
      }
      char count=a+'0';
      printf("%c\n",count );
      if(send(client_socket,&count,sizeof(count),0)==-1){
        perror("125 Error ");
        exit(-1);
      }
      fclose(f);
      
      f=fopen(fname,"r");
      fread(&m,sizeof(struct Msg),1,f);
      while(!feof(f)){
        if(m.read_receipt=='0'){
          printf("%s - %s\n",m.gonderen_id,m.mesaj);
          send(client_socket,&m.gonderen_id,sizeof(m.gonderen_id),0);
          
          recv(client_socket,&status,sizeof(status),0);
          
          if(status=='1'){
            //strncpy(mesaj,m.mesaj,sizeof(m.mesaj));
            printf("%s\n",m.mesaj );
            if(send(client_socket,&m.mesaj,sizeof(m.mesaj),0)==-1){
              perror("262 Error ");
              exit(-1);
            }
            strncpy(path,m.gonderen_id,sizeof(m.gonderen_id));
            change_message_status(id,path);
          }
        }
        fread(&m,sizeof(struct Msg),1,f);
      }
      fclose(f);
    }


    if(altsecim=='2'){
      fname=merge_strings(id,"gelenmessages.txt");
      f=fopen(fname,"r");
      
      fread(&m,sizeof(struct Msg),1,f);

      while(!feof(f)){
        printf("%s - %s\n",m.gonderen_id,m.mesaj);
        fread(&m,sizeof(struct Msg),1,f);
        strncpy(path,m.gonderen_id,sizeof(m.gonderen_id));
        change_message_status(id,path);
      }
      fclose(f);
    }
  
  }
  
  else if(secim=='2'){
   list_messages(id);
  }

}
void receive_message(int client_socket){
  struct Msg m;
  FILE *f;
  FILE *f1;
  char * fname;
  char * fname1;
  char tmp[16];

  if(recv(client_socket,&m.gonderen_id,sizeof(m.gonderen_id),0)==-1){
    perror("Receive Error");
    exit(-1);
  }
  printf("gi:%s\n",m.gonderen_id);

  //strncpy(m.gonderen_id,tmp,sizeof(tmp));

  //printf("gi:%s\n",tmp);
  
  recv(client_socket,&m.alan_id,sizeof(m.alan_id),0);
  printf("AI:%s\n",m.alan_id);
  
  recv(client_socket,&m.read_receipt,sizeof(m.read_receipt),0);
  printf("RR:%c\n",m.read_receipt);
  
  recv(client_socket,&m.mesaj,sizeof(m.mesaj),0);
  printf("Msg:%s\n",m.mesaj);
  
  fname = merge_strings(m.alan_id,"gelenmessages.txt");
  fname1=merge_strings(m.gonderen_id,"gidenmessages.txt");
  
  f=fopen(fname,"a+");
  f1=fopen(fname1,"a+");
  printf("%s - %s - %s - %c\n",m.gonderen_id,m.alan_id,m.mesaj,m.read_receipt);
  fwrite(&m,sizeof(struct Msg),1,f);
  fwrite(&m,sizeof(struct Msg),1,f1);
  
  fclose(f);
  fclose(f1);
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

void * login(void * data ){
  struct STR *cs = (struct STR *) data;
  struct USER user;
  char st;

  if(recv(cs->client_socket,&user.u_id,sizeof(user.u_id),0)==-1){
    perror("Receive Error");
    exit(-1);
  }
  printf("GI:%s\n",user.u_id );

  search_ID(user.u_id);

  while(1){
    if(recv(cs->client_socket,&st,sizeof(st),0)==-1){
      perror("Receive Error");
      exit(-1);
    }
    if(st=='0')
      receive_message(cs->client_socket);

    if(st=='1')
      check_Messages(user.u_id,cs->client_socket);
    if (st=='q'){
      break;
    }
  }
}
  
int main(int argc, char const *argv[]){
  
  struct sockaddr_in server_address,client;
  int server_socket;
  int client_socket,c;
  struct Msg message;
  struct USER user;
  ssize_t t;
  char rm_status;
  pthread_t thread;
  struct STR *arg;
  arg = (struct STR *)calloc(1,sizeof(struct STR ));

  c = sizeof(struct sockaddr_in);

  server_socket=socket(AF_INET,SOCK_STREAM,0);
	perror("Socket Error ");
  
  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(8084);
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
    
    arg->client_socket = client_socket;
    if(pthread_create(&thread,NULL,login,(void *) arg)){
      printf("Error while creating thread\n");
    }

    printf("thread sorasi\n");

	}	

  


	close(client_socket);
	
	return 0;
}




//int someInt = 368;
//char str[12];
//sprintf(str, "%d", someInt);