#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "defines.h"

#define ADMIN "admin"


char command[20];
static UTENTE logged_user;

/* -----------------------SEZIONE DEDICATA ALL'UTENTE AMMINISTRATORE---------------------------- */

/*                              *** Gestione categorie ***                                        */

//VISUALIZZA TITOLARIO GERARCHICO
void op_a1(MYSQL *conn){

	MYSQL_STMT *stmt;
	int status;
    
    	top:
    	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n    **** VISUALIZZAZIONE CATEGORIE : TITOLARIO GERARCHICO ****\n\n");

	//inizializzazione statement procedurale.
    	if(!setup_prepared_stmt(&stmt , "call visualizzazione_titolario_gerarchico()", conn)){
    		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		goto top;
    	}
    
    	// Run the stored procedure
    	if(mysql_stmt_execute(stmt) != 0){
    		print_stmt_error(stmt, "Errore nella visualizzazione del titolario gerarchico.\n");
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		return;
    	}
    
   	procedure_output(conn, stmt);
    	mysql_stmt_close(stmt);
    
}


//INSERIMENTO CATEGORIA
void op_a2(MYSQL *conn){

	MYSQL_STMT *stmt;	
    
	top:
	
	printf("\033[2J\033[H");
    	printf("\n\n           **** INSERIMENTO CATEGORIA ****\n\n");
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n");
	printf("|                                                              |\n");
	printf("| In questa sezione è possibile inserire nuove categorie       |\n");
	printf("| nel titolario corrente. Il titolario è strutturato in        |\n");
	printf("| una gerarchia a tre livelli:                                 |\n");
	printf("|                                                              |\n");
	printf("| Lv 1 : categorie genitori di categorie di Lv (1)             |\n");
	printf("| Lv 2 : categorie genitori di categorie di Lv (2)             |\n");
	printf("| Lv 3 : categorie effettivamente associabili ad oggetti Lv(3) |\n");
	printf("|                                                              |\n");
	printf("| E' possibile inserire una nuova categoria in uno             |\n");
	printf("| qualsiasi di questi livelli, indicando il suo nome           |\n");
	printf("| e quello della categoria genitore, se esistente.             |\n");
	printf("| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| \n");
	printf("\nDigitare il livello del titolario in cui si desidera inserire la nuova categoria.\n");
	printf("\nAltrimenti, digitare il codice '0' per visionare il titolario corrente... ");

	char *_lv = malloc(sizeof(char) * 32);
	int lv;
	getInput(32, _lv, false);
	lv = atoi(_lv);
	free(_lv);

	int answer;

	switch(lv){
		
		case 0:
			op_a1(conn);
            		printf("\n\n Premi invio per tornare alla scelta del livello...\n");
            		while(getchar() != '\n'){}
				printf("\033[2J\033[H");
            		goto top;
            		break;

		case 1:

			printf("\033[2J\033[H");
			printf("INSERIMENTO CATEGORIA: LIVELLO 1 \n");

			MYSQL_BIND ps_params[1];  // input parameter buffers
			
			//define input variable
			char nome[64];
			printf("\nInserire il nome della nuova categoria: ");
			getInput(64, nome, false);

			//inizializzazione statement procedurale.
			if(!setup_prepared_stmt(&stmt, "call inserimento_categoria1(?)", conn)) {
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
				print_stmt_error(stmt, "\nErrore nell'inserimento della categoria di livello 1.\n");
				return;
			}

			printf(ANSI_COLOR_GREEN "\nCategoria inserita correttamente\n" ANSI_COLOR_RESET);	
			break;
		
		
		case 2:

			printf("\033[2J\033[H");
			printf("INSERIMENTO CATEGORIA: LIVELLO 2 \n");

			printf("Si desidera associare la categoria a un genitore al livello 1 ?\n");
			printf("Digitare il codice di risposta [ 0 = sì  |  1 = no ] : ");

			char* _answer2 = malloc(sizeof(char)*32);
			getInput(32, _answer2, false);
			answer = atoi(_answer2);
			free(_answer2);

			if(answer == 0){

				MYSQL_BIND ps_params[2];  // input parameter buffers
    				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);

				char nomePadre[64];
				printf("\nInserire il nome della categoria genitore: ");
				getInput(64, nomePadre, false);


				//inizializzazione statement procedurale.
				if(!setup_prepared_stmt(&stmt, "call inserimento_categoria2(?, ?)", conn)) {
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
				
				ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[1].buffer = nomePadre;
				ps_params[1].buffer_length = strlen(nomePadre);

				// bind input parameters
				if(mysql_stmt_bind_param(stmt, ps_params) != 0){
					finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
				
				// Run the stored procedure
				if(mysql_stmt_execute(stmt) != 0){
					print_stmt_error(stmt, "\nErrore nell'inserimento della categoria di livello 2.\n");
					return;
				}
				
				printf(ANSI_COLOR_GREEN "\nCategoria inserita correttamente\n" ANSI_COLOR_RESET);
				
			} else{

				MYSQL_BIND ps_params[1];  // input parameter buffers
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);


				//inizializzazione statement procedurale.
				if(!setup_prepared_stmt(&stmt, "call inserimento_categoria2(?, ?)", conn)) {
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
					print_stmt_error(stmt, "\nErrore nell'inserimento della categoria di livello 2.\n");
					return;
				}
				
				printf(ANSI_COLOR_GREEN "\nCategoria inserita correttamente\n" ANSI_COLOR_RESET);
				
			}
			
			break;

		case 3:
			printf("\033[2J\033[H");
			printf("INSERIMENTO CATEGORIA: LIVELLO 3 \n");

			printf("Si desidera associare la categoria a un genitore al livello 2 ?\n");
			printf("Digitare il codice di risposta [ 0 = sì  |  1 = no ] : ");

			char* _answer = malloc(sizeof(char)*32);
			getInput(32, _answer, false);
			answer = atoi(_answer);
			free(_answer);

			if(answer == 0){

				MYSQL_BIND ps_params[2];	// input parameter buffers
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);

				char nomePadre[64];
				printf("\nInserire il nome della categoria genitore: ");
				getInput(64, nomePadre, false);


				//inizializzazione statement procedurale.
				if(!setup_prepared_stmt(&stmt, "call inserimento_categoria3(?, ?)", conn)) {
					finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
    					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}


				// prepare parameters
				memset(ps_params, 0, sizeof(ps_params));

				ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[0].buffer = nome;
				ps_params[0].buffer_length = strlen(nome);
				
				ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
				ps_params[1].buffer = nomePadre;
				ps_params[1].buffer_length = strlen(nomePadre);
				
				// bind input parameters
				if(mysql_stmt_bind_param(stmt, ps_params) != 0){
					finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
					printf("\nPremi invio per continuare...\n");
					while(getchar()!= '\n'){}
					goto top;
				}
				
				// Run the stored procedure
				if(mysql_stmt_execute(stmt) != 0){
					print_stmt_error(stmt, "\nErrore nell'inserimento della categoria di livello 3.\n");
					return;
				}
				
				printf(ANSI_COLOR_GREEN "\nCategoria inserita correttamente\n" ANSI_COLOR_RESET);
				
			} else{

				MYSQL_BIND ps_params[1];  // input parameter buffers
				
				//define input variable
				char nome[64];
				printf("\nInserire il nome della nuova categoria: ");
				getInput(64, nome, false);


				//inizializzazione statement procedurale.
				if(!setup_prepared_stmt(&stmt, "call inserimento_categoria3(?, ?)", conn)) {
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
					print_stmt_error(stmt, "\nErrore nell'inserimento della categoria di livello 3.\n");
					return;
				}
				
				printf(ANSI_COLOR_GREEN "\nCategoria inserita correttamente\n" ANSI_COLOR_RESET);
				
			}
			
			break;

		
		default:
			goto top;
			break;
	}
	
	mysql_stmt_close(stmt);
}


