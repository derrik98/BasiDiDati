#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "defines.h"

#define STANDARD "standard"

static UTENTE logged_user;

/* ----------------------------SEZIONE DEDICATA ALL'UTENTE BASE------------------------------------ */


//RICERCA ASTA PER NOME OGGETTO
void op_1(MYSQL *conn){
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];  // input parameter buffers
    
	top:
	    
	//titolo : operazione
	printf("\033[2J\033[H");
	printf("\n\n**** VISUALIZZAZIONE ASTE: RICERCA PER OGGETTO ****\n\n");

	//define input variable
	char nome[64];
	printf("Ricerca aste per nome: ");
	getInput(64, nome, false);
	printf("\n");

	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_aste_per_nome_oggetto(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
 	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].buffer_length = strlen(nome);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	}
	
	// Run the stored procedure	
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione dello stato dell'asta\n");
		return;
	}
	
	procedure_output(conn, stmt);
	mysql_stmt_close(stmt);
}

//VISUALIZZAZIONE ASTE ATTIVE
void op_2(MYSQL *conn){

	MYSQL_STMT *stmt;

	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** VISUALIZZAZIONE ASTE ATTIVE ****\n\n");

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_aste_attive()", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
 	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle aste attive\n");
		return;
	}
	
	procedure_output(conn, stmt);
    	mysql_stmt_close(stmt);
}


//VISUALIZZAZIONE STATO DI UN ASTA
void op_3(MYSQL *conn){

    	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
        
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** VISUALIZZAZIONE ASTE: VISUALIZZA LO STATO DI UN'ASTA ****\n\n");

	//define input variable
	int *id = malloc(sizeof(int));
	char* _id = malloc(sizeof(char)*64);
	printf("inserire il codice dell'asta: ");
	getInput(64, _id, false);
	printf("\n");
	*id =  atoi(_id);
	free(_id); 

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_stato_asta(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
 	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = id;
	ps_params[0].buffer_length = sizeof(int);
	
	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		goto top;
	}
	
	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle aste attive\n");
		free(id);
		return;
	}

	procedure_output(conn, stmt);
	free(id);
	mysql_stmt_close(stmt);

}

//VISUALIZZAZIONE OGGETTI AGGIUDICATI
void op_4(MYSQL *conn){

	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
    
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** VISUALIZZAZIONE OGGETTI AGGIUDICATI ****\n\n");

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_oggetti_aggiudicati(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
 	}
	
	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = strlen(logged_user.cf);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione degli oggetti aggiudicati\n");
		return;
	}
	
	procedure_output(conn, stmt);
	mysql_stmt_close(stmt);
}

//REGISTRAZIONE OFFERTA SU UN'ASTA
void op_5(MYSQL *conn){

    	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[4];  // input parameter buffers
    
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
   	printf("\n\n**** REGISTRAZIONE OFFERTA SU UN'ASTA ****\n\n");


	//define input variable
	char* _id = malloc(sizeof(char)*64);
	printf("Inserire il codice dell'asta: ");
	getInput(64, _id, false);
	int *id = malloc(sizeof(int));
	*id = (int) atoi(_id);
	free(_id);
	
	char* _offerta = malloc(sizeof(char)*64);
	printf("\nInserire l'importo dell'offerta: ");
	getInput(64, _offerta, false);
	float *offerta = malloc(sizeof(float));
	*offerta = (float) atof(_offerta);
	free(_offerta);

	char* _importo_controff = malloc(sizeof(char)*64);
	printf("\nInserire un importo massimo di controfferta: ");
	getInput(64, _importo_controff, false);
	float *importo_controff = malloc(sizeof(float));
	*importo_controff = (float) atof(_offerta);
	free(_importo_controff);

	//inizializzazione statement procedurale.	
	if(!setup_prepared_stmt(&stmt , "call registra_offerta(?, ?, ?, ?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		free(offerta);
		free(importo_controff);
		goto top;
 	}
	
	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = strlen(logged_user.cf);
	
	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = id;
	ps_params[1].buffer_length = sizeof(int);

	ps_params[2].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[2].buffer = offerta;
	ps_params[2].buffer_length = sizeof(float);
	
	ps_params[3].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[3].buffer = importo_controff;
	ps_params[3].buffer_length = sizeof(float);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(id);
		free(offerta);
		free(importo_controff);
		goto top;
	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella registrazione dell'offerta\n");
		free(id);
		free(offerta);
		free(importo_controff);
		return;
	}

	printf(ANSI_COLOR_GREEN"\nL'offerta è stata regolarmente registrata.\n" ANSI_COLOR_RESET);
	
	free(id);
	free(offerta);
	free(importo_controff);
	
	mysql_stmt_close(stmt);
}

