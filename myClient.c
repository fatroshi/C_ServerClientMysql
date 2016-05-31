/* 
	Created by Farhad Atroshi 
	2016-06-29
	
	Client in C (TCP)
		1. Create socket with socket().
		2. Connect the socket to the address of the server, using connect().
		3. Send/recieve data with read() and write().

	source: source: http://www.tutorialspoint.com/unix_sockets/socket_client_example.htm

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>

/* GTK lib */
#include <gtk/gtk.h>


#define MAX_BUFFER 10000
#define MAX_ENTRIES 6

struct GUI {
  GtkWidget * window;
  GtkWidget * table;
  GtkWidget * label[8];
  GtkWidget * button;
  GtkWidget * entry[MAX_ENTRIES];
};

struct APP {
	int sockfd;
	int portnumber;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[MAX_BUFFER];
};

struct CLIENT {
	struct GUI gui;
	struct APP app;
};


/* This function is used for sending the sql query to the server */
void sendMsg(struct CLIENT * client, char * msg){
	/* Send message to the server */
	int n = write(client->app.sockfd, msg, strlen(msg));

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}
}


/* Get data from the server, this is the result of the query */
void getMsg(struct CLIENT * client){
	bzero(client->app.buffer,MAX_BUFFER);
	int n = read(client->app.sockfd, client->app.buffer, MAX_BUFFER);

	if (n < 0) {
		perror("ERROR reading from socket");
		exit(1);
	}

	printf("%s\n",client->app.buffer);
}


/*
	Check if next entry is empty based on current index
*/
bool isNextEntryEmpty(struct CLIENT * client, int currentIndex){
	bool empty = true;

	// Check so that its not the last element
	if((currentIndex + 1) < MAX_ENTRIES){

		for (int i = currentIndex + 1; i < MAX_ENTRIES; ++i){
			char * content = (char *) gtk_entry_get_text(GTK_ENTRY(client->gui.entry[i]));
			// Check if field is not empty
			if(strcmp(content, "") != 0){
				empty = false;
			}
		}
	}
	return empty;
}

/* Create query to send by analyzing the search columns */ 
void appendToQuery(char * column, char * content, char * query, struct CLIENT * client, int currentIndex){
	
	strcat(query, column);
	// Flag for checking if content has wildcard/s
	bool foundWildCard = false;
	
	// Replace character if needed
	for(int j=0; j < strlen(content); j++){
		if(content[j] == '*'){
			// Replace character
			content[j] = '%';
			// Set flag
			foundWildCard = true;
		}
	}

	// If wildcard exists use LIKE else =
	if(foundWildCard){
		strcat(query, " LIKE ");	
	}else{
		strcat(query, " = ");

	}

	// Add to query	
	strcat(query, content);

	// Add to query
	if(!isNextEntryEmpty(client,currentIndex)){
		if(foundWildCard){
			strcat(query, " OR ");
		}else{
			strcat(query, " AND ");
		}
	}
}

/*
	This functions is called when enter or the search buttons is pressed.
	It sends the entry content (sql query) to the server receives data from the server.

	@param widget 	- the widget is button in this case
	@param client 	- The client contains all graphic and program variables 
*/
static void button_clicked(GtkWidget * widget, struct CLIENT * client){
	g_print("%s\n", gtk_entry_get_text(GTK_ENTRY(client->gui.entry[0])));
	
	char query[1024] = {0};
	strcat(query, "SELECT * FROM data ");
	
	// Check if any column has content
	// -1 is so that it starts the loop from 0
	if(!isNextEntryEmpty(client, -1)){
		strcat(query, "WHERE ");
	}

	for (int i = 0; i < MAX_ENTRIES; ++i){
		// Store content of the entry
		char * content = (char *) gtk_entry_get_text(GTK_ENTRY(client->gui.entry[i]));
		// Check if field is not empty
		if(strcmp(content, "") != 0){
			// Check entry
			switch(i){
				case 0:
					// Store content of c1
					appendToQuery("c1", content, query, client, i);
					break;
				case 1:
					// Store content of c2
					appendToQuery("c2", content, query, client, i);
					break;
				case 2:
					// Store content of c3
					appendToQuery("c3", content, query, client, i);
					break;
				case 3:
					// Store content of c4
					appendToQuery("c4", content, query, client, i);
					break;
				case 4:
					// Store content of c5
					appendToQuery("c5", content, query, client, i);
					break;
				case 5:
					// Store content of c6
					appendToQuery("c6", content, query, client, i);
					break;
				default:
					break;
			}

		}
	}

	// Send query to server
	strcat(query, " \0");			// Finish of the string
	gtk_label_set_text((GtkLabel *)client->gui.label[7],query);
	sendMsg(client, query);
	// Get response
	getMsg(client);
}


