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
#define SQLTYPE_INT "integer"

typedef int nullable_type;

typedef const char * column_type;

typedef char * (*parser)(void*);

typedef struct _column {
    const char * name;
    column_type type;
    nullable_type nullable;
    parser parser;
} column;

typedef struct _table {
    unsigned int length;
    column * columns;
	const char * name;
} table;

typedef struct _table_builder {
	const char * name;
    column * columns;
} table_builder;

column * create_column(const char * name, column_type type, nullable_type nullable, parser parser) {
    column * result;
    result = (column *)malloc(sizeof(column));
    result->name = name;
    result->type = type;
    result->nullable = nullable;
    result->parser = parser;
    return result;
}

table * create_table(const char * name, int length, column * columns) {
    table * result;
    result = (table *)malloc(sizeof(table));
    result->length = length;
	result->name = name;
	result->columns = (column *)malloc(sizeof(column) * length);
    memcpy(result->columns, columns, sizeof(column) * length);
    return result;
}

table_builder * create_table_builder(const char * name) {
}

#endif
