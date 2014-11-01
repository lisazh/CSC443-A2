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
  sm->sort_attrs = (int *) malloc(sizeof(int) * schema_len);

  for (int i = 0; i < schema_len; ++i) {
    attr_name = schema[i].get("name", "UTF-8" ).asString();
    attr_len = schema[i].get("length", "UTF-8").asInt();
    cout << "{name : " << attr_name << ", length : " << attr_len << "}" << endl;

    Attribute *attr = &(sm->attrs[i]);
    attr->name = (char *) malloc(sizeof(char) * attr_name.size());
    strcpy(attr->name, attr_name.c_str());
    attr->length = attr_len;
  }

  // Do the sort
  // Your implementation
  
  return 0;
}
