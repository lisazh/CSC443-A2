#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

#include "library.h"
#include "json/json.h"

using namespace std;

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

  
  // Do the sort
  // Your implementation

  // FILE *in_fp = fopen(argv[2], "r");
  // if (in_fp == NULL) {
  //   cout << "ERROR: fail to open input file" << endl;
  // }

  // FILE *out_fp = fopen(argv[3], "w");
  // if (out_fp == NULL) {
  //   cout << "ERROR: fail to create output file" << endl;
  // }

  // mk_runs(in_fp, out_fp, 30, sm);
  // fclose(in_fp);
  // fclose(out_fp);

  // FILE *run_fp = fopen(argv[3], "r");
  // if (run_fp == NULL) {
  //   cout << "ERROR: fail to create output file" << endl;
  // }

  // cout << "Number of sort attr: " << sm->n_sort_attrs << endl;
  // for (int i = 0; i < sm->n_sort_attrs; i++) {
  //   cout << "Sorting on attr: " << sm->sort_attrs[i] << endl;
  // }
  // for (int i = 0; i < sm->nattrs; ++i) {
  //   Attribute *attr = &(sm->attrs[i]);
  //   cout << "{name: " << attr->name << ", length: " << attr->length << "}" << endl;
  // }

  // RunIterator *runIterator[4];

  // out_fp = fopen("final_output.csv", "w");
  // if (out_fp == NULL) {
  //   cout << "ERROR: fail to create output file" << endl;
  // }

  // runIterator[0] = new RunIterator(run_fp, 0, 30, 100, sm);
  // runIterator[1] = new RunIterator(run_fp, 30, 30, 100, sm);
  // runIterator[2] = new RunIterator(run_fp, 60, 30, 100, sm);
  // runIterator[3] = new RunIterator(run_fp, 90, 10, 100, sm);

  // char * buf = (char *) malloc(100);

  // merge_runs(runIterator, 4, out_fp, 0, buf, 100);

  // free(buf);
  // fclose(run_fp);
  // fclose(out_fp);

  return 0;
}