//AGGIORNAMENTO CATEGORIA
void op_a3(MYSQL *conn){

	MYSQL_STMT *stmt;
    
	top:
	
	printf("\033[2J\033[H");
    	printf("\n\n           **** AGGIORNAMENTO CATEGORIA ****\n\n");
	printf(" _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n");
	printf("|                                                        |\n");
	printf("| In questa sezione è possibile modificare il nome di    |\n");
	printf("| una categoria e/o associarla ad una categoria genitore,|\n");
	printf("| scelta tra quelle esistenti al livello superiore del   |\n");
	printf("| titolario. Si scelga la tipologia di aggiornamento     |\n");
	printf("| tra le seguenti:                                       |\n");
	printf("|                                                        |\n");
	printf("| 1. Modifica del nome di una categoria                  |\n");
	printf("| 2. Associa un padre ad una categoria (orfana)          |\n");
	printf("| 3. Cambia padre ad una categoria                       |\n");
	printf("| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|\n");
	printf("\nDigitare il codice relativo alla tipologia di aggiornamento desiderata.\n");
	printf("\nAltrimenti, digitare il codice '0' per visionare il titolario corrente... ");

	char *_cod = malloc(sizeof(char) * 32);
	int cod;
	getInput(32, _cod, false);
	cod = atoi(_cod);
	free(_cod);

	MYSQL_BIND ps_params[3];  // input parameter buffers
	char nome[64];
	char nuovo_nome[64];
	char nomePadre[64];
	char new_nomePadre[64];

	switch(cod){

		case 0:
			op_a1(conn);
            		printf("\n\n Premi invio per tornare alla scelta del livello...\n");
            		while(getchar() != '\n'){}
			printf("\033[2J\033[H");
            		goto top;
            		break;

		case 1:

			//define input variable
			printf("\nInserire il nome attuale della categoria : ");
			getInput(64, nome, false);

			printf("\nInserire il nuovo nome : ");
			getInput(64, nuovo_nome, false);
			
			//inizializzazione statement procedurale.
			if(!setup_prepared_stmt(&stmt, "call aggiornamento_categoria(?, ?, null, null)", conn)) {
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
			
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = nuovo_nome;
			ps_params[1].buffer_length = strlen(nuovo_nome);

			// bind input parameters
			if(mysql_stmt_bind_param(stmt, ps_params) != 0){
				finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			
			// Run the stored procedure
			if(mysql_stmt_execute(stmt) != 0){
				print_stmt_error(stmt, "\nErrore nell'aggiornamento della categoria.\n");
				return;
			}
			
			printf(ANSI_COLOR_GREEN "\nNome modificato correttamente\n" ANSI_COLOR_RESET);
			break;

		case 2:
			//define input variable
			printf("\nInserire il nome della categoria : ");
			getInput(64, nome, false);

			printf("\nInserire il nome della categoria che si vuole rendere padre : ");
			getInput(64, nomePadre, false);
			
			//inizializzazione statement procedurale.
			if(!setup_prepared_stmt(&stmt, "call aggiornamento_categoria(?, null, null, ?)", conn)) {
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
			
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = nomePadre;
			ps_params[1].buffer_length = strlen(nomePadre);

			// bind input parameters
			if(mysql_stmt_bind_param(stmt, ps_params) != 0){
				finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			
			// Run the stored procedure
			if(mysql_stmt_execute(stmt) != 0){
				print_stmt_error(stmt, "\nErrore nell'associazione della categoria.\n");
				return;
			}
			
			printf(ANSI_COLOR_GREEN "\nAssociazione completata\n" ANSI_COLOR_RESET);
			break;
	
		case 3:

			//define input variable
			printf("\nInserire il nome della categoria : ");
			getInput(64, nome, false);
			
			printf("\nInserire il nome della attuale categoria padre : ");
			getInput(64, nomePadre, false);
			
			printf("\nInserire il nome della categoria che si vuole rendere padre : ");
			getInput(64, new_nomePadre, false);
			
			//inizializzazione statement procedurale.
			if(!setup_prepared_stmt(&stmt, "call aggiornamento_categoria(?, null, ?, ?)", conn)) {
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
		
			ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[1].buffer = nomePadre;
			ps_params[1].buffer_length = strlen(nomePadre);

			ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
			ps_params[2].buffer = new_nomePadre;
			ps_params[2].buffer_length = strlen(new_nomePadre);

			// bind input parameters
			if(mysql_stmt_bind_param(stmt, ps_params) != 0){
				finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
				printf("\nPremi invio per continuare...\n");
				while(getchar()!= '\n'){}
				goto top;
			}
			
			// Run the stored procedure
			if(mysql_stmt_execute(stmt) != 0){
				print_stmt_error(stmt, "\nErrore nell'associazione della categoria.\n");
				return;
			}
			
			printf(ANSI_COLOR_GREEN "\nAssociazione completata\n" ANSI_COLOR_RESET);
			break;

		default:
			goto top;
			break;
	}
	
	mysql_stmt_close(stmt);
}



//CANCELLAZIONE CATEGORIA ****
void op_a4(MYSQL *conn){

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];  // input parameter buffers
	
	//define input variable
	char nome[64];
	
	top:
	
	printf("\033[2J\033[H");
	printf("\nInserire il nome della categoria da eliminare: ");
	
	getInput(64, nome, false);

	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt, "call cancellazione_categoria(?)", conn)) {
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
		print_stmt_error(stmt, "\nErrore nella calcellazione della categoria.\n");
		return;
	}
	
	printf(ANSI_COLOR_GREEN "Cancellazione effettuata" ANSI_COLOR_RESET);	
	mysql_stmt_close(stmt);
}

//INIZIALIZZAZIONE NUOVA ASTA
void op_a5(MYSQL *conn){

	MYSQL_STMT *stmt;
 	MYSQL_BIND ps_params[9];  // input parameter buffers
    
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** INIZIALIZZAZIONE NUOVA ASTA ****\n\n");

	//define input variable
	char* nome = malloc(sizeof(char)*64);
	printf("inserire il nome dell'oggetto all'asta: ");
	getInput(64, nome, false);

	char* cat = malloc(sizeof(char)*64);
	printf("\ninserire la categoria di afferenza dell'oggetto: ");
	getInput(64, cat, false);

	char* desc = malloc(sizeof(char)*64);
	printf("\ninserire una breve descrizione dell' oggetto: ");
	getInput(64, desc, false);

	char* stato = malloc(sizeof(char)*64);
	printf("\ninserire lo stato attuale dell'oggetto (nuovo/usato/buone condizioni/etc...): ");
	getInput(64, stato, false);

	char* colore = malloc(sizeof(char)*64);
	printf("\ninserire il colore dell'oggetto: ");
	getInput(64, colore, false);

	char* dimens = malloc(sizeof(char)*64);
	printf("\ninserire le dimensioni dell'oggetto: ");
	getInput(64, dimens, false);
	
	char* _prezzobase = malloc(sizeof(char)*64);
	printf("\ninserire il prezzo di partenza per l'oggetto all'asta: ");
	getInput(64, _prezzobase, false);
	float *prezzobase = malloc(sizeof(float));
	*prezzobase = (float) atof(_prezzobase);
	free(_prezzobase);

	char* _durata = malloc(sizeof(char)*64);
	printf("\ninserire la durata, in giorni, dell'asta (il range da considerare è [1, 7] ) : ");
	getInput(64, _durata, false);
	int *durata = malloc(sizeof(int));
	*durata = (int) atoi(_durata);
	free(_durata);

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt, "call inizializzazione_asta(?, ?, ?, ?, ?, ?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		goto top;
	}
    
	// initialize parameters
	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].buffer_length = strlen(nome);

	ps_params[1].buffer_type = MYSQL_TYPE_STRING;
	ps_params[1].buffer = cat;
	ps_params[1].buffer_length = strlen(cat);

	ps_params[2].buffer_type = MYSQL_TYPE_STRING;
	ps_params[2].buffer = desc;
	ps_params[2].buffer_length = strlen(desc);

	ps_params[3].buffer_type = MYSQL_TYPE_STRING;
	ps_params[3].buffer = stato;
	ps_params[3].buffer_length = strlen(stato);

	ps_params[4].buffer_type = MYSQL_TYPE_STRING;
	ps_params[4].buffer = colore;
	ps_params[4].buffer_length = strlen(colore);

	ps_params[5].buffer_type = MYSQL_TYPE_STRING;
	ps_params[5].buffer = logged_user.cf;
	ps_params[5].buffer_length = strlen(logged_user.cf);

	ps_params[6].buffer_type = MYSQL_TYPE_STRING;
	ps_params[6].buffer = dimens;
	ps_params[6].buffer_length = strlen(dimens);

	ps_params[7].buffer_type = MYSQL_TYPE_FLOAT;
	ps_params[7].buffer = prezzobase;
	ps_params[7].buffer_length = sizeof(float);

	ps_params[8].buffer_type = MYSQL_TYPE_LONG;
	ps_params[8].buffer = durata;
	ps_params[8].buffer_length = sizeof(int);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		goto top;
	}

	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nell'inserimento dell'oggetto.\n");
		free(nome);
		free(cat);
		free(desc);
		free(stato);
		free(colore);
		free(dimens);
		free(prezzobase);
		free(durata);
		return;
	}
	
	printf(ANSI_COLOR_GREEN  "\nOggetto inserito correttamente" ANSI_COLOR_RESET);
	
	free(nome);
	free(cat);
	free(desc);
	free(stato);
	free(colore);
	free(dimens);
	free(prezzobase);
	free(durata);
	
	mysql_stmt_close(stmt);
}

