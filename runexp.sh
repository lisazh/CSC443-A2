#!/bin/bash

echo "Testing performance of msort with different file sizes"
./msort newschema.json smalldata smalldatamsort 1000 5 student_number 

./msort newschema.json meddata meddatamsort 1000 5 student_number

./msort newschema.json bigdata bigdatamsort 1000 5 student_number

echo "Testing performance of msort with different memory capacities"

./msort newschema.json meddata smallmemmsort 350 5 student_number 

./msort newschema.json meddata mid1memmsort 700 5 student_number

./msort newschema.json meddata mid2memmsort 2800 5 student_number

./msort newschema.json meddata bigmemmsort 22400 5 student_number

./msort newschema.json meddata reallybigmemmsort 44800 5 student_number

echo "Testing performance of msort with different values of k"

./msort newschema.json meddata smallkmsort 6000 2 student_number 

./msort newschema.json meddata mid1kmsort 6000 5 student_number

./msort newschema.json meddata mid2kmsort 6000 10 student_number

./msort newschema.json meddata mid3kmsort 6000 50 student_number

./msort newschema.json meddata bigkmsort 6000 100 student_number

echo "Testing performance of bsort with different file sizes"

./bsort newschema.json smalldata smalldatabsort student_number

./bsort newschema.json meddata meddatabsort student_number

./bsort newschema.json bigdata bigdatabsort student_number

