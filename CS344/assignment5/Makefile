all: enc_server enc_client dec_server dec_client keygen

enc_server:
	gcc --std=gnu99 -o enc_server enc_server.c

enc_client:
	gcc --std=gnu99 -o enc_client enc_client.c

dec_server:
	gcc --std=gnu99 -o dec_server dec_server.c

dec_client:
	gcc --std=gnu99 -o dec_client dec_client.c

keygen:
	gcc --std=gnu99 -o keygen keygen.c

clean:
	rm -f ./mytestresults ./enc_server ./enc_client ./dec_server ./dec_client ./keygen