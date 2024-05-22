#! /bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <commit_message>"
    exit 1
fi

git add .

git commit -m "$1"

git push 
