#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cmath>
#include <unistd.h>
#include <cstring>

#include "library.h"
#include "json/json.h"

using namespace std;

const char *pass0 = "pass0";
const char *pass1 = "pass1";

int main(int argc, const char* argv[]) {
  if (argc < 7) {
    cout << "ERROR: invalid input parameters!" << endl;
    cout << "Please enter <schema_file> <input_file> <output_file> <mem_capacity> <k> <sorting_attributes>" << endl;
    exit(1);
  }
  string schema_file(argv[1]);

  // Parse the schema JSON file
  Json::Value schema;
  Json::Reader json_reader;
  // Support for std::string argument is added in C++11
  // so you don't have to use .c_str() if you are on that.
  ifstream schema_file_istream(schema_file.c_str(), ifstream::binary);
  bool successful = json_reader.parse(schema_file_istream, schema, false);
  if (!successful) {
    cout << "ERROR: " << json_reader.getFormatedErrorMessages() << endl;
    exit(1);
  }

  // Print out the schema and init schema for later use
  string attr_name;
  int attr_len;
  int schema_len = schema.size();

  Schema *sm = (Schema *) malloc(sizeof(Schema));
  sm->attrs = (Attribute *) malloc(sizeof(Attribute) * schema_len);
  sm->nattrs = schema_len;
  sm->n_sort_attrs = argc - 6;
  sm->sort_attrs = (int *) malloc(sizeof(int) * sm->n_sort_attrs);
  memset(sm->sort_attrs, 0, sizeof(int) * sm->n_sort_attrs);

  for (int i = 0; i < schema_len; ++i) {
    attr_name = schema[i].get("name", "UTF-8" ).asString();
    attr_len = schema[i].get("length", "UTF-8").asInt();

    Attribute *attr = &(sm->attrs[i]);
    attr->name = (char *) malloc(sizeof(char) * attr_name.size());
    strcpy(attr->name, attr_name.c_str());
    attr->length = attr_len;

    for (int i = 6; i < argc; i++) {
        for (int j = 0; j < schema_len; j++) {
            if (!schema[j].get("name", "UTF-8").asString().compare(argv[i])) {
                sm->sort_attrs[i - 6] = j;
                break;
            } else if (j == schema_len - 1) {
                cout << "ERROR: invalid sorting attribute " << "\"" << argv[i] << "\"" << endl;
                exit(1);
            }
        }
    }
  }

  //start timer
  clock_t start = clock();

  std::ifstream f(argv[2]);
  std::string line;
  int num_records = 0;
  for (int i = 0; std::getline(f, line); ++i) {
    num_records++;
  }

  FILE *in_fp = fopen(argv[2], "r");
  if (in_fp == NULL) {
    cout << "ERROR: fail to open input file" << endl;
    exit(1);
  }

  FILE *pass0_fp = fopen(pass0, "w");
  if (pass0_fp == NULL) {
    cout << "ERROR: fail to create pass0 file" << endl;
    exit(1);
  }

  int line_length = get_line_length(sm);
  int mem_capacity = atoi(argv[4]);
  int run_length = mem_capacity / line_length;
  mk_runs(in_fp, pass0_fp, run_length, sm);
  fclose(pass0_fp);

  int turn = 1;

  FILE *in;
  FILE *out;

  int k = atoi(argv[5]);
  if (k <= 1) {
    cout << "ERROR: <k> has to be greater than 1" << endl;
    exit(1);
  }
  if (mem_capacity < (line_length + 1) * (k + 1)) {
    cout << "ERROR: <mem_capacity> not enough, please have bigger <mem_capacity> or smaller <k>" << endl;
    exit(1);
  }
  long buf_size = mem_capacity / (k + 1);

  FILE *out_fp = fopen(argv[3], "w");
  if (out_fp == NULL) {
    cout << "ERROR: fail to create output file" << endl;
  }

  while (1) {
    int merge_length = k * run_length;
    int num_merge = ceil((double) num_records / merge_length);
    int last_merge = num_records % merge_length;

    if (turn) {
      in = fopen(pass0, "r");
      if (in == NULL) {
        cout << "ERROR: fail to open input file" << endl;
        exit(1);
      };
      out = fopen(pass1, "w");
      if (out == NULL) {
        cout << "ERROR: fail to open input file" << endl;
        exit(1);
      };
      turn = 0;
    } else {
      in = fopen(pass1, "r");
      if (in == NULL) {
        cout << "ERROR: fail to open input file" << endl;
        exit(1);
      };
      out = fopen(pass0, "w");
      if (out == NULL) {
        cout << "ERROR: fail to open input file" << endl;
        exit(1);
      };
      turn = 1;
    }

    if (num_merge <= k - 1) {
      fclose(out);
      out = out_fp;
    }

    for (int i = 0; i < num_merge; i++) {
      int start_pos = i * merge_length;

      if (i == num_merge - 1) {
        /* Last merge */
        int num_run = ceil((double) last_merge / run_length);
        int last_run = last_merge % run_length;

        RunIterator *runIterator[num_run];
        for (int j = 0; j < num_run; j++) {
          if (j == num_run - 1) {
            /* Last run */
            runIterator[j] = new RunIterator(in, start_pos, last_run, buf_size, sm);
          } else {
            runIterator[j] = new RunIterator(in, start_pos, run_length, buf_size, sm);
            start_pos += run_length;
          }
        }
        char *buf = (char *) malloc(buf_size);
        merge_runs(runIterator, num_run, out, i * merge_length, buf, buf_size);
        free(buf);
      } else {
        RunIterator *runIterator[k];
        for (int j = 0; j < k; j++) {
          runIterator[j] = new RunIterator(in, start_pos, run_length, buf_size, sm);
          start_pos += run_length;
        }
        char *buf = (char *) malloc(buf_size);
        merge_runs(runIterator, k, out, i * merge_length, buf, buf_size);
        free(buf);
      }
    }
    fclose(in);
    fclose(out);

    /* Update run length */
    if (num_merge > k - 1) {
      run_length *= k;
    } else {
      break;
    }
  }

  // fclose(in_fp);
  unlink(pass0);
  unlink(pass1);
  // fclose(out_fp);

  for (int i = 0; i < schema_len; ++i) {
    Attribute *attr = &(sm->attrs[i]);
    free(attr->name);
  }
  free(sm->attrs);
  free(sm->sort_attrs);
  free(sm);

  int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;
  fprintf(stdout, "Sorted %d records in %d milliseconds\n", num_records, msecTime);

  return 0;
}
