# x509-parser

The purpose of the code in this repo is to read the Common Name from an x509 leaf certificate in an embedded device where openssl and wolfssl are not available. This code will be used in a [custom hsm](https://docs.microsoft.com/en-us/azure/iot-dps/use-hsm-with-sdk) for a port of the [Azure IoT SDK C](https://github.com/Azure/azure-iot-sdk-c) to enable connecting through Azure [Device Provisioning Service](https://docs.microsoft.com/en-us/azure/iot-dps/).

## Building

The code in this repo is built in 3 places. The x509.sh script is interpreted by bash. The code to test reading the x509 certificates (x509test) can be compiled locally on a mac or linux environment using make (it will compile on windows, but setting up the environment and project is left as an exercise for the reader). The code for building the Azure IoT SDK C with the custom hsm is done on linux in a docker container

### Building x509test

x509test can be built using the following commmands in the root of this project

```bash
$ cd src
$ make
```

### Building the custom hsm with Azure IoT SDK C

building the linux container with the custom hsm and the Azure IoT SDK C can be done issuing the following command from the root of this project

```bash
$ docker build -t azure-iot-sdk-c .
```

### Running the x509.sh script

Running the script without parameters will give you a quick overview of the funnctions.

```bash
$ ./x509.sh
Usage: createCA ca_base_name common_name
       createICA parent_ca_base_name ica_base_name common_name
       createDevice parent_ca_base_name device_name
       verify ca_base_name verification_code
       dump filename
$
```

To create a self-signed root CA use the ```createCA``` command.

```bash
$ ./x509.sh createCA seank-root-ca
Creating seank-root-ca.key.pem
Generating a 256 bit EC private key
writing new private key to 'seank-root-ca.key.pem'
-----
Signing seank-root-ca.cert.pem
$
```

To create an IntermediateCA use the ```createICA``` command providing the CA or ICA parent that should sign the ICA.

```bash
$ ./x509.sh createICA seank-root-ca product-ica
Generating a 256 bit EC private key
writing new private key to 'product-ica.key.pem'
-----
Signature ok
subject=/CN=product-ica
Getting CA Private Key
product-ica.cert.pem: OK
$ ./x509.sh createICA product-ica sku-ica
Generating a 256 bit EC private key
writing new private key to 'sku-ica.key.pem'
-----
Signature ok
subject=/CN=sku-ica
Getting CA Private Key
sku-ica.cert.pem: OK
$ ./x509.sh createICA sku-ica factory-ica
Generating a 256 bit EC private key
writing new private key to 'factory-ica.key.pem'
-----
Signature ok
subject=/CN=factory-ica
Getting CA Private Key
factory-ica.cert.pem: OK
$ ./x509.sh createICA factory-ica contractor-ica
Generating a 256 bit EC private key
writing new private key to 'contractor-ica.key.pem'
-----
Signature ok
subject=/CN=contractor-ica
Getting CA Private Key
contractor-ica.cert.pem: OK
```

To create a device certificate use the ```createDevice``` command.

```bash
$ ./x509.sh createDevice contractor-ica seank-test
Generating a 256 bit EC private key
writing new private key to 'seank-test.device.pkkey.pem'
-----
Signature ok
subject=/CN=seank-test
Getting CA Private Key
seank-test.device.cert.pem: OK
read EC key
writing EC key
$
```

When you upload your CA to the Certificates blade of your Device Provisioning Service, you can use the ```verify``` command to create the verification certificate.

```bash
$ ./x509.sh verify seank-root-ca FAD8070B370957F5A061695EFE8835ADBA04C228A1C6FB3E
Generating a 256 bit EC private key
writing new private key to 'verification.pkkey.pem'
-----
Signature ok
subject=/CN=FAD8070B370957F5A061695EFE8835ADBA04C228A1C6FB3E
Getting CA Private Key
verification.cert.pem: OK
~/Development/x509-parser/tools (master *% u=)
$
```

If you are curious about what is in one of your certificates, you can use the ```dump``` command to see.

```bash
$ ./x509.sh dump seank-root-ca.cert.pem
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 11734911264884930702 (0xa2dac79f0632388e)
    Signature Algorithm: ecdsa-with-SHA256
        Issuer: CN=seank-root-ca
        Validity
            Not Before: Apr  5 23:47:49 2018 GMT
            Not After : Apr  2 23:47:49 2028 GMT
        Subject: CN=seank-root-ca
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:6c:3c:1b:a4:7e:0f:7b:6d:74:cc:d5:69:0f:4d:
                    e8:49:0d:78:86:d1:44:b6:09:22:46:d7:f8:4c:9f:
                    7f:2e:38:eb:95:da:fe:9a:a4:8a:7f:e1:6b:35:d8:
                    dc:01:9c:59:c8:45:4e:52:b7:ff:5c:fd:95:fd:60:
                    98:85:e7:c0:a1
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        X509v3 extensions:
            X509v3 Key Usage:
                Certificate Sign
            X509v3 Basic Constraints: critical
                CA:TRUE
    Signature Algorithm: ecdsa-with-SHA256
         30:46:02:21:00:90:09:ac:bf:61:8a:a4:3f:65:fa:da:83:98:
         c6:de:1d:d9:fe:74:de:dd:3f:1f:9b:8a:f1:59:f2:b2:33:81:
         98:02:21:00:dd:f5:34:62:b7:26:21:1b:85:c9:0f:69:8a:1f:
         57:76:47:1d:56:11:2c:42:2d:2c:4b:46:4a:67:97:0a:a0:c1
$
```

### Testing your Certificates

To test your certificates, open the custom_hsm/custom_hsm.c file and copy the contents of your XXXXXXX.device.chain.cert.pem and XXXXXXX.device.key.pem into the static strings at the top. Next, update the sed command in the Dockerfile to replace ```[ID Scope]``` with the scope id of your Device PRovisioning Service. Save and then run the following command from a terminal to test.

```bash
$ docker build -t azure-iot-sdk-c .
     ... snip build log ...
$ docker run -it --rm azure-iot-sdk-c
% cd azure-iot-sdk-c/cmake/provisioning_client/
% cd samples/prov_dev_client_ll_sample/
% ./prov_dev_client_ll_sample
Provisioning API Version: 1.2.1
Iothub API Version: 1.2.1
-> 02:07:48 CONNECT | VER: 4 | KEEPALIVE: 0 | FLAGS: 130 | USERNAME: 0ne00012BA4/registrations/seank-test/api-version=2017-11-15&ClientVersion=1.2.1 | CLEAN: 1
<- 02:07:48 CONNACK | SESSION_PRESENT: false | RETURN_CODE: 0x0
-> 02:07:48 SUBSCRIBE | PACKET_ID: 1 | TOPIC_NAME: $dps/registrations/res/# | QOS: 1
Provisioning Status: PROV_DEVICE_REG_STATUS_CONNECTED
<- 02:07:48 SUBACK | PACKET_ID: 1 | RETURN_CODE: 1
-> 02:07:48 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/PUT/iotdps-register/?$rid=1
<- 02:07:49 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=1 | PACKET_ID: 2 | PAYLOAD_LEN: 94
Provisioning Status: PROV_DEVICE_REG_STATUS_ASSIGNING
-> 02:07:49 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/GET/iotdps-get-operationstatus/?$rid=2&operationId=2.13bad1d1ad8d0768.e97cbac0-c81a-49f3-8932-37a000f39a91
<- 02:07:49 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=2 | PACKET_ID: 2 | PAYLOAD_LEN: 167
Provisioning Status: PROV_DEVICE_REG_STATUS_ASSIGNING
-> 02:07:52 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/GET/iotdps-get-operationstatus/?$rid=3&operationId=2.13bad1d1ad8d0768.e97cbac0-c81a-49f3-8932-37a000f39a91
<- 02:07:52 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=3 | PACKET_ID: 2 | PAYLOAD_LEN: 485
Registration Information received from service: Kohler-Test.azure-devices.net!
-> 02:07:52 DISCONNECT
Sending 1 messages to IoTHub every 2 seconds for 2 messages (Send any message to stop)
-> 02:07:52 CONNECT | VER: 4 | KEEPALIVE: 240 | FLAGS: 128 | USERNAME: Kohler-Test.azure-devices.net/seank-test/api-version=2016-11-14&DeviceClientType=iothubclient%2f1.2.1%20(native%3b%20Linux%3b%20x86_64) | CLEAN: 0
<- 02:07:52 CONNACK | SESSION_PRESENT: true | RETURN_CODE: 0x0
-> 02:07:52 SUBSCRIBE | PACKET_ID: 2 | TOPIC_NAME: devices/seank-test/messages/devicebound/# | QOS: 1
<- 02:07:52 SUBACK | PACKET_ID: 2 | RETURN_CODE: 1
IoTHubClient_LL_SendEventAsync accepted message [1] for transmission to IoT Hub.
-> 02:07:55 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE | TOPIC_NAME: devices/seank-test/messages/events/ | PACKET_ID: 3 | PAYLOAD_LEN: 25
<- 02:07:55 PUBACK | PACKET_ID: 3
IoTHubClient_LL_SendEventAsync accepted message [2] for transmission to IoT Hub.
-> 02:07:58 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE | TOPIC_NAME: devices/seank-test/messages/events/ | PACKET_ID: 4 | PAYLOAD_LEN: 25
-> 02:07:58 DISCONNECT
Press any key to continue:

%
```
