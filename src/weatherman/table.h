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

typedef int nullable_type;

typedef const char * column_type;

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
} table_column_data;

typedef struct _table_data {
    table_column_data * columns;
    unsigned int size;
    unsigned int length;
} table_data;

table_data * create_table_data (unsigned int length) {
    table_data * data;
    data = (table_data *)malloc(sizeof(table_column_data) * length);
    data->columns = (table_column_data*)malloc(sizeof(table_column_data) * length);
    data->length = length;
    data->size = 0;
    return data;
}

int add_data_column(table_data * data, const char * name, const void * value) {
    data->columns[data->size].name = name;
    data->columns[data->size].value = value;
    data->size = data->size + 1;
    return data->size;
}

 
column * create_column(const char * name, column_type type, nullable_type nullable, int primary_key, parser parser) {
    column * result;
    result = (column *)malloc(sizeof(column));
    result->name = name;
    result->type = type;
    result->nullable = nullable;
    result->primary_key = primary_key;
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

table_builder * create_table_builder(const char * name, int max_columns) {
    table_builder * builder;
    builder = malloc(sizeof(table_builder));
    builder->name = name;
    builder->current_position = 0;
    builder->columns = malloc(sizeof(column) * max_columns);
    return builder;
}

column * add_column(table_builder * builder, const char * name, column_type type, nullable_type nullable, int primary_key, parser parser) {
    column * result;
    result = create_column(name, type, nullable, primary_key, parser);
    builder->columns[builder->current_position] = *result;
    builder->current_position = builder->current_position + 1;
    return result;
}

table * build_table(table_builder * builder) {
    table * result;
    result = create_table(builder->name, builder->current_position, builder->columns);
    return result;
}

const char * build_query_insert(table * table, table_data * data) {
    char * query;
    char * current;
    int index, jndex;
    char * names;
    char * values;
    
    current = (char *)malloc(128 * sizeof(char));
    query = (char*)malloc(1024 * sizeof(char));
    names = (char*)malloc(1024 * sizeof(char));
    values = (char*)malloc(1024 * sizeof(char));
    
    sprintf(current, "insert into %s (\n", table->name);
    strcpy(query, current);
    
    for (index = 0; index < table->length; index = index + 1) {
        for (jndex = 0; jndex < data->length; jndex = jndex + 1) {
            if (strcmp(table->columns[index].name, data->columns[index].name) == 0) {
                strcat(names, table->columns[index].name);
                strcat(names, ", ");
            }
        }
    }
    
    for (index = 0; index < table->length; index = index + 1) {
        for (jndex = 0; jndex < data->length; jndex = jndex + 1) {
            if (strcmp(table->columns[index].name, data->columns[index].name) == 0) {
                char * value_str;
                if (table->columns[index].parser) {
                    value_str = table->columns[index].parser(data->columns[index].value);
                } else {
                    value_str = (char*)data->columns[index].value;
                }
                strcat(values, value_str);
                strcat(names, ", ");
            }
        }
    }
    
    strncpy(query, names, strlen(names) - 2);
    strcpy(query, ") values (");
    strncpy(query, values, strlen(values) - 2);
    
    return query;
};

const char * build_query_create(table * table) {
    char * query;
    char * current;
    const char * p_keys[5];
    
    int index;
    int size_p_keys = 0;
    
    current = (char *)malloc(128 * sizeof(char));
    query = (char*)malloc(1024 * sizeof(char));
    
    sprintf(current, "create table %s(\n", table->name);
    strcpy(query, current);
    
    for (index = 0; index < table->length; index = index + 1) {
        sprintf(current, "\"%s\" %s%s%s\n", table->columns[index].name, table->columns[index].type,
                table->columns[index].nullable?"":" NOT NULL",
                (index < table->length-1 || size_p_keys>0?",":""));
        strcat(query, current);
        
        if (table->columns[index].primary_key) {
            p_keys[size_p_keys] = table->columns[index].name;
            size_p_keys = size_p_keys + 1;
        }
    }
    
    
    if (size_p_keys) {
        strcat(query, "primary key(");
        for(index = 0; index < size_p_keys; index = index + 1) {
            strcat(query, p_keys[index]);
            if (index < size_p_keys - 1) {
                strcat(query, ", ");
            }
        }
//        join_str(current, size_p_keys, ", ", p_keys);
//        strcat(query, current);
        strcat(query, ")\n");
    }
    strcat(query, ");");
    
    return query;
}

#endif
