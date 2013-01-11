//
//  table.h
//  weatherman
//
//  Created by Leo G Dion on 1/9/13.
//  Copyright (c) 2013 Leo Dion. All rights reserved.
//

#ifndef weatherman_table_h
#define weatherman_table_h

#define SQLTYPE_REAL "real"
#define SQLTYPE_TEXT "text"
#define SQLTYPE_INTEGER "integer"

#define DATATYPE_REAL "real"
#define DATATYPE_TEXT "text"
#define DATATYPE_INTEGER "integer"
#define DATATYPE_BOOLEAN "boolean"

typedef enum COLUMN_TYPE {
	sqltype_real,
	sqltype_text,
	sqltype_integer
} column_type;

typedef enum DATA_TYPE {
	datatype_null,
	datatype_real,
	datatype_text,
	datatype_integer,
	datatype_boolean
} data_type;

typedef int nullable_type;

//typedef const char * column_type;

typedef char * (*parser)(const void*);

typedef struct _column {
    const char * name;
    column_type type;
    nullable_type nullable;
    int primary_key;
    parser parser;
} column;

typedef struct _table {
    unsigned int length;
    column * columns;
	const char * name;
} table;

typedef struct _table_builder {
	const char * name;
    int current_position;
    column * columns;
} table_builder;

typedef struct _table_column_data {
    const char * name;
    const void * value;
	data_type type;
} table_column_data;

typedef struct _table_data {
    table_column_data * columns;
    unsigned int size;
    unsigned int length;
} table_data;

const char * get_columntype_name (column_type type);

table_data * create_table_data (unsigned int length);

int add_data_column(table_data * data, const char * name, const void * value, data_type data_type);
 
column * create_column(const char * name, column_type type, nullable_type nullable, int primary_key, parser parser);

table * create_table(const char * name, int length, column * columns);

table_builder * create_table_builder(const char * name, int max_columns);

column * add_column(table_builder * builder, const char * name, column_type type, nullable_type nullable, int primary_key, parser parser);

table * build_table(table_builder * builder);

const char * build_query_insert(table * table, table_data * data);

const char * build_query_create(table * table);

#endif
