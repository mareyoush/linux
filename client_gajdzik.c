#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>

#define MAXDATASIZE 1000 // maksymalna ilosc dancyh, ktore mozemy wyslac
int main(int argc, char *argv[])
{
	//od gajdzika
	int i,j,k,l,z;
	char adres[16], siec[100], maska[16], msg[100];
	int ile_srednikow;
	char odp[1000];
	bool wyslane=false;
	//
        int sockfd, numbytes, wyjscie=0, wyslano, n;
        unsigned int port;
        char buffer[MAXDATASIZE];// port, z którym klient bedzie sie laczyc
        char msg2[MAXDATASIZE];
        struct sockaddr_in their_addr;// informacja o adresie osoby laczacej sie
        struct hostent *he;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }
        their_addr.sin_family = AF_INET; 
        
        if (argc == 1)
            {
            their_addr.sin_port = htons(5555); 
            their_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
            }
        else if (argc == 2) {
            their_addr.sin_port = htons(5555); 
            their_addr.sin_addr.s_addr = inet_addr((const char*)argv[1]);
        }
    	else if (argc==3){
            port=atoi(argv[2]);
            their_addr.sin_port = htons(port); 
            their_addr.sin_addr.s_addr = inet_addr((const char*)argv[1]);
    	}
    	else
    	{
            printf("Błędnie podany port lub ip\n");
            exit(1);
    	}
        memset(&(their_addr.sin_zero), '\0', 8);  // wyzeruj reszte struktury
        if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
            perror("Nawiazanie polaczenia nie powiodlo sie!\n");
            exit(1);
        }
        printf("Polaczenie nawiazane\n");
        fflush(stdout);
while(1)
{
//od gajdzika
if(wyslane==false)
{
	ile_srednikow=0;
	i=4;
	j=4;
		printf("-------------------------\n");
		printf("Co chcesz zrobić?\n");
		printf("1. Wyświetl listę dostępnych interfejsów sieciowych\n");
		printf("2. Dane szczegółowe interfejsów\n");
		printf("3. Ustawić na serverze\n");
		printf("4. Exit\n");
		scanf("%d",&i);
		switch(i)
		{
		case 1:
			odp[0]='1';
			odp[1]='\0';
			printf("%s\n",odp);
			write(sockfd,odp,sizeof(odp));
			wyslane=true;
			break;
		case 2:
			printf("Podaj nazwy interfejsów sieciowych odzielonych średnikiem, których chcesz wyświetlić dane\n");
			scanf("%s",siec);
			odp[0]='2';
			odp[1]=';';
			for (k=1; k<=strlen(siec);k++)
				{
				odp[k+1]=siec[k-1];
				}
			odp[k+1]='\0';
			ile_srednikow=0;
			int b;
			for (b=0; b<=k; b++) //zliczanie średników
			{
				if (odp[b]==';')
				ile_srednikow+=1;
			}
			printf("%s\n",odp);
			write(sockfd,odp,sizeof(odp));
			wyslane=true;
			break;
		case 3:
			printf("--------------------\n");
			printf("1.Ustawić adres fizyczny na interfejsie sieciowym\n");
			printf("2.Ustawić adres IP i maskę sieciową na interfejsie sieciowym\n");
			printf("3.Cofnij\n");
			odp[0]='3';
			odp[1]=';';
			scanf("%d",&j);
			switch(j)
			{
			case 1:
				l=0;
				odp[2]='1';
				odp[3]=';';
				printf("Podaj nazwe interfejsu sieciowego na którym chcesz zmienić\n");
				scanf("%s",siec);		
				z=4+(strlen(siec));
				for (k=4; k<z;k++)
					{
					odp[k]=siec[l];
					l++;
					}
				odp[k]=';';
				k++;
				printf("Podaj adres fizyczny\n");
				scanf("%s",adres);
				l=0;
				z=k+strlen(adres);
				for (;k<z;k++)
					{
					odp[k]=adres[l];
					l++;
					}
				k++;
				odp[k]='\0';
				printf("%s\n",odp);
				write(sockfd,odp,sizeof(odp));
				wyslane=true;
				break;
			case 2:
				l=0;
				odp[2]='2';
				odp[3]=';';
				printf("Podaj nazwe interfejsu sieciowego na którym chcesz zmienić\n");
				scanf("%s",siec);
				z=4+strlen(siec);
				for (k=4; k<z;k++)
					{
					odp[k]=siec[l];
					l++;
					}
				odp[k]=';';
				k++;
				printf("Podaj adres IP\n");
				scanf("%s",adres);
				l=0;
				z=k+strlen(adres);
				for (k;k<z;k++)
					{
					odp[k]=adres[l];
					l++;
					}
				odp[k]=';';
				k++;
				printf("Podaj maske\n");
				scanf("%s",maska);
				l=0;
				z=k+strlen(maska);
				for (k;k<z;k++)
					{
					odp[k]=maska[l];
					l++;
					}
				odp[k]='\0';
				printf("%s\n",odp);
				write(sockfd,odp,sizeof(odp));
				wyslane=true;
				break;		
			default:
			break;
			}
			break;
		case 4:
			printf("exit\n");
			exit(1);
			break;
		default :
			break;
		}
}
	else
	{
		if(read(sockfd,msg,sizeof(msg)))
		{
			//2;eth0;lo - ile srednikow
			if(odp[0]=='2' && ile_srednikow>1)
			{
			printf ("1 ile srednikow %d !!!", ile_srednikow);
			int i;
				for ( i=1; i<=ile_srednikow; i++)
				{	
					printf ("2 ile srednikow %d", ile_srednikow);
					printf(" kurwa %s lol",msg);
					if (i==ile_srednikow){
						ile_srednikow=0;
						wyslane=false;
						break;
					}
					int lol=0;
					while(lol==0)
						{
						if(read(sockfd,msg,sizeof(msg)))
							lol=1;
						printf("1 kurwa ");
						//puste
						}
				}
			}
			else
			{
			wyslane=false;
			printf("%s",msg);
			}
		}
	}
}
        return 0;
    }
