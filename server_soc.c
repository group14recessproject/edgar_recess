#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <netdb.h>
#include <mariadb/mysql.h>
#include <mariadb/server/mysql.h>
#define PORT 4950
#define BUFSIZE 1024

//Compilation : gcc -I/opt/lampp/include  -o server_soc server_soc.c `/opt/lampp/bin/mysql_config --cflags --libs` 
 
struct member mainproc(struct member mainmemberob);
struct member addmproc(struct member mainmemberob);
void addsproc(struct member mainmemberob);
struct member chekproc(struct member mainmemberob);
struct member getstmtproc(struct member mainmemberob);
struct member srchproc(struct member mainmemberob); 
 
void bbzero(char * strg);
int strchNo(char arr[]); 
int strbg(char strn[]); 
 
//=======member structure============== 
struct member { 
            char messageStr[1200]; 
            char memberName[80];
            char memberID[30];
	        char enrollmentDate[30];
            char memberRecommender[30]; 
            char memberGender[12];
            char agentUsername[30];
            char agentDistrict[20];
            int  recommendedMembers; 
            char sourceIP[30];
	        int portNo;
	        char daterecv[15];
	        char timerecv[15];  
         }; 
	//structure instances 
	struct member memberob;
	struct member memberobret; 
	struct member ret_structr;
//=======member structure end===========
 
FILE *fileptrstatus;
FILE *sgnsfile;
FILE *mbrsfile;

 time_t t; struct tm tmm;

int ras=0,bbg=0,bgg=0,ddx,end=9,ix=0,min,newMin,prior1pos,ax,sh=0,vvx, iix, occ, xi, maxoc, newMax,c,hh,cm, occurr[10]; 
int ixx,cxx,x,zx,maxpos,cmpp,i1,i2,i3,i4,bxxx,rp,op=0,nextready=0,secs,starttime,curtime,bggr=0,endr=9,headerprinted=0;
int wa,wa2,wa3,wa4;
int fdmax,sockfd,jjn;
fd_set master;
//========================================
	MYSQL *conn;
	MYSQL_RES *rset; 
	MYSQL_ROW row;
	static char *host = "localhost";
	static char *user = "root";
	static char *pass = "";
	static char *dbname = "recess_political_party"; 
	unsigned int mysqlport = 3306;
	static char *unix_socket = NULL;
	unsigned int flag = 0;
	char querr1[500],querr2[500],querr2b[500],querr3[100];
//========================================

void send_to_all(int j, int sockfd,  struct member ret_structr, fd_set *master) { //sending to all
	if (FD_ISSET(j, master)){
		if (j != sockfd) {
			if (send(j, &ret_structr, sizeof(ret_structr), 0) == -1) {
				perror("send");
		} } } } //......................................................................
		
void new_recv(int i, fd_set *master, int sockfd, int fdmax,struct sockaddr_in my_addr,struct sockaddr_in client_addr)//sending and receiving fuction
{  
    
    t = time(NULL); tmm =  *localtime(&t);
	int nbytes_recvd, j;
	char msgvar[1200], recv_buf[BUFSIZE], buf[BUFSIZE]; 
	if ((nbytes_recvd = read(i, &memberob, sizeof(memberob))) <= 0) {
		if (nbytes_recvd == 0) { printf("socket %d hung up\n", i);
		}else { perror("recv"); }
		close(i);
		FD_CLR(i, master);
	}else {   
	                                                         if(headerprinted==0){
	printf("\nmember_name |  gender  | recommender |  Date    |   Time |");
	printf("\n============================================================="); headerprinted=1;
	                                                            }
	     //access all the fields of the recieved object from memberob
	     printf("\nReceived request --------------------------------------");
	     printf("\n%s\nFrom agent : %s\nDistrict : %s\n",memberob.messageStr,memberob.agentUsername,memberob.agentDistrict);
	     
  }  }//...............................end of new_recv() function.......................................
		
void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd; 	
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept"); exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){  *fdmax = newsockfd;  }
        printf("\nNew client connection from %s on port %d \n",
        inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
	}
 headerprinted=0;
}
	
void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket"); exit(1); } 
	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, ' ', sizeof my_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt"); exit(1); }
		
	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind"); exit(1); }
	if (listen(*sockfd, 10) == -1) { perror("listen"); exit(1); }
	printf("\nServer Waiting for client on port 4950 .....");
	fflush(stdout);
}

