

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "felli"
#define DATABASE "app"

/*
	Print out mysql error, exit program
*/

void finish_with_error(MYSQL * con);

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param selectQuery - conditions for selecting records
*/
void selectData(MYSQL * con, char * selectQuery);

/*
	Show result/records of the query
*/
void showResult(MYSQL * con);

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param dataQuery - the record information
*/
void insert(MYSQL * con, char * tableName, char * dataQuery);

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param columns - for creating the columns in the table, examaple: "id INT, name TEXT"
*/
void createTable(MYSQL * con, char * tableName, char * columns);


/*
	Connect to mysql and select database
*/
bool isMysqlConnected(MYSQL * con);

#include "mysqlFunctions.c"