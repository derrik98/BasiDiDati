#include <mysql.h>
#include <stdbool.h>

#define HOST "host"
#define USERNAME "username"
#define PASSWORD "password"
#define DATABASE "database"
#define PORT "port"

#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\e[0;32m"
#define ANSI_COLOR_RESET	"\x1b[0m"
#define ANSI_COLOR_BOLD  	"\e[1m"
#define ANSI_COLOR_OFF   	"\e[m"


typedef struct{
	char cf[16];
} UTENTE;


int getInput(unsigned int lung, char *string, bool hide);
void admin(MYSQL *conn, char *username);
void standard(MYSQL *conn, char *username);
extern void print_stmt_error(MYSQL_STMT *stmt, char *message);
extern void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool lcose_stmt);
extern bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *conn);
extern void procedure_output(MYSQL *conn, MYSQL_STMT *stmt);
extern void op_3(MYSQL *conn);
extern void op_a1(MYSQL *conn);
const char* getJson(char *user, char *record);
