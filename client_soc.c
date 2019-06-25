#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024
void bbzero(char * strg);
//======================================
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
char c,chara,shstr[2],commandList[1200],cmdd[5],agentUser[30],recommenderVar[30];
char memberIDvar[30],districtName[20],commandListCopy[1200],filenamee[30];
char filedata[1200];
char cmddpiece[30],cmddpiece2[30],cmddpiece3[30],sgniture[20];
int i; 
FILE *fileptr;
 
	void send_recv(int i, int sockfd){

		char send_buf[BUFSIZE];
		char recv_buf[BUFSIZE];
		int nbyte_recvd; 
		     char cc[3], msgvar[1200] ; 
		     int mbrno=0;   
		if (i != 0){ 
			//fgets(send_buf, BUFSIZE, stdin);
			
			/**********************************************/
			printf("\nWhat do you wana do ? : \t");
		     //scanf(commandList,commandList);
		     fgets(commandList,sizeof(commandList),stdin); 
	   				/*----------------------------------------*/ 
	   				sprintf(cmdd,"%c%c%c%c",commandList[0],commandList[1],commandList[2],commandList[3]);  
		            strcpy(memberob.agentDistrict,districtName); 
		            
		            if(strcmp(cmdd,"Addm")==0 || strcmp(cmdd,"addm")==0){
		             
		            	strcpy(commandListCopy,commandList);  
		                //if command addmember is followed by a file name, 
		                strcpy(cmddpiece , strtok(commandListCopy, " ")); 
		                strcpy(cmddpiece2 , strtok(NULL, " "));   
		                if(strcmp(cmddpiece2,"file_input.txt")==0){ 
		                	strcpy(filenamee,cmddpiece2); //filenamee = cmddpiece2; 
		                	//read new menbers from the file instead.
		                	fileptr = fopen(filenamee, "r");
		                	if(fileptr != NULL){
		                		//do the work
		                		strcpy(memberob.messageStr,"");
		                		strcat(memberob.messageStr,"adds "); 
		                		while((chara = fgetc(fileptr)) != EOF){
		                			if(chara=='\n'){chara=';';}
		                			sprintf(&shstr[0],"%c",chara);
		                			strcat(filedata,shstr); 
		                		} 
		                		strcat(memberob.messageStr,filedata);    
		                		//close the file
		                		fclose(fileptr);
		                		send(sockfd,&memberob,sizeof(memberob),0);  
		                	}else{  
		                		printf("File '%s' not found in the working directory !!",filenamee);
		                	}
				            //then: create logic for assigning member idz
		                }else{ 
				            //otherwise do the following.
				            printf("\nEnter the recommender ID or 'none' : \t");
				            fgets(recommenderVar,sizeof(recommenderVar),stdin); 
				            strcpy(memberob.memberRecommender,recommenderVar);
				        	sprintf(memberIDvar,"%c%c%c%d%d",districtName[0],districtName[1],districtName[2],0,mbrno); mbrno++;
				        	strcpy(memberob.memberID,memberIDvar);
						    strcpy(memberob.messageStr,commandList); 
						    send(sockfd,&memberob,sizeof(memberob),0); 
				        }//end of if else 
				        //Signing: prompting 15 times in a loop
				        printf("Provide your 15 character signiture to continue\n");
				        for(i=0;i<15;i++){ printf("\nsign %d : ",i); scanf("%c",&sgniture[i]);  } 
				     }else{
				     	strcpy(memberob.messageStr,commandList); 
				        send(sockfd,&memberob,sizeof(memberob),0);  
				     } 
		  		    /*----------------------------------------*/ 
			 }
			 
			nbyte_recvd = read(sockfd, &ret_structr, sizeof(ret_structr)); 
			//if(strcmp(ret_structr.agentUsername,agentUser)==0){ 
				printf("\nret_structr.agentUsername %s ,agentUser: %s",ret_structr.agentUsername,agentUser);  //print the response from the srver.
			//}
			fflush(stdout);
				  
		} //end of send_recv() function
			 
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(4950);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, ' ', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
}
 
	
int main(){

	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	
	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
	fdmax = sockfd;
	//prompt for the district
	printf("AVAILABLE CMMANDS : \n%s\n%s\n%s\n%s\n\n",
	"\tAddmember member_name, date, gender, recommender",
	"\tAddmember filename.txt  eg. 'Addmember file_input.txt '",  /*eg. Addmember file_input.txt      */
	"\tCheck_status",	  			/*To check status of the file*/
	"\tget_statement",  			/*To check statement of payments for the logged in user*/
	"\tSearch criteria eg. \"Search memberName='Reagan' \" or \"Search enrollmentDate='2019-06-21' \"" 			/*To search and view a record from the file by date or name */
	);
	printf("\nWHAT IS YOUR DISTRICT ? : \t"); //scanf("%s",districtName); 
		fgets(districtName,sizeof(districtName),stdin);
	printf("\nWHAT IS YOUR USERNAME ? : \t"); //scanf("%s",agentUser);
		fgets(agentUser,sizeof(agentUser),stdin); 
		strcpy(memberob.agentUsername,agentUser);
	
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){	perror("select"); exit(4);  }
		
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds)) 
				send_recv(i, sockfd);
	} //end of while(1)
	
	printf("client-quited\n");
	close(sockfd);
	
	
	return 0;
}  //end of main function

void bbzero(char *strg){ for(int i=0;i<sizeof(strg);i++){ strg[i]='\0'; } }








