#!/bin/bash

# List of users to generate keys for
users=("BC" "Alice" "Bob" "Charlie" "David" "Emma" "Fred" "Ginger" "Henry" "Iris" "Seq")

# Generate keys for each user
for user in "${users[@]}"
do
	echo "Generating keys for user: $user"
	
	# Create private and public key according to lab08, and convert it to an easier format to handle in java
	openssl genrsa -out private.pem 2048
	openssl pkcs8 -topk8 -inform PEM -outform DER -in private.pem -out private.der -nocrypt
	openssl rsa -in private.pem -pubout -outform DER -out public.der  

	# Special edge case for the sequencer's key pair
	if [ "$user" == "Seq" ] ; then
		echo "Generating sequencer key"
		mkdir -p sequencer/src/main/resources
		mv private.der sequencer/src/main/resources/$user.priv

		# "seq" directory used to diferentiate between a "Seq" user
		mkdir -p node/src/main/resources/seq
		mv public.der node/src/main/resources/seq/$user.pub
	else
		mkdir -p client/src/main/resources
		mkdir -p node/src/main/resources
		mv private.der client/src/main/resources/$user.priv
		mv public.der node/src/main/resources/$user.pub
	fi

	# Cleanup leftover file
	rm private.pem
done

echo "All keys generated successfully!"