//RICERCA ASTE PER ESPOSITORE
void op_6(MYSQL *conn){

	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
        
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** RICERCA ASTE PER ESPOSITORE ****\n\n");


	//define input variable
	char* cf_esp = malloc(sizeof(char)*16);
	printf("Inserire il codice fiscale dell'espositore: ");
	getInput(16, cf_esp, false);
	printf("\n");
	
    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_aste_per_espositore(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf_esp);
		goto top;
 	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = cf_esp;
	ps_params[0].buffer_length = strlen(cf_esp);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf_esp);
		goto top;
	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle aste per espositore\n");
		free(cf_esp);
		return;
	}

	procedure_output(conn, stmt);
	free(cf_esp);
	mysql_stmt_close(stmt);
	
}

//RICERCA ASTE PER CATEGORIA
void op_7(MYSQL *conn){

	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
        
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** VISUALIZZAZIONE ASTE: RICERCA PER CATEGORIA ****\n\n");

	//define input variable
	char* categoria = malloc(sizeof(char)*64);
	printf("inserire il nome di una categoria esistente (o parte di esso): ");
	getInput(64, categoria, false);
	printf("\n");

    	//inizializzazione statement procedurale.	
	if(!setup_prepared_stmt(&stmt , "call visualizza_aste_per_categoria(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(categoria);
		goto top;
 	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = categoria;
	ps_params[0].buffer_length = strlen(categoria);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(categoria);
		goto top;
	}
	
	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle aste per categoria\n");
		free(categoria);
		return;
	}

    	procedure_output(conn, stmt);
	free(categoria);
	mysql_stmt_close(stmt);
}

//RICERCA ASTE PER PARTECIPAZIONE
void op_9(MYSQL *conn){

	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
        
	top:
	
   	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** RICERCA ASTE PER PARTECIPAZIONE ****\n\n");

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt , "call visualizza_partecipazione_aste(?)", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
 	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = logged_user.cf;
	ps_params[0].buffer_length = strlen(logged_user.cf);
	
	//printf("Utente %s\n", logged_user.cf);
	

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle partecipazioni alle aste\n");
		return;
	}
	
	procedure_output(conn, stmt);
	mysql_stmt_close(stmt);
}


