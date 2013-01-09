//
//  main.c
//  weatherman
//
//  Created by Leo G Dion on 1/8/13.
//  Copyright (c) 2013 Leo Dion. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include "json.h"

static size_t write_data(char *ptr, size_t size, size_t nmemb, char *userdata)
{
	if (strlen(userdata) > 0) {
		strncat(userdata, ptr, nmemb);
	} else {
		strcpy(userdata, ptr);
	}
	return size * nmemb;
}

static void enumerate (json_value * value)
{
	int index;
	for (index = 0; index < value->u.object.length; index++) {
		puts(value->u.object.values[index].name);
		switch (value->u.object.values[index].value->type) 
		{
		case json_object:
			enumerate(value->u.object.values[index].value);
			break;
		case json_string:
			puts(value->u.object.values[index].value->u.string.ptr);
			break;
		case json_integer:
			printf("%d\n",(value->u.object.values[index].value->u.integer));
			break;
		case json_double:
			printf("%f\n",value->u.object.values[index].value->u.dbl);
			break;
		case json_boolean:
			printf("%d\n",value->u.object.values[index].value->u.boolean);
			break;
		case json_null:

		default:
			break;
		}
	}
}

int main(int argc, const char * argv[])
{
	char * text;
	CURL *curl;
	CURLcode res;
	json_value * value;
	FILE * sqlFile;
	char * sql;
	size_t fileSize;
	size_t length;
	const char * dbPath = "file://weatherman.db";
	sqlite3 * db;

	text  = (char *)malloc(sizeof(char) * 4096);
	memset(text, '\0', 4096 * sizeof(char));

	sqlFile = fopen("..\\..\\db\\initialize.sql", "r");
	fseek(sqlFile, 0, SEEK_END);
	fileSize = ftell(sqlFile);
	fseek(sqlFile, 0, SEEK_SET);
	sql = (char*)malloc(sizeof(char) * fileSize);
	length = fread(sql, 1, fileSize, sqlFile);
	sql[length] = '\0';

	sqlite3_open(dbPath, &db);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.wunderground.com/api/1128524773875b60/conditions/q/KLAN.json");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, text);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

#ifdef SKIP_PEER_VERIFICATION
		/*
		* If you want to connect to a site who isn't using a certificate that is
		* signed by one of the certs in the CA bundle you have, you can skip the
		* verification of the server's certificate. This makes the connection
		* A LOT LESS SECURE.
		*
		* If you have a CA cert for the server stored someplace else than in the
		* default bundle, then the CURLOPT_CAPATH option might come handy for
		* you.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERFICATION
		/*
		* If the site you're connecting to uses a different host name that what
		* they have mentioned in their server certificate's commonName (or
		* subjectAltName) fields, libcurl will refuse to connect. You can skip
		* this check, but this will make the connection less secure.
		*/
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
		res = curl_easy_perform(curl);
		value = json_parse(text);

		enumerate(value);
		/* always cleanup */
		curl_easy_cleanup(curl);
		free(text);
		free(value);

	}
	return 0;
}

