# x509-parser

The purpose of the code in this repo is to read the Common Name from an x509 leaf certificate in an embedded device where openssl and wolfssl are not available. This code will be used in a [custom hsm](https://docs.microsoft.com/en-us/azure/iot-dps/use-hsm-with-sdk) for a port of the [Azure IoT SDK C](https://github.com/Azure/azure-iot-sdk-c) to enable connecting through Azure [Device Provisioning Service](https://docs.microsoft.com/en-us/azure/iot-dps/).

## Creating an x509 certificate chain

Before we tackle decoding x509 certificate we must first determine what type if x509 certificate are supported by Azure Device Provisioning. The best place to start is the [docs](https://docs.microsoft.com/en-us/azure/iot-dps/concepts-security). This will lead to an overview of [Device Authentication using X.509 CA Certificates](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-x509ca-overview) as well as [Conceptual understanding of X.509 CA certificates in the IoT industry](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-x509ca-concept) which together provide a great overview of what we are trying to build. Now we just need the details. Reading about [Authenticating devices signed with X.509 CA certificates](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-x509ca-overview#authenticating-devices-signed-with-x509-ca-certificates) we learn that the device must upload it's certificate chain for validation. This will become important later. Reading in [Enrollment group](https://docs.microsoft.com/en-us/azure/iot-dps/concepts-service#enrollment-group) we learn that the the enrollment group name and certificate name must be alphanumeric, lowercase, and may contain hyphens. Reading further in [individual enrollment](https://docs.microsoft.com/en-us/azure/iot-dps/concepts-service#individual-enrollment) we see the same requirement for device certificates. Presumably, CA certificates have the same requirement. There are no errors presented when uploading or verifying certificates to the Azure portal that hint at the above limitations, you just have to have read the docs. Reading further we finally stumble on to the section titled "[Digitally sign the verification code to create a verification certificate](https://docs.microsoft.com/en-us/azure/iot-dps/how-to-verify-certificates#digitally-sign-the-verification-code-to-create-a-verification-certificate)", here we learn that Microsoft has made [tools](https://github.com/Azure/azure-iot-sdk-c/tree/master/tools/CACertificates) available to help build certificate chains. Unfortunately they don't quite work and so we must try to build our own.

Creating a certificate chain is horribly complicated because not many people do it, so there is very little information available on the internet to help you out. I stumbled around quite a bit and where things really started to open up is when I realized I could dump certificates using the following command.

```bash
$ openssl x509 -in certificate.pem -text -noout
```

Using this together with the [man pages for openssl](https://www.openssl.org/docs/manmaster/man1/) and [phil's guide to openssl config files](https://www.phildev.net/ssl/opensslconf.html). I pieced together a script that will let you build signed certificate chains using openssl. Below is a sample set of commands to invoke the script to build a typical chain.

```bash
$ cd tools
$ ./x509.sh
Usage: createCA ca_base_name common_name
       createICA parent_ca_base_name ica_base_name common_name
       createDevice parent_ca_base_name device_name
       verify ca_base_name verification_code
       dump filename
$ ./x509.sh createCA sample-ca
Creating sample-ca.key.pem
Generating a 256 bit EC private key
writing new private key to 'sample-ca.key.pem'
-----
Signing sample-ca.cert.pem
$ ./x509.sh createICA sample-ca sample-ica
Generating a 256 bit EC private key
writing new private key to 'sample-ica.key.pem'
-----
Signature ok
subject=/CN=sample-ica
Getting CA Private Key
sample-ica.cert.pem: OK
$ ./x509.sh createICA sample-ica sample2-ica
Generating a 256 bit EC private key
writing new private key to 'sample2-ica.key.pem'
-----
Signature ok
subject=/CN=sample2-ica
Getting CA Private Key
sample2-ica.cert.pem: OK
$ ./x509.sh createDevice sample2-ica sample-device
Generating a 256 bit EC private key
writing new private key to 'sample-device.device.pkkey.pem'
-----
Signature ok
subject=/CN=sample-device
Getting CA Private Key
sample-device.device.cert.pem: OK
read EC key
writing EC key
$
```

Next I built a version of the Azure IoT SDK with a custom hsm. However it failed to register with the Device Provisioning Service.

```bash
$ docker build --target mine -t azure-iot-sdk-c-my-certs .
... snip build logs ...
$ docker run -ti --rm azure-iot-sdk-c-my-certs
% cd azure-iot-sdk-c/cmake/provisioning_client/
% cd samples/prov_dev_client_ll_sample/
% ./prov_dev_client_ll_sample
Provisioning API Version: 1.2.1
Iothub API Version: 1.2.1
-> 04:58:55 CONNECT | VER: 4 | KEEPALIVE: 0 | FLAGS: 130 | USERNAME: 0ne00012BA4/registrations/seank-sample/api-version=2017-11-15&ClientVersion=1.2.1 | CLEAN: 1
<- 04:58:55 CONNACK | SESSION_PRESENT: false | RETURN_CODE: 0x5
Error: Time:Tue Apr  3 04:58:55 2018 File:/src/azure-iot-sdk-c/provisioning_client/src/prov_transport_mqtt_common.c Func:mqtt_operation_complete_callback Line:198 Connection Not Accepted: 0x5: Not Authorized
Error: Time:Tue Apr  3 04:58:55 2018 File:/src/azure-iot-sdk-c/provisioning_client/src/prov_transport_mqtt_common.c Func:mqtt_error_callback Line:131 MQTT communication error
Error: Time:Tue Apr  3 04:58:55 2018 File:/src/azure-iot-sdk-c/provisioning_client/src/prov_device_ll_client.c Func:on_transport_registration_data Line:454 Failure retrieving data from the provisioning service
Failure encountered on registration!
registration failed!
Press any key to continue:
```

Next we need to examine certificates that are working with the Device Provisioning Service. The SDK provides a sample that uses the RIoT library for Microsoft Research. So lets build the sample and run it.

```bash
$ docker build --target dice -t azure-iot-sdk-c-dice .
... snip build logs ...
$ docker run -ti --rm azure-iot-sdk-c-dice
% cd azure-iot-sdk-c/cmake/provisioning_client/
% cd tools/dice_device_enrollment/
% ./dice_device_enrollment
Would you like to do Individual (i) or Group (g) enrollments: g

root cert:
-----BEGIN CERTIFICATE-----
MIIBfTCCASSgAwIBAgIFGis8TV4wCgYIKoZIzj0EAwIwNDESMBAGA1UEAww
JcmlvdC1yb290MQswCQYDVQQGDAJVUzERMA8GA1UECgwITVNSX1RFU1QwHh
...
```

Cutting and pasting the certificates into .pem files, we can use the openssl command above to dump their properties.

```
$ ./x509.sh dump ../azure-iot-sdk/riot-root.pem
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 112394521950 (0x1a2b3c4d5e)
    Signature Algorithm: ecdsa-with-SHA256
        Issuer: CN=riot-root, C=US, O=MSR_TEST
        Validity
            Not Before: Jan  1 00:00:00 2017 GMT
            Not After : Jan  1 00:00:00 2037 GMT
        Subject: CN=riot-root, C=US, O=MSR_TEST
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:69:ab:5a:26:a1:52:0f:c9:ec:5d:a5:95:f4:97:
                    48:b9:fe:d2:3d:09:c5:9d:1f:a7:50:d3:94:49:c8:
                    fc:9c:eb:df:1e:5b:b0:fd:9e:46:9a:63:d2:37:e5:
                    04:fb:d4:fa:a2:70:4f:b1:05:9e:ad:9a:d5:3c:f8:
                    39:62:b1:ce:7d
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        X509v3 extensions:
            X509v3 Key Usage:
                Certificate Sign
            X509v3 Basic Constraints: critical
                CA:TRUE, pathlen:1
    Signature Algorithm: ecdsa-with-SHA256
         30:44:02:20:51:5c:3d:6e:b9:e3:96:b9:04:d3:fe:ca:7f:54:
         fd:cd:0c:c1:e9:97:bf:37:5d:ca:51:5a:d0:a6:c3:b4:03:5f:
         02:20:74:af:10:51:2c:fd:5e:ee:f6:6d:ce:32:6c:2c:fe:a7:
         8e:33:ba:a9:6a:00:ad:74:6e:4d:5d:a7:0c:ed:d2:f6
$ ./x509.sh dump sample-ca.cert.pem
Certificate:
   Data:
       Version: 3 (0x2)
       Serial Number: 12313715850152712389 (0xaae31b53297318c5)
   Signature Algorithm: ecdsa-with-SHA256
       Issuer: CN=sample-ca
       Validity
           Not Before: Apr  3 01:57:57 2018 GMT
           Not After : Mar 31 01:57:57 2028 GMT
       Subject: CN=sample-ca
       Subject Public Key Info:
           Public Key Algorithm: id-ecPublicKey
               Public-Key: (256 bit)
               pub:
                   04:d8:93:bf:bd:02:ab:06:11:b4:e8:97:67:00:df:
                   aa:82:fb:ad:f1:2a:2e:b9:bb:52:38:c7:2c:e6:1f:
                   f5:53:bf:3f:0d:72:02:54:de:94:00:14:10:7e:0c:
                   8b:34:7d:36:79:e3:02:a0:67:7d:10:39:10:9d:d6:
                   25:cf:82:f0:ea
               ASN1 OID: prime256v1
               NIST CURVE: P-256
       X509v3 extensions:
           X509v3 Subject Key Identifier:
               F5:1F:02:2E:80:98:02:02:FB:DF:1E:F0:1A:66:4A:51:47:BE:82:A2
           X509v3 Authority Key Identifier:
               keyid:F5:1F:02:2E:80:98:02:02:FB:DF:1E:F0:1A:66:4A:51:47:BE:82:A2

           X509v3 Key Usage:
               Certificate Sign
           X509v3 Basic Constraints: critical
               CA:TRUE, pathlen:1
   Signature Algorithm: ecdsa-with-SHA256
        30:45:02:20:1c:14:75:f3:76:d8:48:a9:53:1b:54:e5:c7:e7:
        d2:29:d3:ad:6f:68:29:34:cc:78:7e:bd:21:68:7a:6e:27:04:
        02:21:00:bf:d6:94:36:02:02:2b:02:d0:50:57:fa:62:9c:24:
        92:d7:1c:e6:4b:29:9d:fe:9e:0c:97:d7:0d:ee:6a:2c:1a
```

The only difference between the root CA from the dice sample and the root CA I can produce with Openssl is two additional X509v3 extensions,  ```X509v3 Subject Key Identifier``` and ```X509v3 Authority Key Identifier``` Openssl won't let me remove these without issuing the following error when I try to sign an intermediate CA with this CA.

```bash
140735603557256:error:2207707B:X509 V3 routines:V2I_AUTHORITY_KEYID:unable to get issuer keyid:/BuildRoot/Library/Caches/com.apple.xbs/Sources/libressl/libressl-22/libressl/crypto/x509v3/v3_akey.c:171:
140735603557256:error:22098080:X509 V3 routines:X509V3_EXT_nconf:error in extension:/BuildRoot/Library/Caches/com.apple.xbs/Sources/libressl/libressl-22/libressl/crypto/x509v3/v3_conf.c:97:name=authorityKeyIdentifier, value=keyid:always,issuer
unable to load certificate
140735603557256:error:0906D06C:PEM routines:PEM_read_bio:no start line:/BuildRoot/Library/Caches/com.apple.xbs/Sources/libressl/libressl-22/libressl/crypto/pem/pem_lib.c:704:Expecting: TRUSTED CERTIFICATE
```

Finally, just to make sure we aren't going insane. Lets build a custom hsm using the certificates from the RIoT library

```bash
$ docker build --target azure -t azure-iot-sdk-c-azure-certs .
... snip build logs ...
$ docker run -ti --rm azure-iot-sdk-c-azure-certs
% cd azure-iot-sdk-c/cmake/provisioning_client/
% cd samples/prov_dev_client_ll_sample/
% ./prov_dev_client_ll_sample
Provisioning API Version: 1.2.1
Iothub API Version: 1.2.1
-> 05:17:58 CONNECT | VER: 4 | KEEPALIVE: 0 | FLAGS: 130 | USERNAME: 0ne00012BA4/registrations/riot-device-cert/api-version=2017-11-15&ClientVersion=1.2.1 | CLEAN: 1
<- 05:17:58 CONNACK | SESSION_PRESENT: false | RETURN_CODE: 0x0
-> 05:17:58 SUBSCRIBE | PACKET_ID: 1 | TOPIC_NAME: $dps/registrations/res/# | QOS: 1
Provisioning Status: PROV_DEVICE_REG_STATUS_CONNECTED
<- 05:17:58 SUBACK | PACKET_ID: 1 | RETURN_CODE: 1
-> 05:17:58 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/PUT/iotdps-register/?$rid=1
<- 05:18:04 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=1 | PACKET_ID: 2 | PAYLOAD_LEN: 94
Provisioning Status: PROV_DEVICE_REG_STATUS_ASSIGNING
-> 05:18:04 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/GET/iotdps-get-operationstatus/?$rid=2&operationId=2.87f7851aaaa18ed1.c013a2cd-b9ca-4f3f-9801-b1c881a03fc1
<- 05:18:04 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=2 | PACKET_ID: 2 | PAYLOAD_LEN: 173
Provisioning Status: PROV_DEVICE_REG_STATUS_ASSIGNING
-> 05:18:07 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_MOST_ONCE | TOPIC_NAME: $dps/registrations/GET/iotdps-get-operationstatus/?$rid=3&operationId=2.87f7851aaaa18ed1.c013a2cd-b9ca-4f3f-9801-b1c881a03fc1
<- 05:18:07 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE = 0x01 | TOPIC_NAME: $dps/registrations/res/200/?$rid=3 | PACKET_ID: 2 | PAYLOAD_LEN: 506
Registration Information received from service: Kohler-Test.azure-devices.net!
-> 05:18:07 DISCONNECT
Sending 1 messages to IoTHub every 2 seconds for 2 messages (Send any message to stop)
-> 05:18:07 CONNECT | VER: 4 | KEEPALIVE: 240 | FLAGS: 128 | USERNAME: Kohler-Test.azure-devices.net/riot-device-cert/api-version=2016-11-14&DeviceClientType=iothubclient%2f1.2.1%20(native%3b%20Linux%3b%20x86_64) | CLEAN: 0
<- 05:18:07 CONNACK | SESSION_PRESENT: true | RETURN_CODE: 0x0
-> 05:18:07 SUBSCRIBE | PACKET_ID: 2 | TOPIC_NAME: devices/riot-device-cert/messages/devicebound/# | QOS: 1
<- 05:18:07 SUBACK | PACKET_ID: 2 | RETURN_CODE: 1
IoTHubClient_LL_SendEventAsync accepted message [1] for transmission to IoT Hub.
-> 05:18:10 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE | TOPIC_NAME: devices/riot-device-cert/messages/events/ | PACKET_ID: 3 | PAYLOAD_LEN: 25
<- 05:18:12 PUBACK | PACKET_ID: 3
IoTHubClient_LL_SendEventAsync accepted message [2] for transmission to IoT Hub.
-> 05:18:13 PUBLISH | IS_DUP: false | RETAIN: 0 | QOS: DELIVER_AT_LEAST_ONCE | TOPIC_NAME: devices/riot-device-cert/messages/events/ | PACKET_ID: 4 | PAYLOAD_LEN: 25
-> 05:18:13 DISCONNECT
Press any key to continue:
```

[](https://nostdahl.com/2017/08/11/x-509-certificates-explained/)
