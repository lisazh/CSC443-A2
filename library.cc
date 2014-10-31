#include "library.h"

void mk_runs(FILE *in_fp, FILE *out_fp, long run_length, Schema *schema)
{
  // Your implementation
}

void merge_runs(RunIterator* iterators[], int num_runs, FILE *out_fp,
                long start_pos, char *buf, long buf_size)
{
  // Your implementation
}

/**
 * Read the given schema file.
 */
void read_schema_file(const char *schema_f, Schema *sm) {
  string schema_file(schema_f);

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

  sm->attrs = (Attribute *) malloc(sizeof(Attribute) * schema_len);
  sm->sort_attrs = (int *) malloc(sizeof(int) * schema_len);

  for (int i = 0; i < schema_len; ++i) {
    attr_name = schema[i].get("name", "UTF-8" ).asString();
    attr_len = schema[i].get("length", "UTF-8").asInt();

    Attribute *attr = &(sm->attrs[i]);
    attr->name = (char *) malloc(sizeof(char) * attr_name.size());
    strcpy(attr->name, attr_name.c_str());
    attr->length = attr_len;
  }
}
