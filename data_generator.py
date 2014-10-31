import random, csv, string
from decimal import *

TYPE = 'type'
LENGTH = 'length'
ID = 'id'
DIST = 'distribution'
DIST_NAME = 'name'
DIST_MIN = 'min'
DIST_MAX = 'max'
DIST_MU = 'mu'
DIST_SIGMA = 'sigma'

'''
You should implement this script to generate test data for your
merge sort program.

The schema definition should be separate from the data generation
code. See example schema file `schema_example.json`.
'''

def generate_data(schema, out_file, nrecords):
  '''
  Generate data according to the `schema` given,
  and write to `out_file`.
  `schema` is an list of dictionaries in the form of:
    [ 
      {
        'name' : <attribute_name>, 
        'length' : <fixed_length>,
        ...
      },
      ...
    ]
  `out_file` is the name of the output file.
  The output file must be in csv format, with a new line
  character at the end of every record.
  '''

  create_attr_by_type = {"string": create_string_attr, "integer": create_int_attr, "float": create_float_attr}
  supported_type = create_attr_by_type.keys()

  with open(out_file, 'wb') as csvfile:
    csv_writer = csv.writer(csvfile)
    for i in range(nrecords):
      row = []
      for attr_def in schema:
        attr_type = attr_def[TYPE]
        if attr_type in supported_type:
          row.append(create_attr_by_type[attr_type](attr_def))
        else:
          print 'Unsupported attribute type %s' % attr_type
          sys.exit(2)
      csv_writer.writerow(row)

  print "Generating %d records" % nrecords

def create_string_attr(attr_def):
  length = attr_def[LENGTH]
  if (attr_def[ID] == 'y'):
    return str(SequenceGenerator.next_value()).zfill(length)
  else:
    return ''.join(random.choice(string.letters + string.digits) for i in range(length))

def create_int_attr(attr_def):
  dist_def = attr_def[DIST]
  if dist_def[DIST_NAME] == 'uniform':
    min = dist_def[DIST_MIN]
    max = dist_def[DIST_MAX]
    return random.randint(min, max)
  else:
    print 'Unsupported attribute distribution %s for integer' % dist_def[DIST_NAME]
    sys.exit(2)

def create_float_attr(attr_def):
  dist_def = attr_def[DIST]
  length = attr_def[LENGTH]
  if dist_def[DIST_NAME] == 'normal':
    mu = dist_def[DIST_MU]
    sigma = dist_def[DIST_SIGMA]
    min = dist_def[DIST_MIN]
    max = dist_def[DIST_MAX]

    getcontext().prec = length - 1
    result = Decimal(random.gauss(mu, sigma))
    while (result > max or result < min):
      result = Decimal(random.gauss(mu, sigma))
    return result * Decimal(1)
  else:
    print 'Unsupported attribute distribution %s for float' % dist_def[DIST_NAME]
    sys.exit(2)

class SequenceGenerator:
  count = 0

  @classmethod
  def next_value(self):
    SequenceGenerator.count += 1
    return SequenceGenerator.count

if __name__ == '__main__':
  import sys, json
  if not len(sys.argv) == 4:
    print "data_generator.py <schema json file> <output csv file> <# of records>"
    sys.exit(2)

  schema = json.load(open(sys.argv[1]))
  output = sys.argv[2]
  nrecords = int(sys.argv[3])

  generate_data(schema, output, nrecords)

