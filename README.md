# Machine: Ubuntu 16.04 LTS

# What to install:
	MySQL:
		1: mysql-server (sudo apt-get install mysql-server -y)
		2: sudo apt install libmysqlclient-dev
		3: Login and create database:
			3.1 mysql -u root -p 	(username=root, password=felli)
			3.2 CREATE DATABASE app;

	Graphic lib.
		1: sudo apt-get install gnome-devel
		2: sudo apt-get install build-essential 
		3: sudo apt-get install libgtk2.0-dev
		4: sudo apt-get install libgtk2.0-doc devhelp
		
# Compile steps


1: readFile.c
	
	gcc readFile.c -o readFile  `mysql_config --cflags --libs` && ./readFile
	
2: myServer.c

	gcc myServer.c -o myServer `mysql_config --cflags --libs` && ./myServer 5000
	
3: myClient.c

	gcc myClient.c -o myClient `pkg-config --cflags --libs gtk+-2.0` && ./myClient localhost 5000
	

Result:
0. All data is read by ReadFile and stored in a database.
1. client sends a query to the server
2. The server sends over the query result to the client.
	But the result is shown in the termianl. 
	the main fokus has been on following files:
		- readFile
		- mysqlFunctions
		- client
		
