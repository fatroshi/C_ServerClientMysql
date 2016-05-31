
/*
	Print out mysql error, exit program
*/

void finish_with_error(MYSQL * con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param selectQuery - conditions for selecting records
*/
void selectData(MYSQL * con, char * selectQuery){
	char query[100] = {0};
	//strcat(query, "SELECT * FROM ");
	//strcat(query, tableName);
	//strcat(query, " ");
	strcat(query, selectQuery);

  if (mysql_query(con, query)) 
  	{
      finish_with_error(con);
  	}
}

/*
	Show result/records of the query
*/
void showResult(MYSQL * con){
  MYSQL_RES * result = mysql_store_result(con);
  
  if (result == NULL) 
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  
  while ((row = mysql_fetch_row(result))) 
  { 
      for(int i = 0; i < num_fields; i++) 
      { 
          printf("%s ", row[i] ? row[i] : "NULL"); 
      } 
          printf("\n"); 
  }
  
  mysql_free_result(result);
}

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param dataQuery - the record information
*/
void insert(MYSQL * con, char * tableName, char * dataQuery){
	char query[100] = {0};
	strcat(query, "INSERT INTO ");
	strcat(query, tableName);
	strcat(query, " VALUES(");
	strcat(query, dataQuery);
	strcat(query, ")");

	if (mysql_query(con, query)) {
	  finish_with_error(con);
	}
}

/*
	Create table in the database
	@param con - reference to mysql
	@param tableName - name of the table we want to create
	@param columns - for creating the columns in the table, examaple: "id INT, name TEXT"
*/
void createTable(MYSQL * con, char * tableName, char * columns){
  
  // Drop table if exists
  char dropQuery[100] = {0};
  strcat(dropQuery, "DROP TABLE IF EXISTS ");
  strcat(dropQuery, tableName);

  if (mysql_query(con, dropQuery)) {
      finish_with_error(con);
  }

  // Create table
  char tableQuery[100] = {0};
  strcat(tableQuery, "CREATE TABLE ");
  strcat(tableQuery, tableName);
  strcat(tableQuery, "(");
  strcat(tableQuery, columns);
  strcat(tableQuery, ") ");

  // Perform query, create the table
  if (mysql_query(con, tableQuery)) {      
      finish_with_error(con);
  }
}

/*
	Connect to mysql and select database
*/
bool isMysqlConnected(MYSQL * con)
{
	bool connected = true;

	if (con == NULL) 
	{
	  fprintf(stderr, "%s\n", mysql_error(con));
	  connected = false;
	  exit(1);

	}  

	if (mysql_real_connect(con, HOST, USERNAME, PASSWORD, 
	      DATABASE, 0, NULL, 0) == NULL) 
	{
	  finish_with_error(con);
	  connected = false;
	} 

	return connected;	
}