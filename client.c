#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "stipler.h"
#include <string.h>

char * merge_strings(char *s1,char *s2){
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  char *result;
  result =(char *) malloc((len1+len2+1)*sizeof(char));//+1 null karakteri için
  memcpy(result, s1, len1);
  memcpy(result+len1, s2, len2+1);//+1 null karakteri  için
  return result;
}
void send_Message(char * id,int network_socket){
  struct Msg m;
  m.read_receipt ='0';
  sprintf(m.gonderen_id ,"%s",id);

  printf("Mesaji iletmek istediginiz kisinin id-sini ve mesajinizi giriniz:" );
  scanf("%s",m.alan_id);
  scanf("%[^\n]%*c",m.mesaj);

  printf("%s\n",m.gonderen_id );
  printf("%s\n",m.alan_id);
  printf("%s\n",m.mesaj );

  if(send(network_socket,&m.gonderen_id,sizeof(m.gonderen_id),0)==-1){
    perror("Error ");
    exit(-1);
  }
  if(send(network_socket,&m.alan_id,sizeof(m.alan_id),0)==-1){
    perror("Error ");
    exit(-1);
  }

  // if(send(network_socket,&m.read_receipt,sizeof(m.read_receipt),0)==-1){
  //   perror("Error ");
  //   exit(-1);
  // }
  if(send(network_socket,&m.mesaj,sizeof(m.mesaj),0)==-1){
    perror("Error ");
    exit(-1);
  }
  printf("Sent\n");
}


int search_in_users(char * id){
  FILE *f;
  struct USER u;
  int a=0;
  f=fopen("users.txt","r");
  fread(&u,sizeof(struct USER),1,f);
  while(!feof(f)&&a==0){
    if(!strncmp(id,u.u_id,sizeof(id)))
      a=1;
    fread(&u,sizeof(struct USER),1,f);
  }

  return a;//varsa 1 dondurur
}
int search_in_contact(char *id,char * path){
  FILE *f;
  struct USER u;
  int a=0;

  f=fopen(path,"r");

  fread(&u,sizeof(struct USER),1,f);
  while(!feof(f)&&a==0){
    if(!strncmp(id,u.u_id,sizeof(id)))
      a=1;
    fread(&u,sizeof(struct USER),1,f);
  }

  return a;//varsa 1 dondurur
}
void delete_Contact(char * id,char * path){

  FILE *f;
  char * fname;
  fname = merge_strings(id,".txt");

  int a=search_in_contact(path,fname);
  if(a==0){
    printf("Kisi listenizde bulunamadi\n\n");
  }
  else{
    f=fopen(fname,"r");
    struct USER users[50];
    int i=0;
    fread(&users[i],sizeof(struct USER),1,f);
    while(!feof(f)){
      i++;
      fread(&users[i],sizeof(struct USER),1,f);
    }

    int j=0;
    fclose(f);
    f=fopen(fname,"w");

    for(j=0;j<i;j++){
      printf("\n%s-%s",path,users[j].u_id );
      if(strncmp(path,users[j].u_id,sizeof(path))){
        fwrite(&users[j],sizeof(struct USER),1,f);
        printf("girdi\n");
      }
    }

    printf("Kisi listenizden silindi\n\n");
    fclose(f);
  }

}

void list_contacts(char *id){
  FILE *f;
  int a=0,flag;
  struct USER u;
  char * fname;

  fname = merge_strings(id,".txt");

  f=fopen(fname,"r");
  fread(&u,sizeof(struct USER),1,f);
  while(!feof(f)){
    printf("%s %s %s\n",u.u_id,u.p_num,u.name );
    fread(&u,sizeof(struct USER),1,f);
  }

}


void add_Contact(char * id){

  FILE *f;
  int a=0,flag;
  struct USER u;
  char * fname;

  fname = merge_strings(id,".txt");

  f=fopen(fname,"a+");

  printf("Kisi bilgilerini sirasiyla giriniz(Id,TelefonNumarasi,Isim): ");
  scanf("%s %s %s",u.u_id,u.p_num,u.name);

  if(search_in_users(u.u_id)){
    if (!search_in_contact(u.u_id,fname)){

      fwrite(&u,sizeof(struct USER),1,f);
      printf("Kisi listenize eklendi.\n\n");

    }
    else{
      printf("Id zaten kayitli\n");
    }
  }
  else{
    printf("ID Bulunamadi \n\n");
  }

  fclose(f);
}




