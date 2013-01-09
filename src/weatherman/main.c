//
//  main.c
//  weatherman
//
//  Created by Leo G Dion on 1/8/13.
//  Copyright (c) 2013 Leo Dion. All rights reserved.
//
/*
#define INITIALIZE_SQL "create table conditions ("
"latitude real not null,"
"longitude   real not null,"
"elevation   real not null,"
"station_id  text not null,
observation_epoch integer not null,
local_epoch integer not null,
local_tz_offset   integer not null,
weather  text not null,
temp_c   real not null,
relative_humidity real not null,
wind_degrees   real not null,
wind_kph real not null,
wind_gust_kph  real not null,
pressure_mb real not null,
pressure_trend real not null,
dewpoint_c  real,
heat_index_c   real,
windchill_c real,
visibility_km  real not null,
solarradiation real not null,
UV real not null,
precip_1hr_metric real not null,
precip_today_metric  real not null,
primary key (station_id, observation_epoch)
);"
*/

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include "json.h"
#include "table.h"

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
	//char * sql;
	size_t fileSize;
	size_t length;
	const char * dbPath = "file:weatherman.db";
	const char * tableSql = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name";
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;
    int tableExists = 0;
    
    /*
    text = malloc(sizeof(char) * 256);
    getcwd(text);
    puts(text);
	text  = (char *)malloc(sizeof(char) * 4096);
	memset(text, '\0', 4096 * sizeof(char));
// "..\\..\\db\\initialize.sql"
	sqlFile = fopen("../../db/initialize.sql", "r");
	fseek(sqlFile, 0, SEEK_END);
	fileSize = ftell(sqlFile);
	fseek(sqlFile, 0, SEEK_SET);
	sql = (char*)malloc(sizeof(char) * fileSize);
	length = fread(sql, 1, fileSize, sqlFile);
	sql[length] = '\0';
*/
	sqlite3_open(dbPath, &db);

	result = sqlite3_prepare(db, tableSql, -1, &stmt, NULL);

	while ((result = sqlite3_step(stmt)) == 100) {
		if (strcmp((char*)sqlite3_column_text(stmt, 0),"conditions") == 0) {
            tableExists = 1;
            break;
        }
	}
	sqlite3_finalize(stmt);

    if (tableExists != 0) {
//      result = sqlite3_prepare(db, sql, (int)length, &stmt, NULL);
//      result = sqlite3_step(stmt);
//      sqlite3_finalize(stmt);
    }

	sqlite3_close(db);


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