/* init graphic */
void initGui(struct CLIENT * client){
	// create the window
	client->gui.window 		= gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// create button
	client->gui.button 		= gtk_button_new_with_mnemonic("_Search");	
	// Create label
	client->gui.label[0] 	= gtk_label_new("Search in the database:");
	client->gui.label[1] 	= gtk_label_new("c1 (date)"); 
	client->gui.label[2] 	= gtk_label_new("c2"); 
	client->gui.label[3] 	= gtk_label_new("c3"); 
	client->gui.label[4] 	= gtk_label_new("c4"); 
	client->gui.label[5] 	= gtk_label_new("c5"); 
	client->gui.label[6] 	= gtk_label_new("c6");
	client->gui.label[7] 	= gtk_label_new("");  
	// Create entry
	client->gui.entry[0] 	= gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(client->gui.entry[0]),"");
	client->gui.entry[1] 	= gtk_entry_new();
	client->gui.entry[2] 	= gtk_entry_new();
	client->gui.entry[3] 	= gtk_entry_new();
	client->gui.entry[4] 	= gtk_entry_new();
	client->gui.entry[5] 	= gtk_entry_new();

	// Create table
	client->gui.table 		= gtk_table_new(4,7,0);

	// Set window title
	gtk_window_set_title(GTK_WINDOW(client->gui.window),"Client");
	// Set window size
	gtk_widget_set_size_request(client->gui.window,300,300); 

	// Add label to the window (cointainer)
	gtk_container_add(GTK_CONTAINER(client->gui.window),client->gui.table);

	// Add objects to the table
	// x_start, x_end, y_start, y_end, FILL, FILL, PADDING, PADDING
	// Title
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[0],0,6,0,1, GTK_FILL,GTK_FILL,0,0);
	// Labels for columns
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[1],0,1,1,2, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[2],1,2,1,2, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[3],2,3,1,2, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[4],3,4,1,2, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[5],4,5,1,2, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[6],5,6,1,2, GTK_FILL,GTK_FILL,0,0);
	// Show the query
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.label[7],0,7,3,4, GTK_FILL,GTK_FILL,0,0);
	
	// Textbox
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[0],0,1,2,3, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[1],1,2,2,3, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[2],2,3,2,3, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[3],3,4,2,3, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[4],4,5,2,3, GTK_FILL,GTK_FILL,0,0);
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.entry[5],5,6,2,3, GTK_FILL,GTK_FILL,0,0);
	
	// Search btn
	gtk_table_attach(GTK_TABLE(client->gui.table),client->gui.button,6,7,2,3, GTK_FILL,GTK_FILL,0,0);


	// Set window to full size
	gtk_window_maximize(GTK_WINDOW(client->gui.window));

	// Make the window visible
	//gtk_widget_show(window);
	gtk_widget_show_all(client->gui.window);
}


/* Create listeners for button and entry c1 */
void createListeners(struct CLIENT * client){
	 /* Create a listener for the button 
	 @param button 			- what object to listen to
	 @param click			- Listening to this event
	 @param button_clicked	 - name of the macro, fuction that we want to call
	 @param what we want to send to the function! 
	*/ 	
	g_signal_connect(client->gui.button,"clicked",G_CALLBACK(button_clicked),(gpointer*)client);
	//g_signal_connect(client->gui.entry,"activate",G_CALLBACK(button_clicked),(gpointer*)client);

	/* Create X button so that the user can exit the program
	 @param window 			- what object to listen to
	 @param delete-event 	- Listening to this event
	 @param G_CALLBACK	 	- name of the macro, fuction that we want to call
	 @param what we want to send to the function! 
	*/ 
	 
	g_signal_connect(client->gui.window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
}


/* Check if hostname and port was given */
void isHostnamePortSet(int argc, char *argv[]){
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
}

/* Create socket */
void createSocked(struct CLIENT * client){
	client->app.sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (client->app.sockfd < 0) {
	  perror("ERROR opening socket");
	  exit(1);
	}
}


int main(int argc, char *argv[]) {

	/* Client GUI */
	// Init gtk
	gtk_init (&argc, &argv);

	// Struct APP
	struct CLIENT client;

	// init graphical elements
	initGui(&client);

	// Create listeners
	createListeners(&client);

	/* Client code */
	// Checl if we have 2 arguments
	isHostnamePortSet(argc,argv);

	// Get portnumber		
	client.app.portnumber = atoi(argv[2]);

	/* Create a socket point */
	createSocked(&client);


	// Set server values
	client.app.server = gethostbyname(argv[1]);

	if (client.app.server == NULL) {
	  fprintf(stderr,"ERROR, no such host\n");
	  exit(0);
	}

	bzero((char *) &client.app.serv_addr, sizeof(client.app.serv_addr));
	client.app.serv_addr.sin_family = AF_INET;
	bcopy((char *)client.app.server->h_addr, (char *)&client.app.serv_addr.sin_addr.s_addr, client.app.server->h_length);
	client.app.serv_addr.sin_port = htons(client.app.portnumber);

	/* Now connect to the server */
	if (connect(client.app.sockfd, (struct sockaddr*)&client.app.serv_addr, sizeof(client.app.serv_addr)) < 0) {
	  perror("ERROR connecting");
	  exit(1);
	}

	// Its a loop
	gtk_main();
	return 0;
}