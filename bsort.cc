#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include "leveldb/db.h"
#include "json/json.h"

using namespace std;

int main(int argc, const char* argv[]) {

    if (argc < 5) {
        cout << "ERROR: invalid input parameters!" << endl;
        cout << "Please enter <schema_file> <input_file> <out_index> <sorting_attributes>" << endl;
        exit(1);
    }

    string schema_file(argv[1]);
    int sort_attr[argc - 4];

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

    for (int i = 4; i < argc; i++) {
        for (int j = 0; j < schema_len; j++) {
            if (!schema[j].get("name", "UTF-8").asString().compare(argv[i])) {
                sort_attr[i - 4] = j;
                break;
            } else if (j == schema_len - 1) {
                cout << "ERROR: invalid sorting attribute " << "\"" << argv[i] << "\"" << endl;
                exit(1);
            }
        }
    }

    //start timer
    clock_t start = clock();

    leveldb::DB *db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "./leveldb_dir", &db);

    ifstream in_stream;

    string line;

    in_stream.open(argv[2]);
    int count = 0;

    while(1) {
        in_stream >> line;
        if (in_stream.eof()) {
            break;
        }

        string attr;
        vector<string> attrs;
        string key;
        stringstream stream(line);

        for (int i = 0; getline(stream, attr, ','); i++) {
            attrs.push_back(attr);
        }

        for (int i = 0; i < argc - 4; i++) {
            key.append(attrs.at(sort_attr[i]));
        }
        stringstream ss;
        ss << count;
        key.append(ss.str().c_str());
        count++;

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

        myfile << val_str << endl;

        db->Delete(leveldb::WriteOptions(), key);
    }
    assert(it->status().ok());  // Check for any errors found during the scan
    delete it;
    delete db;

    myfile.close();

    int msecTime = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    fprintf(stdout, "Sorted %d records in %d milliseconds\n", count, msecTime);

    return 0;
}