#!/bin/bash

./msort newschema.json smalldata smalldatamsort 1000 5 student_number 

./msort newschema.json meddata meddatamsort 1000 5 student_number

./msort newschema.json bigdata bigdatamsort 1000 5 student_number