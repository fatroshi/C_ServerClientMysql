/* 
	Created by Farhad Atroshi 
	2016-06-29
	
	Server in C (TCP)
		1. Create socket with socket().
		2. Bind the socket to specific address with bind().
		3. Listen to connections with listen().
		4. Accept connection with accept().
		5. Send/recieve data with read() and write().

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdbool.h>
#include <my_global.h>
#include <mysql.h>


/* Own lib */
#include "MySQL/mysqlFunctions.h"	



void error(char * msg){
	printf("Error: %s\n", msg);
	exit(1);	
} 

void sendMsg(int newsockfd, char * result){
	// Send message to the client
	int n = write(newsockfd, result, strlen(result));

	if(n < 0){
		error("writing to socket");
	}
}

int main(int argc, char  *argv[])
{
	
	// Connect to mysql
  	MYSQL * con = mysql_init(NULL);
  	// Check if connected to mysql and db has been selected
  	isMysqlConnected(con);


	int sockfd, newsockfd, portnumber,clientLength;
	// Buffer for sending/receiving
	char buffer[256];
	//
	struct sockaddr_in server_address, client_address;
	//
	int n;

	// Check if port number was given
	if(argc < 2){
		error("no port provided");
	}

    /*
        creates an UN-named socket inside the kernel and returns an integer known as socket descriptor.
        @param AF_INET      - Internet family of IPv4 addresses
        @param SOCK_STREAM  - This function takes domain/family as its first argument. For Internet family of IPv4 addresses we use AF_INET
        @param 0            - kernel decide the default protocol to use for this connection. 
                              For connection oriented reliable connections, the default protocol used is TCP, based on previous 2 arguments.
    */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Check if we could create socket
	if(sockfd < 0){
		error("opening socket");
	}

	// Clean up the struct
	bzero((char * ) &server_address, sizeof(server_address));

	// Set the portnumber
	portnumber = atoi(argv[1]);

	// Set values for the server
	server_address.sin_family 			= AF_INET;
	server_address.sin_addr.s_addr 		= INADDR_ANY;
	server_address.sin_port				= htons(portnumber);


    /*
        The call to the function ‘bind()’ assigns the details specified in the structure ‘serv_addr’ to 
        the socket created in the step above. The details include, the family/domain, the interface 
        to listen on(in case the system has multiple interfaces to network) and the port on which the 
        server will wait for the client requests to come.
    */

	if(bind(sockfd, (struct sockaddr * ) &server_address, sizeof(server_address)) < 0){
		error("on binding");
	}

    /*
        The call to the function ‘listen()’ with second argument as ’10’ specifies maximum number of client 
        connections that server will queue for this listening socket.
        After the call to listen(), this socket becomes a fully functional listening socket.
    */
	listen(sockfd,10);					

	// Get length
	clientLength = sizeof(client_address);

	// When the client has made a request, we have to accept it.
	// we create a new socket for that particular client
	newsockfd = accept(sockfd, (struct sockaddr *) &client_address, &clientLength);

	// Check if new socket was created for the client
	if(newsockfd < 0){
		error("on accept");
	}

	while(1){
		// Clear the buffer
		bzero(buffer, 256);
		// Store client information in buffer
		n = read(newsockfd, buffer, 256);

		if(n < 0){
			error("reading from socket");
		}

		// Print out the message from the client
		printf("Msg from client: %s\n", buffer);


		// Get data from the database
		selectData(con, "SELECT * FROM data");
		
		// Send result from the query
		MYSQL_RES * result = mysql_store_result(con);
		
		
		if (result == NULL) 
		{
		  finish_with_error(con);
		}

		int num_fields = mysql_num_fields(result);

		// Get data and send to client
		char rowData[250] = {0};
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(result))) 
		{ 
		  
		  for(int i = 0; i < num_fields; i++) 
		  { 
		  		if(row[i] != NULL){
		  			strcat(rowData,(char *)row[i]);
		  			strcat(rowData," ");
		  		}
		  		strcat(rowData,"\n");
		  		// Send to client
	  			sendMsg(newsockfd, rowData);
		  } 
		}
		mysql_free_result(result);
	}


	return 0;
}