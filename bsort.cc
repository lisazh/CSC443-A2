#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include "leveldb/db.h"
#include "json/json.h"

using namespace std;

int main(int argc, const char* argv[]) {

    if (argc != 4) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please enter <schema_file> <input_file> <out_index>" << endl;
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

    int schema_len = schema.size();
    int key_attr[schema_len];

    for (int i = 0; i < schema_len; ++i) {
        key_attr[i] = schema[i].get("id", "UTF-8").asString().compare("y");
    }

    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "./leveldb_dir", &db);

    ifstream in_stream;

    string line;

    in_stream.open(argv[2]);

    while(!in_stream.eof())
    {
        in_stream >> line;
        string attr;
        string key;
        stringstream stream(line);
        for (int i = 0; getline(stream, attr, ','); i++) {
            if (!key_attr[i]) {
                key.append(attr);
            }
        }
        leveldb::Slice db_key = key;
        leveldb::Slice value = string(line);
        db->Put(leveldb::WriteOptions(), db_key, value); 
    }
    ofstream myfile;
    myfile.open (argv[3]);

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        leveldb::Slice key = it->key();
        leveldb::Slice value = it->value();
        std::string key_str = key.ToString();
        std::string val_str = value.ToString();
        myfile << key_str << ": "  << val_str << endl;
    }
    assert(it->status().ok());  // Check for any errors found during the scan
    delete it;

    myfile.close();

    return 0;
}