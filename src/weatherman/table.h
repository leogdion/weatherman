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

typedef int boolean;

typedef const char * column_type;

typedef char * (*parser)(void*);

typedef struct _column {
    const char * name;
    column_type type;
    boolean nullable;
    parser parser;
} column;

typedef struct _table {
    unsigned int length;
    column * columns;
} table;

column * create_column(const char * name, const char * type, boolean nullable, parser parser) {
    column * result;
    result = malloc(sizeof(column));
    result->name = name;
    result->type = type;
    result->nullable = nullable;
    result->parser = parser;
    return result;
}

table * create_table(int length, column * columns) {
    table * result;
    result = malloc(sizeof(table));
    result->length = length;
    memcpy(result->columns, columns, sizeof(column) * length);
    return result;
}

#endif
