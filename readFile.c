#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <my_global.h>
#include <mysql.h>
#include <stdbool.h>

/* Own lib */
#include "MySQL/mysqlFunctions.h"

#define MAX_CHAR 255
#define MAX_COLUMNS 6 

// I dont know what this fields are representing! 
// Sorry for the bad labels
#define COLUMN_1 0
#define COLUMN_2 1
#define COLUMN_3 2
#define COLUMN_4 3
#define COLUMN_5 4
#define COLUMN_6 5


/*
	Inser record in db
	@param con 		- Mysql connection reference
	@param Array 	- Array containing record info
	@param recordId	- the value that will be used as id for the record
*/
void insertInDb(MYSQL * con, char array[][50], int recordId){

	//insert(con, "Cars", "2,'Audi',123");
	char id[10];
	sprintf(id, "%d", recordId);

	// Create the query
	char query[255] = {0};
	strcat(query, id);
	strcat(query, ",'");
	strcat(query, array[COLUMN_1]);
	strcat(query, "','");
	strcat(query, array[COLUMN_2]);
	strcat(query, "','");
	strcat(query, array[COLUMN_3]);
	strcat(query, "','");
	strcat(query, array[COLUMN_4]);
	strcat(query, "',");
	strcat(query, array[COLUMN_5]);
	strcat(query, ",");
	strcat(query, array[COLUMN_6]);

	// Insert record in db
	insert(con, "data", query);
}


int main(int argc, char *argv[])

{
	// Connect to mysql
  	MYSQL * con = mysql_init(NULL);
  	// Check if connected to mysql and db has been selected
  	isMysqlConnected(con);

  	// Create table for the fields
  	createTable(con,"data", "id INT, c1 TEXT,  c2 TEXT, c3 TEXT, c4 TEXT, c5 TEXT, c6 TEXT");


	// Pointer to file
	FILE * ptr_file;

	// var
	char buffer[MAX_CHAR];
	int columCount 	= 0;
	int index 		= 0;
	int line 		= 0;
	int rowCounter	= 0;
	// Array used for storing column values
	char row[MAX_COLUMNS][50];

	// READ FILE
	ptr_file = fopen("data.csv", "r");

	while(fgets(buffer,MAX_CHAR, ptr_file)){
		// Skip first line
		if(line == 0){
			line++;
			continue;
		}

		// Increase rowCounter
		// This will be used as the ID for each new record, the best solution for a real project 
		//	would have been auto increment for the id column! 
		rowCounter++; 	

		for (int i = 0; i < strlen(buffer); ++i)
		{
			char c = buffer[i];
			// Check for tab value
			if(c == 9){
				columCount++;
				// Reset index
				index = 0;
				// Dont store
				continue;
			}

			// Store char
			row[columCount][index] = c;
			// End string
			row[columCount][index+1] = '\0';
			// Indrease index
			index++;

			if(i == strlen(buffer) - 1){
				// Reset columCount
				columCount = 0;
				// Reset index
				index = 0;
				// Store in db
				insertInDb(con, row, rowCounter);
			}
		}

		if(rowCounter == 1){
			//break;
		}
	}

	// Close filehandler
	fclose(ptr_file);

	return 0;
} 