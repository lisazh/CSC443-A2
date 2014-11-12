#!/bin/bash

echo "Testing performance of msort with different file sizes"
./msort newschema.json smalldata smalldatamsort 1000 5 student_number 

./msort newschema.json meddata meddatamsort 1000 5 student_number

#./msort newschema.json bigdata bigdatamsort 1000 5 student_number

echo "Testing performance of msort with different memory capacities"

./msort newschema.json meddata smallmemmsort 350 5 student_number 

./msort newschema.json meddata mid1memmsort 700 5 student_number

./msort newschema.json meddata mid2memmsort 2800 5 student_number

./msort newschema.json meddata bigmemmsort 22400 5 student_number