//AGGIUNGI UTENTE BASE
void op_10(MYSQL *conn){

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[10];  // input parameter buffers
	
	MYSQL_TIME *date, *scad_carta;
	date = malloc(sizeof(MYSQL_TIME));
	scad_carta = malloc(sizeof(MYSQL_TIME));
	
	top:
	
	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** AGGIUNGI UTENTE BASE ****\n\n");
      	
      	//define input variable
	char *cf = malloc(sizeof(char)*16);
	printf("\nCodice fiscale : ");
	getInput(16, cf, false);
	
	char *nome = malloc(sizeof(char)*32);
	printf("\nNome utente : ");
	getInput(32, nome, false);

	char *cognome = malloc(sizeof(char)*32);
	printf("\nCognome : ");
	getInput(32, cognome, false);
		
	char *pswd = malloc(sizeof(char)*32);
	printf("\nPassword di accesso : ");
	getInput(32, pswd, false);
	
	char *_anno = malloc(sizeof(char)*32);
	printf("\nAnno di nascita : ");
	getInput(32, _anno, false);
	date->year = atoi(_anno);
	free(_anno);

	char *_mese = malloc(sizeof(char)*32);
	printf("\nMese di nascita : ");
	getInput(32, _mese, false);
	date->month = atoi(_mese);
	free(_mese);

	char *_giorno = malloc(sizeof(char)*32);
	printf("\nGiorno di nascita : ");
	getInput(32, _giorno, false);
	date->day = atoi(_giorno);
	free(_giorno);

	//length[4] = sizeof(MYSQL_TIME);

	//-------------------    

	char *birthplace = malloc(sizeof(char)*32);
	printf("\nLuogo di nascita : ");
	getInput(32, birthplace, false);

	char *numcarta = malloc(sizeof(char)*32);
	printf("\nNumero di carta : ");
	getInput(32, numcarta, false);

	//scadenza carta suddivisa in 2 sezioni!!!!--

	char *_scadanno = malloc(sizeof(char)*32);
	printf("\nAnno di scadenza : ");
	getInput(32, _scadanno, false);
	scad_carta->year = atoi(_scadanno);
	free(_scadanno);

	char *_scadmese = malloc(sizeof(char)*32);
	printf("\nMese di scadenza : ");
	getInput(32, _scadmese, false);
	scad_carta->month = atoi(_scadmese);
	free(_scadmese);

	//length[7] = sizeof(MYSQL_TIME);

	//-------------------

	char *cvv = malloc(sizeof(char)*10);
	printf("\nCvv : ");
	getInput(10, cvv, false);

	char *indirizzo = malloc(sizeof(char)*32);
	printf("\nIndirizzo di consegna : ");
	getInput(32, indirizzo, false);
    	
    	//inizializzazione statement procedurale.
    	if(!setup_prepared_stmt(&stmt, "call registrazione_utente_base(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		//free(type);
		free(cf);
		free(nome);
		free(cognome);
		free(pswd); 
		free(birthplace);
		free(numcarta);
		free(cvv);
		free(indirizzo);
		free(date);
		free(scad_carta);
		goto top;
	}

	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));
	
	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = cf;
	ps_params[0].buffer_length = strlen(cf);

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = nome;
	ps_params[1].buffer_length = strlen(nome);

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = cognome;
	ps_params[2].buffer_length = strlen(cognome);

	ps_params[3].buffer_type = MYSQL_TYPE_DATE;
	ps_params[3].buffer = date;
	ps_params[3].buffer_length = sizeof(date);

	ps_params[4].buffer_type =  MYSQL_TYPE_VAR_STRING;
	ps_params[4].buffer = birthplace;
	ps_params[4].buffer_length = strlen(birthplace);

	ps_params[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[5].buffer = numcarta;
	ps_params[5].buffer_length = strlen(numcarta);

	ps_params[6].buffer_type = MYSQL_TYPE_DATE;
	ps_params[6].buffer = scad_carta;
	ps_params[6].buffer_length = sizeof(scad_carta);

	ps_params[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[7].buffer = cvv;
	ps_params[7].buffer_length = strlen(cvv);

	ps_params[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[8].buffer = indirizzo;
	ps_params[8].buffer_length = strlen(indirizzo);
			
	ps_params[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[9].buffer = pswd;
	ps_params[9].buffer_length = strlen(pswd);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(cf);
		free(nome);
		free(cognome);
		free(pswd); 
		free(birthplace);
		free(numcarta);
		free(cvv);
		free(indirizzo);
		free(date);
		free(scad_carta);
		goto top;
	}
	
	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella registrazione dell'utente Base.\n");
		free(cf);
		free(nome);
		free(cognome);
		free(pswd); 
		free(birthplace);
		free(numcarta);
		free(cvv);
		free(indirizzo);
		free(date);
		free(scad_carta);
		return;
	}
	
	printf(ANSI_COLOR_GREEN"\nUtente Base registrato\n" ANSI_COLOR_RESET);
	
	free(cf);
	free(nome);
	free(cognome);
	free(pswd); 
	free(birthplace);
	free(numcarta);
	free(cvv);
	free(indirizzo);
	free(date);
	free(scad_carta);
	
	mysql_stmt_close(stmt);
	
}

void standard(MYSQL *conn, char *cf){

	char operation[20];
	
	if(mysql_change_user(conn, getJson(STANDARD, USERNAME), getJson(STANDARD, PASSWORD), getJson(STANDARD, DATABASE))){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
	
	memcpy(logged_user.cf, cf, 64);

    	start: 
    	
    	printf("\033[2J\033[H");
    	printf("Connected as");
    	printf(ANSI_COLOR_BOLD" %s"ANSI_COLOR_OFF , cf);
    	printf("-standard\n\n");
	
    	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("..............................|      MENU UTENTE      |.............................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n\n");

	printf(" _____ ________ ________ _____Operazioni Disponibili_____ ________ ________ _______ _\n");
	printf("|                                                                                    |\n");
	printf("|                                                                    [LOG OUT: 0]    |\n");
	printf("|                                                                                    |\n");
    	printf("|   OPERAZIONE 1  : Ricerca aste per nome dell' oggetto                              |\n");
    	printf("|   OPERAZIONE 2  : Ricerca aste attive                                              |\n");
	printf("|   OPERAZIONE 3  : Visualizzazione dello stato di un'asta                           |\n");
  	printf("|   OPERAZIONE 4  : Visualizzazione oggetti aggiudicati                              |\n");
	printf("|   OPERAZIONE 5  : Registrazione offerta per un'asta                                |\n");
	printf("|   OPERAZIONE 6  : Ricerca aste per espositore                                      |\n");
	printf("|   OPERAZIONE 7  : Ricerca aste attive per categoria di afferenza                   |\n");
	printf("|   OPERAZIONE 8  : Visualizzazione categorie                                        |\n");
	printf("|   OPERAZIONE 9  : Visualizzazione delle aste a cui ho partecipato                  |\n");
	printf("|   OPERAZIONE 10 : Aggiungi utente Base              				     |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______ |\n\n");
    	printf("   Inserisci il codice dell'operazione : ");
   	
   	getInput(64, operation, false);
    	
    	if(!strcmp(operation, "0")){
    		printf(ANSI_COLOR_RED"\n   Logout...\n\n" ANSI_COLOR_RESET);
    		printf("\n\n Premi invio per tornare al Menù Login...\n");
        	while(getchar() != '\n'){}
		return;
    	}
    	else if(!strcmp(operation, "1")){
    		op_1(conn);
    	}
    	else if(!strcmp(operation, "2")){
    		op_2(conn);
    	}
    	else if(!strcmp(operation, "3")){
    		op_3(conn);
    	}
    	else if(!strcmp(operation, "4")){
    		op_4(conn);
    	}
    	else if(!strcmp(operation, "5")){
    		op_5(conn);
    	}
    	else if(!strcmp(operation, "6")){
    		op_6(conn);
    	}
    	else if(!strcmp(operation, "7")){
    		op_7(conn);
    	}
    	else if(!strcmp(operation, "8")){
    		op_a1(conn);
    	}
    	else if(!strcmp(operation, "9")){
    		op_9(conn);
    	}
    	else if(!strcmp(operation, "10")){
    		op_10(conn);
    	}
    	else{
    		printf(ANSI_COLOR_RED"\n   Comando non riconosciuto" ANSI_COLOR_RESET);
    	}
    	
    	printf("\n\n Premi invio per tornare al Menù Utente...\n");
        while(getchar() != '\n'){}
        goto start;
        
}