//REPORT ASTA
void op_a6(MYSQL *conn){

    	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
   
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** REPORT ASTA ****\n\n");

	//define input variable
	int *id = malloc(sizeof(int));
	char* _id = malloc(sizeof(char)*64);
	printf("Inserire il codice dell'asta: ");
	getInput(64, _id, false);
	*id =  atoi(_id);
	free(_id); 
	printf("\n");

  	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt, "call report_asta(?)", conn)) {
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
		print_stmt_error(stmt, "\nErrore nell'inserimento dell'oggetto.\n");
		free(id);
		return;
	}
	
    	procedure_output(conn, stmt);
	free(id);
	mysql_stmt_close(stmt);
}


//VISUALIZZAZIONE ASTE INDETTE
void op_a7(MYSQL *conn){

	MYSQL_STMT *stmt;
    	MYSQL_BIND ps_params[1];  // input parameter buffers
    
	top:
	
    	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** VISUALIZZAZIONE ASTE INDETTE ****\n\n\n");

    	//inizializzazione statement procedurale.
	if(!setup_prepared_stmt(&stmt, "call visualizza_aste_per_espositore(?)", conn)) {
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
		print_stmt_error(stmt, "\nErrore nella visualizzazione delle aste indette.\n");
		return;
	}
	
	procedure_output(conn, stmt);
	mysql_stmt_close(stmt);
}

