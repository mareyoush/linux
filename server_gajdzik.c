#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
 

#define BACKLOG 10
#define MAXDATASIZE 1000 // maksymalna ilość dancyh, jaką możemy odebrać



char *show_network_interfaces(const int fd)
{
    char *destination = (char*)malloc(50);
    int i = 0;
    unsigned int MAX_INTERFACES = 128;
    struct ifreq ifr[MAX_INTERFACES];
    struct ifconf config;
    config.ifc_len = MAX_INTERFACES * sizeof(struct ifreq);
    config.ifc_buf = (char *)ifr;

    //ioctl modify ifconf.ifc_len to amount of interfaces * sizeof structure
    if(0 > ioctl(fd, SIOCGIFCONF, (char *)&config)){
     //   printf("ioctl error (SIOCGIFCONF): \'%s\'\n", strerror(errno));
       // return -1;
    }

    for(i = 0; i < config.ifc_len / (sizeof(struct ifreq)); ++i){
        strcat(destination, ifr[i].ifr_name);
        strcat(destination, "\n");
    }

    return destination;
}


char *show_netmask(const int fd, const char *interface_name)
{
	char *str = (char*)malloc(50);
	struct ifreq ifr;
	strncpy(ifr.ifr_name, interface_name, IFNAMSIZ-1);
	ioctl(fd,SIOCGIFNETMASK,&ifr);
	sprintf(str,"Netmask :  %s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
return str;
}
char *show_ipv4(const int fd, const char *interface_name)
{
	char *str = (char*)malloc(50);
	struct ifreq ifr;
	strncpy(ifr.ifr_name, interface_name, IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	sprintf(str,"IPv4 : %s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
return str;
}
char *show_mac(const int fd, const char *interface_name)
{
	char *str = (char*)malloc(50);
	struct ifreq ifr;
	strncpy(ifr.ifr_name, interface_name, IFNAMSIZ-1);
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	sprintf(str,"MAC Address : %02x:%02x:%02x:%02x:%02x:%02x",
	  (unsigned char) ifr.ifr_hwaddr.sa_data[0],
	  (unsigned char) ifr.ifr_hwaddr.sa_data[1],
	  (unsigned char) ifr.ifr_hwaddr.sa_data[2],
	  (unsigned char) ifr.ifr_hwaddr.sa_data[3],
	  (unsigned char) ifr.ifr_hwaddr.sa_data[4],
	  (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
return str;
}
char *show_interface_info(const int fd, char * name)
{
	//printf("%s",show_ipv4(fd,"eth0"));
	//printf("%s",show_netmask(fd,"eth0"));
	//printf("%s",show_mac(fd,"eth0"));
	char *str = (char*)malloc(1000);
	sprintf(str, "%s\n%s\n%s\n", show_ipv4(fd,name),show_mac(fd,name),show_netmask(fd,name));
return str;
}
void sigchld_handler(int s)
   {
        while(wait(NULL) > 0);
    }
 

int func (int sd, int new_sd, char str[])
{
 // char str[] ="eth0;lo;eth1;eth2;eth3";
  char * msg2 = (char*)malloc(100);
  char * name;
  int wys;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  name = strtok (str,";");
  name = strtok (NULL,";");


while (name != NULL)
{

   sprintf(msg2, "%s", show_interface_info(sd,name));  
   if((wys = write(new_sd,msg2,1000)) < 0){
         perror("send");
   }    
  
    printf ("%s\n",name);
    printf("co wysylamy : %s\n", msg2);
    name = strtok (NULL, ";");
  }
  return 0;
}



int main(int argc, char *argv[])
{
	int fd, sd, rc, port, new_sd,wyslano;
	struct sockaddr_in s_addr, c_addr;
	struct sigaction sa;
	int c_addrlen = sizeof(c_addr);
   	char buf[MAXDATASIZE];
   	char msg[MAXDATASIZE];
   	char *msg2 = (char*)malloc(100);
	int numbytes;
	 //  struct ifreq ifr;
	        
	//ioctl(sd,SIOCGIFNETMASK,&ifr);
	//printf("Netmask :  %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
   if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
   {
      perror("blad funkcji socket():");
      exit(-1);
   }
   memset(&s_addr, 0, sizeof(struct sockaddr));
   s_addr.sin_family      = AF_INET;
   
      if (argc == 1)
         {
         s_addr.sin_port = htons(5555); 
         s_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
         }
        else if (argc == 2) {
         s_addr.sin_port = htons(5555); 
            s_addr.sin_addr.s_addr = inet_addr((const char*)argv[1]);
        }
         else if (argc==3){
         port=atoi(argv[2]);
         s_addr.sin_port = htons(port); 
            s_addr.sin_addr.s_addr = inet_addr((const char*)argv[1]);
          }
         else
          {
         printf("Błędnie podany port lub ip\n");
         exit(1);
          }
   rc = bind(sd, (struct sockaddr *)&s_addr, sizeof(s_addr)); 
   if (rc< 0) 
   {
      perror("blad funkcji bind():");
      close(sd);
      exit(-1);
   }
   rc = listen(sd, BACKLOG); 
   if (rc==-1)
   {
      perror("blad funkcji listen():");
      exit(1);
   }
/*
 ifr.ifr_addr.sa_family = AF_INET;
// strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
 ioctl(sd, SIOCGIFADDR, &ifr);
printf("IPv4 : %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
ioctl(sd, SIOCSIFADDR, &ifr);
printf("IPv6 : %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
 ioctl(sd, SIOCGIFHWADDR, &ifr);
printf("MAC Address : %02x:%02x:%02x:%02x:%02x:%02x\n",
  (unsigned char) ifr.ifr_hwaddr.sa_data[0],
  (unsigned char) ifr.ifr_hwaddr.sa_data[1],
  (unsigned char) ifr.ifr_hwaddr.sa_data[2],
  (unsigned char) ifr.ifr_hwaddr.sa_data[3],
  (unsigned char) ifr.ifr_hwaddr.sa_data[4],
  (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
//ioctl(sd,SIOCGIFNETMASK,&ifr);
//printf("Netmask :  %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
*/
//msg2 = show_interface_info(sd);

 while(1) 
   {
      if ((new_sd = accept(sd, (struct sockaddr *) &c_addr, &c_addrlen)) == -1) 
      {
         perror("accept");
         continue;
      }
      printf("polaczono z: %s\n",inet_ntoa(c_addr.sin_addr));
        while(1) 
   {
         if ((numbytes=recv(new_sd, buf, MAXDATASIZE-1, 0)) == -1) 
         {
            perror("recv");
            exit(1);
         }
         buf[numbytes] = '\0';
         printf("serwer tcp odebral: %s \n",buf);

   //    printf("Podaj tekst do wyslania: ");
     //  scanf("%s", msg);

if(buf[0]=='1'){
	printf("w bufie jest 1");
	sprintf(msg2,"%s",show_network_interfaces(sd));
   if ((wyslano = write(new_sd,msg2,100)) < 0) {
            perror("send");
   }
}
if(buf[0]=='2') {
	printf("w bufie jest 2");
	func (sd, new_sd,buf);
	//sprintf(msg2, "%s", show_interface_info(sd));
  	//if ((wyslano = write(new_sd,msg2,100)) < 0) 
          //  perror("send");
   
}
          if(numbytes == 0)
             {
               puts("Client disconnected");
               fflush(stdout);
               close(new_sd);
               break;
             }
          else if(numbytes == -1)
             {
              perror("recv failed");
              close(new_sd);
              break;
             }
      }
    
   }
   return (1);
}
