#!/bin/bash

echo "Testing performance of bsort with different file sizes"

./bsort newschema.json smalldata smalldatabsort student_number

./bsort newschema.json meddata meddatabsort student_number

./bsort newschema.json bigdata bigdatabsort student_number

echo "Testing performance of msort with poorly chosen parameters against different file sizes"

./msort newschema.json smalldata smallpoormsort 6000 100 student_number 

./msort newschema.json meddata medpoormsort 6000 100 student_number

./msort newschema.json bigdata bigpoormsort 6000 100 student_number

echo "Testing performance of msort with well tuned parameters against different file sizes"
 
./msort newschema.json smalldata smallwellmsort 6000 100 student_number 

./msort newschema.json meddata medwellmsort 6000 100 student_number

./msort newschema.json bigdata bigwellmsort 6000 100 student_number 