int main(int argc,char *argv[]){
  int network_socket;
  struct sockaddr_in server_address,client;
  int connection_status;
  struct Msg message;
  struct USER user;
  int a,c,id;
  char sm_status;
  int client_socket;
  int choice;

  c = sizeof(struct sockaddr_in);

  // printf("%s\n",argv[1] );

  strncpy(user.u_id,argv[1],sizeof(argv[1]));
  printf("GI:%s\n",user.u_id );

  //printf("%d\n",message.gonderen_id  );


  network_socket=socket(AF_INET,SOCK_STREAM,0);

  if(network_socket == -1){
  	perror("Error ");
  	exit(-1);
  }

  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(8084);
  server_address.sin_addr.s_addr=INADDR_ANY;



  client_socket=connect(network_socket, (struct sockaddr *)&server_address,sizeof(server_address));


  if(client_socket == -1){
  	perror("Client_socket Error ");
  	exit(-1);
  }


  if(send(network_socket,&user.u_id,sizeof(user.u_id),0) == -1){
	  	perror("Send Error");
	  	exit(-1);
	}



  //menus
  while(choice!=6){
    printf("1.List Contacs\n2.Add User\n3.Delete User\n4.Send Messages\n5.Check Messages\n6.Exit\n");
    printf("\nSecim yapin : ");
    scanf("%d",&choice);

    switch(choice){
      case 1:{
        list_contacts(argv[1]);
        break;
      }
      case 2:{
        add_Contact(argv[1]);
        break;
      }
      case 3:{
        printf("Silmek istediginiz kisi id'sini girin : ");

        char path[16];

        scanf("%s",path);

        delete_Contact(argv[1],path);
        break;
      }
      case 4:{
        sm_status='0';
        send(network_socket,&sm_status,sizeof(sm_status),0);
        send_Message(argv[1],network_socket);
        break;
      }
      case 5:{
        char secim,count,status;
        char mesaj[256];
        char check_m = '1';
        if(send(network_socket,&check_m,sizeof(check_m),0)==-1){
          perror("262 Error ");
          exit(-1);
        }
        printf("1.Gelen Mesajlar\n2.Giden Mesajlar:\nLutfen seciminizi yapin: ");
        scanf(" %c",&secim);
        if (secim=='1'){
          printf("in secim\n");
          if(send(network_socket,&secim,sizeof(secim),0)==-1){
            perror("269 Error ");
            exit(-1);
          }

          char altsecim;

          printf("1.Okunmamis Mesajlar\n2.Tum Mesajlar:\nLutfen seciminizi yapin: ");
          scanf(" %c",&altsecim);
          if(send(network_socket,&altsecim,sizeof(altsecim),0)==-1){
            perror("275 Error ");
            exit(-1);
          };
          if(altsecim=='1'){
            printf("altsecim\n");
            if(recv(network_socket,&count,sizeof(count),0)==-1){
              perror("281 Error ");
              exit(-1);
            }
            printf("Toplam %c- Okunmamis mesajiniz var\n",count );

            int say=count-'0';
            int i;
            printf("%d\n",say );
            for(i=0;i<say;i++){
              struct Msg m;
              if(recv(network_socket,&m.gonderen_id,sizeof(m.gonderen_id),0)==-1){
                perror("291 Error ");
                exit(-1);
              }
              printf("recv sonrasi\n");
              printf("%s-den Okunmamis mesajiniz var\n",m.gonderen_id);
              printf("Mesaji okumak ister misiniz? > ");
              scanf(" %c",&status);
              if(send(network_socket,&status,sizeof(status),0)==-1){
                perror("298 Error ");
                exit(-1);
              }
              if (status=='1'){
                printf("in if\n" );
                if(recv(network_socket,&m.mesaj,sizeof(m.mesaj),0)==-1){
                  perror("304 Error ");
                  exit(-1);
                }

                printf("%s-den gelen mesajiniz:\n-%s\n",m.gonderen_id,m.mesaj );
              }

            }
          }
        }
        else
          if(send(network_socket,&secim,sizeof(secim),0)==-1){
            perror("315 Error ");
            exit(-1);
          }

        break;
      }
      case 6:{
        char st = 'q';
        if(send(network_socket,&st,sizeof(st),0)==-1){
          perror("315 Error ");
          exit(-1);
        }
        break;
      }
    }
  }



  close(client_socket);
	return 0;
}
