#!/bin/bash
cd test
for file in `find . -type f ! -name "*.*"`
do
	cd ..
	OUTPUT=$(./test.sh $file)
	cd test
	if ! [[ "$OUTPUT" =~ "OK" ]]
	then
		echo $file " " $OUTPUT
	fi
	
done
cd ..