int main(){

	//fd_set master;   //declared globally instead.
	fd_set read_fds;
	int i;
	sockfd= 0;
	struct sockaddr_in my_addr, client_addr;
	FD_ZERO(&master);
	FD_ZERO(&read_fds); 
	connect_request(&sockfd, &my_addr);
	
	FD_SET(sockfd, &master); 
	fdmax = sockfd;
	 
	while(1){ 
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){ perror("select"); exit(4); }
		
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				  if (i == sockfd)
					connection_accept(&master, &fdmax, sockfd, &client_addr);
				  else
					new_recv(i, &master, sockfd, fdmax,my_addr, client_addr);
			 }
		}//end for loop
	//******************************* processing ******************************** 
		  //*********=======calling main processor to process========************  
			  ret_structr = mainproc(memberob);
			  
			  strcpy(ret_structr.agentUsername,memberob.agentUsername);
			  strcpy(ret_structr.agentDistrict,memberob.agentDistrict);
 				
   			  //call the send to all function after processing
		      for (jjn = 0; jjn <= fdmax; jjn++){ 
			       send_to_all(jjn, sockfd, ret_structr, &master ); 
		       } bbzero(ret_structr.messageStr); bbzero(ret_structr.agentUsername);
		  //********************************************	
	}//end while(1)
	 
	return 0;
} //end of main() function 
//===============================================================================================================================

struct member mainproc(struct member mainmemberob){
    int a,bg,b,c=0;
    char cmmd[5],messageStrcopy[1200],singlememberStr[1200];
    time_t strtm,finitm,processingdur;
    struct timeval start, end; 
    long mtime, mstime, seconds, useconds;    
 
    gettimeofday(&start, NULL);
    usleep(2000);
     gettimeofday(&end, NULL); 
       seconds  = end.tv_sec  - start.tv_sec;
       useconds = end.tv_usec - start.tv_usec; 
       mtime = seconds + useconds;
       mstime = useconds; 
     
    for(a=0;a<4;a++){ sprintf(&cmmd[a],"%c",mainmemberob.messageStr[a]); }

    struct member agentsResponse;
    struct member pasdmember; 
    pasdmember = mainmemberob;
      sprintf(&cmmd[0],"%c",mainmemberob.messageStr[0]);
      sprintf(&cmmd[1],"%c",mainmemberob.messageStr[1]);
      sprintf(&cmmd[2],"%c",mainmemberob.messageStr[2]);
      sprintf(&cmmd[3],"%c",mainmemberob.messageStr[3]);


       if(strcmp(cmmd,"addm")==0 || strcmp(cmmd,"Addm")==0){ 
                  agentsResponse = addmproc(pasdmember);
       }else if(strcmp(cmmd,"chec")==0 || strcmp(cmmd,"Chec")==0){
                  agentsResponse = chekproc(pasdmember);
       }else if(strcmp(cmmd,"get_")==0 || strcmp(cmmd,"Get_")==0){
                 agentsResponse = getstmtproc(pasdmember);
       }else if(strcmp(cmmd,"adds")==0){ 
       			addsproc(pasdmember);
       }else if(strcmp(cmmd,"sear")==0 || strcmp(cmmd,"Sear")==0){
                  agentsResponse = srchproc(pasdmember);
       }else{
         //unknown command.
       }   
        
