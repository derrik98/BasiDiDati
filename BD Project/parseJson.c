#include<stdio.h>
#include<json-c/json.h>

const char* getJson(char *user, char *record){
	
	FILE *fp;
	char buffer[1024];
	struct json_object *parsed_json;
	struct json_object *jObj;
	struct json_object *result;	

	fp = fopen("test.json","r");
	fread(buffer, 1024, 1, fp);
	fclose(fp);

	parsed_json = json_tokener_parse(buffer);
	
	jObj = json_object_object_get(parsed_json, user);
	int count = json_object_array_length(jObj);

   	json_object *element = json_object_array_get_idx(jObj, 0);
   	result = json_object_object_get(element, record);
  
	return json_object_get_string(result);
}

