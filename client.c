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

#define PORT 8110

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
  int a_id;
  sprintf(m.gonderen_id ,"%s",id);

  printf("Mesaji iletmek istediginiz kisinin id-sini ve mesajinizi giriniz:" );
  scanf(" %d",&a_id);
  scanf(" %[^\n]%*c",m.mesaj);

  // printf("%s\n",m.gonderen_id );
  // printf("%d\n",a_id);
  sprintf(m.alan_id,"%d",a_id);
  // printf("%s\n",m.alan_id );
  // printf("%s\n",m.mesaj );

  if(send(network_socket,&m.alan_id,sizeof(m.alan_id),0)==-1){
    perror("Error ");
    exit(-1);
  }

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
    printf("\n\nKisi listenizde bulunamadi\n\n");
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
     // printf("\n\n%s-%s\n\n",path,users[j].u_id );
      if(strncmp(path,users[j].u_id,sizeof(path))){
        fwrite(&users[j],sizeof(struct USER),1,f);
      }
    }

    printf("\n\nKisi listenizden silindi\n\n");
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
    printf("\n\n%s %s %s\n",u.u_id,u.p_num,u.name );
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

  printf("\nKisi bilgilerini sirasiyla giriniz(Id,TelefonNumarasi,Isim): ");
  scanf("%s %s %s",u.u_id,u.p_num,u.name);

  if(search_in_users(u.u_id)){
    if (!search_in_contact(u.u_id,fname)){

      fwrite(&u,sizeof(struct USER),1,f);
      printf("\nKisi listenize eklendi.\n\n");

    }
    else{
      printf("\nId zaten kayitli\n");
    }
  }
  else{
    printf("\nID Bulunamadi \n\n");
  }

  fclose(f);
}

void yeniUye(char * id){
    struct USER u;
    FILE *f;
    f=fopen("users.txt","a+");
    char * fname;
	printf("\n\nKullanici Bilgileriniz bulunamadi . \n" );
    printf("Lutfen Kullanici bilgilerini giriniz(Isim,TelefonNumarasi):");
    scanf("%s %s",u.name,u.p_num);
    sprintf(u.u_id,"%s",id);
    fwrite(&u,sizeof(struct USER),1,f);
    fclose(f);


    fname = merge_strings(id,".txt");

    f=fopen(fname,"a+");
}

void list_messages(char * id){
  struct Msg m;
  FILE *f;
  char * fname;
  char * status;
  fname=merge_strings(id,"gidenmessages.txt");
  // printf("32:%s\n",fname );
  f=fopen(fname,"a+");
  while(fread(&m,sizeof(struct Msg),1,f)){

    if(m.read_receipt=='0'){
        status="Okunmadi";
    }
    else{
        status="Okundu";
    }
    printf("\n%s - %s - %s\n",m.alan_id,m.mesaj,status);
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
  // printf("GI:%s\n",user.u_id );

  //printf("%d\n",message.gonderen_id  );


  network_socket=socket(AF_INET,SOCK_STREAM,0);

  if(network_socket == -1){
  	perror("Error ");
  	exit(-1);
  }

  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(PORT);
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

    char response;
    recv(network_socket,&response,sizeof(response),0);
    if (response=='0') {
        yeniUye(argv[1]);
    }

  //menus
  while(choice!=6){
    printf("\n1.List Contacs\n2.Add User\n3.Delete User\n4.Send Messages\n5.Check Messages\n6.Exit\n");
    printf("\nSecim yapin : ");
    scanf(" %d",&choice);

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
		struct Msg m;
        sm_status='1';
        if(send(network_socket,&sm_status,sizeof(sm_status),0)==-1){
          perror("262 Error ");
          exit(-1);
        }
        printf("\n1.Gelen Mesajlar\n2.Giden Mesajlar:\nLutfen seciminizi yapin: ");
        scanf(" %c",&secim);

        if(send(network_socket,&secim,sizeof(secim),0)==-1){
          perror("269 Error ");
          exit(-1);
        }

        if (secim=='1'){
	        //   printf("in secim\n");


	          char altsecim;

	          printf("\n1.Okunmamis Mesajlar\n2.Tum Mesajlar:\nLutfen seciminizi yapin: ");
	          scanf(" %c",&altsecim);

	          if(send(network_socket,&altsecim,sizeof(altsecim),0)==-1){
	            perror("275 Error ");
	            exit(-1);
	          };

	          if(altsecim=='1'){
	            // printf("altsecim\n");
	            if(recv(network_socket,&count,sizeof(count),0)==-1){
	              perror("281 Error ");
	              exit(-1);
	            }
	            printf("\nToplam %c- Okunmamis mesajiniz var\n",count );

	            int say=count-'0';
	            int i;
	            //printf(" %d\n",say );
				struct Msg m;
	            for(i=0;i<say;i++){
		            if(recv(network_socket,&m.gonderen_id,sizeof(m.gonderen_id),0)==-1){
		                perror("291 Error ");
		                exit(-1);
		            }
		            // printf("recv sonrasi\n");
		            printf("%s-den Okunmamis mesajiniz var\n",m.gonderen_id);
		            printf("Mesaji okumak ister misiniz?(1/0) > ");
		            scanf(" %c",&status);
		            if(send(network_socket,&status,sizeof(status),0)==-1){
		                perror("298 Error ");
		                exit(-1);
		            }
		            if (status=='1'){
		                // printf("in if\n" );
		                if(recv(network_socket,&m.mesaj,sizeof(m.mesaj),0)==-1){
		                  	perror("304 Error ");
		                  	exit(-1);
		                }

		                printf("\n%s-den gelen mesajiniz:\n->%s\n",m.gonderen_id,m.mesaj );
		            }

	            }
			}
			if(altsecim=='2'){
				int count,i;
				char c[4];
				// printf("%c\n",altsecim);
				// printf("%d NS\n",network_socket);

				if(recv(network_socket,&c,sizeof(c),0)==-1){
					perror("HATA");
				}
				// printf("%s\n",c );
				count = strtol(c,NULL,10);
				// printf("%d\n",count );

				for(i=0;i<count;i++){
					if(recv(network_socket,&m.mesaj,sizeof(m.mesaj),0)==-1){
						perror("HATA");
					}

					if(recv(network_socket,&m.gonderen_id,sizeof(m.gonderen_id),0)==-1){
						perror("HATA");
					}
					printf("\n%s -> %s\n",m.gonderen_id,m.mesaj );
				}

				// printf("for sonrasi\n");
			}
        }
		else{
		    list_messages(argv[1]);
		}
		// printf("break oncesi\n" );
		break;


      }
      case 6:{
        sm_status = 'q';
        if(send(network_socket,&sm_status,sizeof(sm_status),0)==-1){
          perror("315 Error ");
          exit(-1);
        }
        close(client_socket);
        break;
      }
    }
  }




	return 0;
}