    return agentsResponse;

}/*end of mainproc function*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct member addmproc(struct member mainmemberob){
		char signatr[2],signaturesymbol[]="***signature null***\n";
		char strtoken1[15],strtoken2[15],strtoken3[15],recommendertoken6[30],gendertoken5[30],datetoken4[30];
		 strcpy(strtoken1 , strtok(mainmemberob.messageStr, " ")); 
		 strcpy(strtoken2 , strtok(NULL, " ")); //first name
		 strcpy(strtoken3 , strtok(NULL, " ")); //second name
		 strcpy(datetoken4 , strtok(NULL, " "));
		 strcpy(gendertoken5 , strtok(NULL, " "));
		 strcpy(recommendertoken6 , strtok(NULL," "));
		 strcpy(mainmemberob.memberName,"");
		 strcat(mainmemberob.memberName,strtoken2);
		 strcat(mainmemberob.memberName," ");
		 strcat(mainmemberob.memberName,strtoken3);
		 sprintf(mainmemberob.messageStr,"Added member : '%s' .\n",mainmemberob.memberName);
		 sprintf(mainmemberob.enrollmentDate,"%s",datetoken4);
		 sprintf(mainmemberob.memberGender,"%s",gendertoken5);
		 sprintf(mainmemberob.memberRecommender,"%s",recommendertoken6);
	   conn = mysql_init(NULL); 
	    /*connecting to the database. NB: the function mysql_real_connect(..) returns 0 upon success and non zero when failed. */
		if(!(mysql_real_connect(conn,host,user,pass,dbname,mysqlport,unix_socket,flag))){ fprintf(stderr,"Error !! : %s\n",mysql_error(conn) ); 
		}else{  printf("\nAdding the member ... "); } 
		sprintf(querr1,"CREATE TABLE if not exists members(memberName varchar(30), memberID varchar(30), enrollmentDate varchar(30), memberRecommender varchar(30),memberGender varchar(12),enrolledBy varchar(30),districtName varchar(30),recommendedMembers int(11))"); 
		
		/*
		sprintf(querr1b,"CREATE TABLE if not exists agents(agentName varchar(30), agentUsername varchar(30),signatre varchar(2),districtName varchar(30),agentHead boolean default false, wageUnits int(11) )"); 
		sprintf(querr1c,"CREATE TABLE if not exists districts(districtName varchar(30), headAgent varchar(30), no_of_members int(11) default 0, no_of_agents int(11) default 0 )"); 
		sprintf(querr1d,"CREATE TABLE if not exists admin(adminUsername varchar(30), adminpassword varchar(100))"); 
		sprintf(querr1e,"CREATE TABLE if not exists donations(donationID varchar(30) AUTO_INCREMENT, amountdonated int(11), month varchar(20),donatedBy varchar(30))"); 
		sprintf(querr1f,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Entebbe', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1g,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Mbale', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1h,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Gulu', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1i,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Mbarara', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1j,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Jinja', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1k,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Iganga', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1l,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Masaka', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1m,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Kasese', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1n,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Hoima', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1o,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Mukono', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	sprintf(querr1p,"INSERT INTO `districts` (`districtName`, `headAgent`, `no_of_members`, `no_of_agents`) VALUES ('Kampala', NULL, '0', '0'), ('Jinja', NULL, '0', '0')");
	  	*/
	  	
	  	sprintf(querr2,"INSERT INTO members set memberName='%s', memberID='%s', enrollmentDate='%s', memberRecommender='%s',memberGender='%s',enrolledBy='%s',districtName='%s',recommendedMembers='%d'",mainmemberob.memberName,mainmemberob.memberID,mainmemberob.enrollmentDate,mainmemberob.memberRecommender,mainmemberob.memberGender,mainmemberob.agentUsername,mainmemberob.agentDistrict,0);
	  	sprintf(querr3,"SELECT signature FROM agents where username='%s' limit 1",mainmemberob.agentUsername);
	  	sprintf(querr2b,"UPDATE districts set no_of_members=(no_of_members+1) where districtName='%s' ",mainmemberob.agentDistrict);
	  	mysql_query(conn,(querr1));
	  	mysql_query(conn,(querr2)); 
	  	mysql_query(conn,(querr2b));
	  	mysql_query(conn,(querr3));
	  	rset = mysql_store_result(conn);
		while(row = mysql_fetch_row(rset)){ strcpy(signatr,row[0]); }
		if(strcmp(signatr,"B")==0 || strcmp(signatr,"b")==0){  
			strcpy(signaturesymbol,"** \n* *\n** \n* *\n** \n");
		}else if(strcmp(signatr,"C")==0 || strcmp(signatr,"c")==0){ 
			strcpy(signaturesymbol," **\n*  \n*  \n*  \n **\n");
		}else if(strcmp(signatr,"D")==0 || strcmp(signatr,"d")==0){ 
			strcpy(signaturesymbol,"** \n* *\n* *\n* *\n** \n");
		}else if(strcmp(signatr,"E")==0 || strcmp(signatr,"e")==0){ 
			strcpy(signaturesymbol,"***\n*  \n***\n*  \n***\n");
		}else if(strcmp(signatr,"F")==0 || strcmp(signatr,"f")==0){ 
			strcpy(signaturesymbol,"***\n*  \n***\n*  \n*  \n");
		}else if(strcmp(signatr,"G")==0 || strcmp(signatr,"g")==0){ 
			strcpy(signaturesymbol," **\n*  \n* *\n* *\n **\n");
		}else if(strcmp(signatr,"H")==0 || strcmp(signatr,"h")==0){ 
			strcpy(signaturesymbol,"* *\n* *\n***\n* *\n* *\n");
		}else if(strcmp(signatr,"I")==0 || strcmp(signatr,"i")==0){ 
			strcpy(signaturesymbol,"***\n * \n * \n * \n***\n");
		}else if(strcmp(signatr,"L")==0 || strcmp(signatr,"l")==0){ 
			strcpy(signaturesymbol,"*  \n*  \n*  \n*  \n***\n");
		}else if(strcmp(signatr,"A")==0 || strcmp(signatr,"a")==0){ 
			strcpy(signaturesymbol," * \n* *\n***\n* *\n* *\n");
		}else if(strcmp(signatr,"J")==0 || strcmp(signatr,"j")==0){ 
			strcpy(signaturesymbol,"***\n  *\n  *\n* *\n***\n");
		}
	  	sgnsfile = fopen("signitures.txt","a+");
	  	fprintf(sgnsfile,"%s \n______%s____\n",signaturesymbol,mainmemberob.agentUsername); //write signiture symbols to the file signitures.txt
	  	
	 	 //write message string to the file members.txt
	 	 printf("\nRequest: --------------------------------------");
	     printf("\n%s\nFrom agent : %s\nDistrict : %s\n",memberob.messageStr,memberob.agentUsername,memberob.agentDistrict);
      
		mysql_free_result(rset);  
		mysql_close(conn);
		return mainmemberob;
    }
    
