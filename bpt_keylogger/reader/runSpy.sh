#! /bin/bash

taskset -c 2 ./spy
python filter.py
taskset -c 2 ./spy
while [ $? -eq 42 ]; do
    python filter.py
    taskset -c 2 ./spy
done