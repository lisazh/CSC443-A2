#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>

#include "library.h"

using namespace std;

string get_sort_attrs(Record *record, Schema *schema);
int compare_records(const void * a, const void * b);

void mk_runs(FILE *in_fp, FILE *out_fp, long run_length, Schema *schema)
{
    if (in_fp == NULL) {
        fprintf(stderr, "%s\n", "Input file doesn't exist");
        exit(2);
    }

    int line_length = get_line_length(schema);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Record *records = (Record *) malloc(sizeof(Record) * run_length);
    int i;

    for (i = 0; (read = getline(&line, &len, in_fp)) != -1; i++) {
        records[i].data = (char *) malloc(line_length + 1);
        strncpy(records[i].data, line, line_length);
        records[i].data[line_length] = '\0';
        records[i].schema = schema;

        if (i == run_length - 1) {
            qsort(records, run_length, sizeof(Record), compare_records);

            for (int j = 0; j < run_length; j++) {
                fputs(records[j].data, out_fp);
                putc('\n', out_fp);

                free(records[j].data);
            }
            free(records);
            records = (Record *) malloc(sizeof(Record) * run_length);
            i = -1;
        }
    }

    qsort(records, i, sizeof(Record), compare_records);
    for (int j = 0; j < i; j++) {
        fputs(records[j].data, out_fp);
        putc('\n', out_fp);

        free(records[j].data);
    }
    free(line);
    free(records);
}

int compare_records(const void * a, const void * b) {
    Record *r1 = (Record *) a;
    Record *r2 = (Record *) b;

    Schema *schema = r1->schema;

    string key1 = get_sort_attrs(r1, schema);
    string key2 = get_sort_attrs(r2, schema);

    return key1.compare(key2);
}

string get_sort_attrs(Record *record, Schema *schema) {
    string result;
    char *attr;
    char *temp = (char *) malloc(strlen(record->data) + 1);

    strcpy(temp, record->data);

    attr = strtok(temp, ",");
    vector<char *> attrs;

    for (int i = 0; attr != NULL; i++) {
        attrs.push_back(attr);
        attr = strtok(NULL, ",");
    }

    for (int i = 0; i < schema->n_sort_attrs; i++) {
        result.append(attrs.at(schema->sort_attrs[i]));
    }

    return result;
}

void merge_runs(RunIterator* iterators[], int num_runs, FILE *out_fp,
                long start_pos, char *buf, long buf_size)
{
    Record *records = (Record *) malloc(num_runs * sizeof(Record));
    Record *r;
    int alive_runs = num_runs;
    int hold_records;
    int line_length;
    int offset = 0;
    int alive[num_runs];
    int num_records = 0;

    for (int i = 0; i < num_runs; i++) {
        if (iterators[i]->has_next()) {
            r = iterators[i]->next();
            records[i] = *r;
            line_length = get_line_length(r->schema);

            free(r);
            alive[i] = 1;
        } else {
            alive_runs--;
            alive[i] = 0;
        }
    }
    fseek(out_fp, start_pos * (line_length + 1), SEEK_SET);

    hold_records = buf_size / (line_length + 1);

    while (alive_runs) {
        num_records += 1;
        Record smallest_record = records[0];
        int smallest = 0;
        for (int i = 1; i < num_runs; i++) {
            if (!alive[smallest] || (alive[i] && compare_records(&smallest_record, &(records[i])) > 0)) {
                smallest_record = records[i];
                smallest = i;
            }
        }

        if (offset == hold_records) {
            fwrite(buf, (line_length + 1) * offset, 1, out_fp);
            offset = 0;
        }
        strncpy(buf + offset * (line_length + 1), smallest_record.data, line_length);
        memset(buf + offset * (line_length + 1) + line_length, '\n', 1);
        offset++;
        free(smallest_record.data);

        if (iterators[smallest]->has_next()) {
            r = iterators[smallest]->next();
            records[smallest] = *r;

            free(r);
        } else {
            alive_runs--;
            alive[smallest] = 0;
        }
    }
    fwrite(buf, (line_length + 1) * offset, 1, out_fp);
    free(records);
}

RunIterator::RunIterator(FILE *fp, long start_pos, long run_length, long buf_size,
              Schema *schema) {
    records = NULL;
    file = fp;
    cur_pos = 0;
    iter_start_pos = start_pos;
    iter_run_length = run_length;
    iter_buf_size = buf_size;
    iter_schema = schema;
    line_length = get_line_length(iter_schema);
    hold_records = iter_buf_size / line_length;
}

RunIterator::~RunIterator() {
    if (records != NULL) {
        free(records);
    }
}

Record* RunIterator::next() {
    int nth_record = cur_pos % hold_records;
    if (nth_record == 0) {
        if (records != NULL) {
            free(records);
        }
        records = (char *) malloc(iter_buf_size);
        fseek(file, (iter_start_pos + cur_pos) * (line_length + 1), SEEK_SET);

        char *line = (char *) malloc(line_length + 2);
        char *offset = records;
        for (int i = 0; i < hold_records && fread(line, line_length + 1, 1, file); i++) {
            line[line_length + 1] = '\0';
            strncpy(offset, line, line_length);
            offset += line_length;
        }
        free(line);
        *offset = '\0';
    }

    Record *record = (Record *) malloc(sizeof(Record));
    record->schema = iter_schema;
    record->data = (char *) malloc(line_length + 1);
    strncpy(record->data, records + line_length * nth_record, line_length);
    record->data[line_length] = '\0';

    cur_pos++;

    return record;
}

bool RunIterator::has_next() {
    return cur_pos < iter_run_length;
}

int get_line_length(Schema *schema) {
    int result = schema->nattrs - 1;
    for (int i = 0; i < schema->nattrs; i++) {
        result += schema->attrs[i].length;
    }
    return result;
}








