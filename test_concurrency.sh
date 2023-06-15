#!/bin/bash

./fget INFO newfile.txt & ./fget RM newfolder/test & ./fget GET newfile.txt get_op.txt & ./fget MD newfolder/test_concurrent & ./fget PUT put_op.txt README.txt