//AGGIUNGI UTENTE AMMINISTRATORE
void op_a9(MYSQL *conn){

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[4];  // input parameter buffers
	
	top:
	
	//titolo : operazione
	printf("\033[2J\033[H");
    	printf("\n\n**** AGGIUNGI UTENTE AMMINISTRATORE ****\n\n");
      	
    	//define input variable
	char *cf = malloc(sizeof(char)*32);
	printf("\nCodice fiscale : ");
	getInput(32, cf, false);
	
	char *nome = malloc(sizeof(char)*32);
	printf("\nNome utente : ");
	getInput(32, nome, false);

	char *cognome = malloc(sizeof(char)*32);
	printf("\nCognome : ");
	getInput(32, cognome, false);
		
	char *pswd = malloc(sizeof(char)*32);
	printf("\nPassword di accesso : ");
	getInput(32, pswd, false);
	printf("\n");
    	
    	//inizializzazione statement procedurale.
    	if(!setup_prepared_stmt(&stmt, "call registrazione_utente_amm(?, ?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, stmt, "Impossibile inizializzare lo statement.\n", false);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		//free(type);
		free(cf);
		free(nome);
		free(cognome);
		free(pswd);
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
			
	ps_params[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[3].buffer = pswd;
	ps_params[3].buffer_length = strlen(pswd);

	// bind input parameters
	if(mysql_stmt_bind_param(stmt, ps_params) != 0){
		finish_with_stmt_error(conn, stmt, "Errore nel binding dei parametri.\n", true);
		printf("\nPremi invio per continuare...\n");
		while(getchar()!= '\n'){}
		//free(type);
		free(cf);
		free(nome);
		free(cognome);
		free(pswd);
		goto top;
	}
	
	// Run the stored procedure
	if(mysql_stmt_execute(stmt) != 0){
		print_stmt_error(stmt, "\nErrore nella registrazione dell'utente Amministratore.\n");
		free(cf);
		free(nome);
		free(cognome);
		free(pswd);
		return;
	}
		
	printf(ANSI_COLOR_GREEN"\nUtente Amministratore registrato\n" ANSI_COLOR_RESET);
	free(cf);
	free(nome);
	free(cognome);
	free(pswd);
	
	mysql_stmt_close(stmt);
}



void admin(MYSQL *conn, char *cf){

	char operation[20];

	if(mysql_change_user(conn, getJson(ADMIN, USERNAME), getJson(ADMIN, PASSWORD), getJson(ADMIN, DATABASE))){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
	
	memcpy(logged_user.cf, cf, 64);
		
	start: 

	printf("\033[2J\033[H");
	printf("Connected as");
    	printf(ANSI_COLOR_BOLD" %s"ANSI_COLOR_OFF , cf);
    	printf("-admin\n\n");	
	
    	printf("....................................................................................\n");
	printf("....................................................................................\n");
	printf("............................|     MENU AMMINISTRATORE    |..........................\n");
	printf("....................................................................................\n");
	printf("....................................................................................\n\n\n");

	printf(" ____ ________ ________ ____ Gestione delle Categorie ____ ________ ________ _______\n");
	printf("|                                                                                   |\n");
	printf("|                                                                    [LOG OUT: 0]   |\n");
	printf("|                                                                                   |\n");
  	printf("|   OPERAZIONE 1 : Visualizzazione titolario completo                               |\n");
   	printf("|   OPERAZIONE 2 : Inserimento categoria                                            |\n");
	printf("|   OPERAZIONE 3 : Aggiornamento categoria                                          |\n");
  	printf("|   OPERAZIONE 4 : Cancellazione categoria                                          |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______|\n\n");

	printf(" _____ ________ ________ ______ Gestione delle Aste ______ ________ ________ _______\n");
	printf("|                                                                                   |\n");
   	printf("|   OPERAZIONE 5 : Inizializzazione di una nuova asta                               |\n");
   	printf("|   OPERAZIONE 6 : Generazione Report di un'asta                                    |\n");
	printf("|   OPERAZIONE 7 : Visualizzazione aste indette                                     |\n");
  	printf("|   OPERAZIONE 8 : Visualizzazione dello stato di un'asta                           |\n");
  	printf("|   OPERAZIONE 9 : Aggiungu utente Amministratore	                            |\n");
	printf("|____ ________ ________ ________ ________ ________ ________ ________ ________ ______|\n\n");
  	printf("   Inserisci il codice dell'operazione : ");
  		
  	getInput(64, operation, false);
		
	if(!strcmp(operation, "0")){
    		printf(ANSI_COLOR_RED"\n   Logout...\n\n" ANSI_COLOR_RESET);
    		printf("\n\n Premi invio per tornare al Menù Login...\n");
        	while(getchar() != '\n'){}
		return;
    	}
    	else if(!strcmp(operation, "1")){
    		op_a1(conn);
    	}
    	else if(!strcmp(operation, "2")){
    		op_a2(conn);
    	}
    	else if(!strcmp(operation, "3")){
    		op_a3(conn);
    	}
    	else if(!strcmp(operation, "4")){
    		op_a4(conn);
    	}
    	else if(!strcmp(operation, "5")){
    		op_a5(conn);
    	}
    	else if(!strcmp(operation, "6")){
    		op_a6(conn);
    	}
    	else if(!strcmp(operation, "7")){
    		op_a7(conn);
    	}
    	else if(!strcmp(operation, "8")){
    		op_3(conn);
    	}
    	else if(!strcmp(operation, "9")){
    		op_a9(conn);
    	}
    	else{
    		printf(ANSI_COLOR_RED"\n   Comando non riconosciuto" ANSI_COLOR_RESET);
    	}
    	
    	printf("\n\n Premi invio per tornare al Menù Utente...\n");
        while(getchar() != '\n'){}
        goto start;
  
}
