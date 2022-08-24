#include <stdio.h>
#include <string.h>
#include <time.h>

#include "defines.h"

int *buff_table;

void print_error(MYSQL *con, char *message){
	
	fprintf(stderr, "%s\n", message);
	if(con != NULL){
	#if MYSQL_VERSION_ID >= 40101
		fprintf(stderr, "Error %u, (%s): %s\n", mysql_errno(con), mysql_sqlstate(con), mysql_error(con));
	#else
		fprintf(stderr, "Error %u: %s\n", mysql_errno(con), mysql_error(con));
	#endif
	}
}

void print_stmt_error(MYSQL_STMT *stmt, char *message){

	fprintf(stderr, "%s\n", message);
	if(stmt != NULL){
		fprintf(stderr, "Error %u (%s): %s\n", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
	}
}


bool setup_prepared_stmt(MYSQL_STMT **stmt, char *statement, MYSQL *con){
	
	bool update_length = true;
	*stmt = mysql_stmt_init(con);
	if(*stmt == NULL){
		print_error(con, "Could not initializestatement handler");
		return false;
	}
	
	if(mysql_stmt_prepare(*stmt, statement, strlen(statement)) != 0){
		print_stmt_error(*stmt, "Could not prepare statement");
		return false;
	}
	
	mysql_stmt_attr_set(*stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update_length);
	return true;
}

void finish_with_stmt_error(MYSQL *conn, MYSQL_STMT *stmt, char *message, bool close_stmt){
	
	print_stmt_error(stmt, message);
	if(close_stmt) mysql_stmt_close(stmt);
	mysql_close(conn);
	exit(EXIT_FAILURE);
}


int checkDate(MYSQL_TIME *date){
	
	struct tm* ptr;
	time_t t;
	t = time(NULL);
	ptr = localtime(&t);
	
	//check year
	if(date->year > ptr->tm_year + 1900){
        	return 1;
    	}
	else if (date->year < ptr->tm_year + 1900)
	{
        	return 0;
    	}
    	// year1 ==  year2
    	else{
    		//check month
        	if (date->month ==  ptr->tm_mon + 1){
        		// check day
            		if (date->day ==  ptr->tm_mday){
                		//check hour
                		if(date->hour == ptr->tm_hour + 1){
                			//chech minute
                			if(date->minute == ptr->tm_min + 1){
                				//check second
                				if(date->second > ptr->tm_sec + 1){
                					return 1;
                				}
            					else{
                	        			return 0;
            					}
                			}
                			else if(date->minute > ptr->tm_min + 1){
                        			return 1;
            				}
            				else{
                	        		return 0;
            				}
                		}
                		else if(date->hour> ptr->tm_hour + 1){
                			return 1;
            			}
            			else{
                			return 0;
            			}
           		}
            		else if(date->day > ptr->tm_mday){
                        	return 1;
            		}
            		else{
                		return 0;
            		}
        	}
        	else if (date->month > ptr->tm_mon + 1){
            		return 1;
        	}
        	else{
            		return 0;
        	}        
    }
}


static void print_dashes(MYSQL_RES *res_set){

	MYSQL_FIELD *field;
	unsigned int i, j;
	mysql_field_seek(res_set, 0);
	putchar('+');
	for (i = 0; i < mysql_num_fields(res_set) ; i++){
		field = mysql_fetch_field(res_set);
		for(j = 0; j < buff_table[i] + 2; j++){
			putchar('-');
		}
		putchar('+');
	}
	putchar('\n');
}

static void dump_result_set_header(MYSQL_RES *res_set){

	MYSQL_FIELD *field;
	unsigned long col_len;
	unsigned int i;
	
	mysql_field_seek(res_set, 0);
	putchar('|');
	
	for(i = 0; i < mysql_num_fields(res_set); ++i){
	
		field = mysql_fetch_field(res_set);
		
		if(field->type == MYSQL_TYPE_TIMESTAMP){
			printf(ANSI_COLOR_BOLD" %-*s "ANSI_COLOR_OFF, (int)(strlen(field->name)*3), field->name);
			printf("|");
			buff_table[i] = (int)(strlen(field->name)*3);
		}
			
		else{
			printf(ANSI_COLOR_BOLD" %-*s " ANSI_COLOR_OFF, (int)(strlen(field->name)*2), field->name);
			printf("|");
			buff_table[i] = (int)(strlen(field->name)*2);
		}
	}
	putchar('\n');
	print_dashes(res_set);
	putchar('\n');
}


void procedure_output(MYSQL *conn, MYSQL_STMT *stmt){

	int i; 
	int status;
	int num_fields;		/*number of columns in result*/
	MYSQL_FIELD *fields;	/*for result set metadata*/
	MYSQL_BIND *rs_bind;	/*for output buffers*/
	MYSQL_RES *rs_metadata, *mysql_res;
	MYSQL_TIME *date;
	MYSQL_TIME *time;
	size_t attr_size;
	
	bool *is_null; // output value nullability
		
	if(mysql_stmt_store_result(stmt)){
		fprintf(stderr, "mysql_stmt_execute() failed.\n");
		fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
		exit(0);
	}
	
	
	/*the column count is > 0 if there is a result set*/
	/*0 if the result is only the final status packet*/
	
	num_fields = mysql_stmt_field_count(stmt);
	is_null = malloc(sizeof(num_fields));
	
	buff_table = malloc(64*sizeof(num_fields));
	
	do {
	
		if(num_fields > 0){
		
			/*there is a result set to fetch*/
			// Get information about the outcome of the stored procedure
			if((rs_metadata = mysql_stmt_result_metadata(stmt)) == NULL){
				finish_with_stmt_error(conn, stmt, "Unable to retrieve result metadata\n", true);
			}
		
			dump_result_set_header(rs_metadata);
			
			// Retrieve the fields associated with OUT/INOUT parameters
			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
		
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			// set up and bind result set output buffers
			for (i = 0; i < num_fields; ++i) {

				//Properly size the parameter buffer
				switch (fields[i].type) {
				
					
					
					case MYSQL_TYPE_TIMESTAMP:
					case MYSQL_TYPE_DATETIME:
					case MYSQL_TYPE_DATE:
					case MYSQL_TYPE_TIME:
						attr_size = sizeof(MYSQL_TIME);
						break;
					
					case MYSQL_TYPE_FLOAT:
						attr_size = sizeof(float);
						break; 

					case MYSQL_TYPE_DOUBLE:
						attr_size = sizeof(double);
						break; 
						
					case MYSQL_TYPE_TINY:
						attr_size = sizeof(signed char);
						break;
						
					case MYSQL_TYPE_SHORT:
					case MYSQL_TYPE_YEAR:
						attr_size = sizeof(short int);
						break;
						
					case MYSQL_TYPE_LONG:
					case MYSQL_TYPE_INT24:
						attr_size = sizeof(int);
						break;
						
					case MYSQL_TYPE_LONGLONG:
						attr_size = sizeof(int);
						break; 
						
					default:
						attr_size = fields[i].max_length;
						break;
				}
				
				//setup the binding for the current parameter
				rs_bind[i].buffer_type = fields[i].type;
				rs_bind[i].buffer = malloc(attr_size + 1);
				rs_bind[i].buffer_length = attr_size + 1;
				
				rs_bind[i].is_null = &is_null[i];
				
				if(rs_bind[i].buffer == NULL){
					finish_with_stmt_error(conn, stmt, "Cannot allocate output parameters\n\n", true);
				}
			}
			
			if(mysql_stmt_bind_result(stmt, rs_bind)){
				finish_with_stmt_error(conn, stmt, "Unable to bind output parameters\n", true);
			}
			
			while(true){
			
				status = mysql_stmt_fetch(stmt);
				
				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}
				
				putchar('|');
				
				for(i = 0; i < num_fields; ++i){
					
					if(*rs_bind[i].is_null){
						printf(" %-*s |", buff_table[i], "NULL");
						continue;
					}
					
					switch (rs_bind[i].buffer_type) {

						case MYSQL_TYPE_VAR_STRING: 
						case MYSQL_TYPE_DATETIME: 
							printf(" %-*s |", buff_table[i], (char*)rs_bind[i].buffer);
							break;
					
						case MYSQL_TYPE_DATE: 
						case MYSQL_TYPE_TIMESTAMP: 
							date = (MYSQL_TIME *)rs_bind[i].buffer;
							if(checkDate(date)){
								printf(ANSI_COLOR_GREEN" %d-%02d-%02d %.02d:%.02d:%.04d %*s"ANSI_COLOR_RESET,date->year, date->month, date->day, date->hour, date->minute, date->second, buff_table[i] -(int)sizeof("0000-00-0000:00:0000"), "");
								printf("|");
							}
							else{
								printf(ANSI_COLOR_RED" %d-%02d-%02d %.02d:%.02d:%.04d %*s"ANSI_COLOR_RESET,date->year, date->month, date->day, date->hour, date->minute, date->second, buff_table[i] -(int)sizeof("0000-00-0000:00:0000"), "");
								printf("|");
							}
							break;
						
						case MYSQL_TYPE_STRING: 
							printf(" %-*s |", buff_table[i], (char*) rs_bind[i].buffer);
							break;

						case MYSQL_TYPE_FLOAT: 
						case MYSQL_TYPE_DOUBLE: 
							printf(" %-*.02f |", buff_table[i], *(float*) rs_bind[i].buffer);
							break;
						
						
						
						case MYSQL_TYPE_LONG: 
						case MYSQL_TYPE_SHORT: 
						case MYSQL_TYPE_TINY: 
							printf(" %-*d |", buff_table[i], *(int*) rs_bind[i].buffer);
							break;
				
						
						case MYSQL_TYPE_NEWDECIMAL: 
							printf(" %-*.021f |", buff_table[i], *(float*) rs_bind[i].buffer);
							break;
							
						case MYSQL_TYPE_TIME:
							time = (MYSQL_TIME *)rs_bind[i].buffer;
							printf("%.02d:%.02d %*s |", time->hour, time->minute, buff_table[i]  - (int)sizeof(time->hour) - (int)sizeof(time->minute), "");
							break;

						default:
							printf("ERROR: unexpected type (%d)\n", rs_bind[i].buffer_type);
							abort();
					}
				}
				putchar('\n');
				print_dashes(rs_metadata);
			}
				
			mysql_free_result(rs_metadata);	// free metadata
			
			/*free output buffers*/
			for (i = 0; i < num_fields; ++i){
				free(rs_bind[i].buffer);
			}
			
			free(rs_bind);	// free output buffers
	}
	else{
		printf("\n\nNo more output\n");
	}
			
	if(mysql_stmt_next_result(stmt) > 0){
		finish_with_stmt_error(conn, stmt, "Unable to bind output parameters\n", true);
	}
	
	} while (mysql_stmt_next_result(stmt) == 0);
	
	free(is_null);
	free(buff_table);
}