void addsproc(struct member mainmemberob){ 
	  //"""""""""""""""""
	  	char messageStrcopy[1200],singlememberStr[1200];
	  	int i,jjn;
	    bbzero(messageStrcopy);  
       			  
       			  struct member tempmember; 
    			  tempmember = mainmemberob; 
    			  strcpy(tempmember.memberRecommender,"none");
       			  for(i=5;i<strlen(mainmemberob.messageStr);i++){ messageStrcopy[i-5] = mainmemberob.messageStr[i]; }
       			   for(i=0;i<strlen(messageStrcopy);i++){ 
		   			   	if(messageStrcopy[i]==';'){
		   			   		//call addmproc 
		   			   		strcpy(tempmember.messageStr,"addmember ");
		   			   		strcat(tempmember.messageStr,singlememberStr);
		   			   		//call the addmproc() with this ready single member .
		   			   		ret_structr = addmproc(tempmember);
		   			   		//send the new ret_structr and then continue looping .+++++++++++++++++
		   			   		for(jjn = 0; jjn <= fdmax; jjn++){ 
							   send_to_all(jjn, sockfd, ret_structr, &master ); 
							 } bbzero(ret_structr.messageStr); bbzero(ret_structr.agentUsername); 
		   			   		//clean buffer+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		   			   		bbzero(mainmemberob.messageStr);
		   			   		bbzero(singlememberStr); 
		   			        //TODO: pause a bit 
		   			   	}else{
		   			   		singlememberStr[i]=messageStrcopy[i];
		   			   	}
       			   }//end of for loop 
	  //"""""""""""""""""
   }
   
struct member chekproc(struct member mainmemberob){
	
}
struct member getstmtproc(struct member mainmemberob){
	
}
struct member srchproc(struct member mainmemberob){
	char tempstr[1200],commd[7],criterion[50],searchqry;
	char memberName[30],memberID[30],enrollmentDate[30],memberRecommender[30],memberGender[12],enrolledBy[30],districtName[30],recommendedMembers[30];
	char resultrow[300];
	strcpy(tempstr,mainmemberob.messageStr);
	strcpy(commd,strtok(tempstr," "));
	strcpy(criterion,strtok(NULL," "));
		conn = mysql_init(NULL); 
	    /*connecting to the database. NB: the function mysql_real_connect(..) returns 0 upon success and non zero when failed. */
		if(!(mysql_real_connect(conn,host,user,pass,dbname,mysqlport,unix_socket,flag))){ fprintf(stderr,"Error !! : %s\n",mysql_error(conn) ); 
		}else{  printf("\nRetrieving the searched items ... "); } 
	sprintf(searchqry,"SELECT * FROM members WHERE %s",criterion);
	mysql_query(conn,(searchqry));
	strcpy(mainmemberob.messageStr,""); 
	strcpy(resultrow,"");
	while(row = mysql_fetch_row(rset)){   
		strcpy(memberName,row[1]);
		strcpy(memberID,row[2]);
		strcpy(enrollmentDate,row[3]);
		strcpy(memberRecommender,row[4]);
		strcpy(memberGender,row[5]);
		strcpy(enrolledBy,row[6]);
		strcpy(districtName,row[7]);
		strcpy(recommendedMembers,row[8]);
		sprintf(resultrow,"\n %s %s %s %s %s %s %s %s",memberName,memberID,enrollmentDate,memberRecommender,memberGender,enrolledBy,districtName,recommendedMembers);
		strcat(mainmemberob.messageStr,resultrow);
	}
	mysql_free_result(rset);  
	mysql_close(conn);
	
	return mainmemberob;
} 


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void bbzero(char * strg){ for(int i=0;i<sizeof(strg);i++){ strg[i]='\0'; } }
 


