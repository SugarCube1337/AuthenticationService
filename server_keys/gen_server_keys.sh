# gen CA's priv_key and self-signed cert
openssl req -x509 -newkey rsa:4096 -days 365 -nodes -keyout ca-key.pem -out ca-cert.pem -subj "/C=ct/ST=testst/L=testl/O=testo/OU=testou/CN=testcn/emailAddress=t.t@test.ts"

echo "CA's self-signed certificate"
openssl x509 -in ca-cert.pem -noout -text

# generate server's priv_key & cert signing request (CSR)
openssl req -newkey rsa:4096 -nodes -keyout server-key.pem -out server-req.pem -subj "/C=ct/ST=testst/L=testl/O=testo/OU=testou/CN=testcn/emailAddress=t.t@test.ts"

# create Subject alternative Name (SAN) with localhost address. In production it should be domain name or set your IP
echo subjectAltName = IP:127.0.0.1 > server-ext.cnf

# use CA's priv_key to sign server's CSR and get back signed cert
openssl x509 -req -in server-req.pem -days 180 -CA ca-cert.pem -CAkey ca-key.pem -CAcreateserial -out server-cert.pem -extfile server-ext.cnf

echo "Server's signed certificate"
openssl x509 -in server-cert.pem -noout -text
