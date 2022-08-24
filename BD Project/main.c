#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#define LOGIN "login"

static MYSQL *con;

typedef enum {
	ADMIN = 1,
	STANDARD,
	FAILED_LOGIN
}role_t;

role_t attempt_login(MYSQL *conn, char *cf, char *password){
	
	MYSQL_STMT *login_procedure;
	MYSQL_BIND param[3];
	
	int role = 0;
	
	if(!setup_prepared_stmt(&login_procedure, "call validazione_accesso(?, ?, ?)", conn)){
		print_stmt_error(login_procedure, "Unable to initialize login statement\n");
		goto err2;
	}
	
	//Prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;  //IN
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);
	
	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;  //IN
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);
	
	if(mysql_stmt_bind_param(login_procedure, param) != 0){
		print_stmt_error(login_procedure, "Could not bind parameters for login");
		goto err;
	}
	
	//Run procedure
	if(mysql_stmt_execute(login_procedure) != 0){
		print_stmt_error(login_procedure, "Could not execute login procedure");
		goto err;
	}
	
	//Prepare output parameters
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;  //OUT
	param[0].buffer = &role;
	param[0].buffer_length = sizeof(role);
	
	
	if(mysql_stmt_bind_result(login_procedure, param)){
		print_stmt_error(login_procedure, "Could not retrieve output parameters");
		goto err;
	}
	
	//Retrieve output parameter
	if(mysql_stmt_fetch(login_procedure)){
		print_stmt_error(login_procedure, "Could not buffer results");
		goto err;
	}
	
	mysql_stmt_close(login_procedure);
	return role;
	
	err:
	mysql_stmt_close(login_procedure);
	
	err2:
	return FAILED_LOGIN;
	
}

void main(int argc, char **argv){
	
	char operation[20];
	char cf[128];
	char password[128];
	
	top:
	
	printf("\033[2J\033[H");
	printf(".................................................................................\n");
	printf(".................................................................................\n");
	printf("...............__ __ __ __ __ __ ..................... _ __ __ _ ................\n");
	printf("..............|                 |.....................|         |................\n");
	printf("..............| LOGIN UTENTE: 1 |.....................| EXIT: 0 |................\n");
	printf("..............|_ __ __ __ __ __ |.....................|__ __ __ |................\n");
	printf(".................................................................................\n");
	printf(".................................................................................\n\n");
	printf("   Inserisci il codice dell'operazione : ");
	
	getInput(64, operation, false);
	
	if(!strcmp(operation, "0")){
    		printf(ANSI_COLOR_RED"\n   Exit...\n\n" ANSI_COLOR_RESET);
		return;
    	}
    	else if(!strcmp(operation, "1")){
    		
    		//Inizializzazione della prima connessione (phantom user), per connettersi al db -> tabella Login.
		con = mysql_init(NULL);
		if (con == NULL) {
			fprintf(stderr, "Initilization error: %s\n", mysql_error(con));
			exit(EXIT_FAILURE);
		}
			 
		if (mysql_real_connect(con, getJson(LOGIN, HOST), getJson(LOGIN, USERNAME), getJson(LOGIN, PASSWORD), getJson(LOGIN, DATABASE), atoi(getJson(LOGIN, PORT)), NULL,
			 CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS) == NULL){
			fprintf(stderr, "Connection error: %s\n", mysql_error(con));
			mysql_close(con);
			exit(1);
		}
	
		printf("\033[2J\033[H");
		printf("....................................................................................\n");
		printf("....................................................................................\n");
		printf("...............__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _..............\n");
		printf("..............|                                                       |.............\n");
		printf("..............|                     LOGIN UTENTE                      |.............\n");
		printf("..............|__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ _|.............\n");
		printf("....................................................................................\n");
		printf("....................................................................................\n\n");
		printf("                                CODICE FISCALE : ");
		getInput(128, cf, false);
		printf("\n\n                           	PASSWORD : ");
		getInput(128, password, true);
		
		int role = attempt_login(con, cf, password);
	
		switch(role){
			case 0:
				admin(con, cf);
				break;
			case 1:
				standard(con, cf);
				break;
			default:
				printf(ANSI_COLOR_RED"\n\n                           	Credenziali errate!\n\n\n" ANSI_COLOR_RESET);
				break;
		}
		
		mysql_close(con);
	}
	else{
    		printf(ANSI_COLOR_RED"\n   Comando non riconosciuto\n\n" ANSI_COLOR_RESET);
    	}
    	
    	goto top;
}


