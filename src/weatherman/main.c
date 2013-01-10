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

static void enumerate (table_data * data, json_value * value)
{
	int index;
	for (index = 0; index < value->u.object.length; index++) {
		puts(value->u.object.values[index].name);
		switch (value->u.object.values[index].value->type) 
		{
		case json_object:
			enumerate(data, value->u.object.values[index].value);
			break;
		case json_string:
                add_data_column(data,value->u.object.values[index].name,value->u.object.values[index].value->u.string.ptr);
//			puts(value->u.object.values[index].value->u.string.ptr);
			break;
            case json_integer:
                add_data_column(data,value->u.object.values[index].name,&(value->u.object.values[index].value->u.integer));
//			printf("%d\n",(value->u.object.values[index].value->u.integer));
			break;
            case json_double:
                add_data_column(data,value->u.object.values[index].name,&(value->u.object.values[index].value->u.dbl));
//			printf("%f\n",value->u.object.values[index].value->u.dbl);
			break;
            case json_boolean:
                add_data_column(data,value->u.object.values[index].name,&(value->u.object.values[index].value->u.boolean));
//			printf("%d\n",value->u.object.values[index].value->u.boolean);
			break;
            case json_null:
                add_data_column(data,value->u.object.values[index].name,0);
                break;

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
	const char * dbPath = "file:weatherman.db";
	const char * tableSql = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name";
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;
    int tableExists = 0;
    table_builder * conditions_build;
    table * conditions;
    table_data * data;
    const char * query;
    
    text = (char*)malloc(sizeof(char) * 1024);
    data = (table_data*)malloc(sizeof(table_data));
	conditions_build = create_table_builder("conditions", 32);
    add_column(conditions_build, "station_id", SQLTYPE_TEXT, 0, 1, 0);
    add_column(conditions_build, "observation_epoch", SQLTYPE_INTEGER, 0, 1, 0);
    add_column(conditions_build, "latitude", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "longitude", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "elevation", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "local_epoch", SQLTYPE_INTEGER, 0, 0, 0);
    add_column(conditions_build, "local_tz_offset", SQLTYPE_INTEGER, 0, 0, 0);
    add_column(conditions_build, "weather", SQLTYPE_TEXT, 0, 0, 0);
    add_column(conditions_build, "temp_c", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "relative_humidity", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "wind_degrees", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "wind_kph", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "wind_gust_kph", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "pressure_mb", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "pressure_trend", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "dewpoint_c", SQLTYPE_REAL, 1, 0, 0);
    add_column(conditions_build, "heat_index_c", SQLTYPE_REAL, 1, 0, 0);
    add_column(conditions_build, "windchill_c", SQLTYPE_REAL, 1, 0, 0);
    add_column(conditions_build, "visibility_km", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "solarradiation", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "UV", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "precip_1hr_metric", SQLTYPE_REAL, 0, 0, 0);
    add_column(conditions_build, "precip_today_metric", SQLTYPE_REAL, 0, 0, 0);
    conditions = build_table(conditions_build);
    query = build_query_create(conditions);

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
        result = sqlite3_prepare(db, query, strlen(query), &stmt, NULL);
        result = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
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

		enumerate(data, value);
		/* always cleanup */
		curl_easy_cleanup(curl);
		free(text);
		free(value);

	}
	return 0;
}

