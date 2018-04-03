// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MSR

#include "hsm_client_data.h"

typedef struct CUSTOM_HSM_INFO_TAG
{
    int info;
} CUSTOM_HSM_INFO;

HSM_CLIENT_HANDLE custom_hsm_create()
{
    CUSTOM_HSM_INFO* result;
    result = malloc(sizeof(CUSTOM_HSM_INFO));
    if (result == NULL)
    {
        (void)printf("Failure: malloc CUSTOM_HSM_INFO.");
    }
    else
    {
        memset(result, 0, sizeof(CUSTOM_HSM_INFO));
    }
    return (HSM_CLIENT_HANDLE)result;
}

void custom_hsm_destroy(HSM_CLIENT_HANDLE handle)
{
    if (handle != NULL)
    {
        CUSTOM_HSM_INFO* hsm_impl = (CUSTOM_HSM_INFO*)handle;
        free(hsm_impl);
    }
}

int hsm_client_x509_init()
{
    // Add any code needed to initialize the x509 module
    return 0;
}

void hsm_client_x509_deinit()
{
}

int hsm_client_tpm_init()
{
    // Add any code needed to initialize the TPM module
    return 0;
}

void hsm_client_tpm_deinit()
{
}

// Return the X509 certificate in PEM format
char* custom_hsm_get_certificate(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* cust_hsm = (CUSTOM_HSM_INFO*)handle;
        cust_hsm->info++;

        const char* cert =
#ifdef MSR
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIICIDCCAcagAwIBAgIFCgsMDQ4wCgYIKoZIzj0EAwIwOzEZMBcGA1UEAww\r\n"
          "QcmlvdC1zaWduZXItY29yZTELMAkGA1UEBgwCVVMxETAPBgNVBAoMCE1TUl\r\n"
          "9URVNUMB4XDTE3MDEwMTAwMDAwMFoXDTM3MDEwMTAwMDAwMFowOzEZMBcGA\r\n"
          "1UEAwwQcmlvdC1kZXZpY2UtY2VydDELMAkGA1UEBgwCVVMxETAPBgNVBAoM\r\n"
          "CE1TUl9URVNUMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEXl8k/JM5Trr\r\n"
          "d+T2hCzH8J8FzdMo+nqEBctNE0wUQWesM+QbsF1PBmEHxb/FpqegLlleJW4\r\n"
          "niMxaN8z3J7T/imKOBtjCBszATBgNVHSUEDDAKBggrBgEFBQcDAjCBmwYGZ\r\n"
          "4EFBQQBBIGQMIGNAgEBMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEdgvx\r\n"
          "WvX6LURj6RDfdXUY30+4hz8gZ6cf25VifQhb3wPuuZTxr09nLslUYdF42qA\r\n"
          "6VsIUj1fym6mLv2K6agLvvTAtBglghkgBZQMEAgEEIGvpsYTJN8KOEi7uUS\r\n"
          "to6o4Aw90VnqToXoTLqWb0Rs1OMAoGCCqGSM49BAMCA0gAMEUCIFFcPW654\r\n"
          "5a5BNP+yn9U/c0MwemXvzddylFa0KbDtANfAiEAuqG9PPKcd3Fntf5c2icX\r\n"
          "fIFRXtSWOpZcR8iNnFwtN5g=\r\n"
          "-----END CERTIFICATE-----\r\n"
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIIBhTCCASugAwIBAgIFDg0MCwowCgYIKoZIzj0EAwIwNDESMBAGA1UEAww\r\n"
          "JcmlvdC1yb290MQswCQYDVQQGDAJVUzERMA8GA1UECgwITVNSX1RFU1QwHh\r\n"
          "cNMTcwMTAxMDAwMDAwWhcNMzcwMTAxMDAwMDAwWjA7MRkwFwYDVQQDDBBya\r\n"
          "W90LXNpZ25lci1jb3JlMQswCQYDVQQGDAJVUzERMA8GA1UECgwITVNSX1RF\r\n"
          "U1QwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAAR2C/Fa9fotRGPpEN91dRj\r\n"
          "fT7iHPyBnpx/blWJ9CFvfA+65lPGvT2cuyVRh0XjaoDpWwhSPV/KbqYu/Yr\r\n"
          "pqAu+9oyMwITALBgNVHQ8EBAMCAAQwEgYDVR0TAQH/BAgwBgEB/wIBATAKB\r\n"
          "ggqhkjOPQQDAgNIADBFAiBRXD1uueOWuQTT/sp/VP3NDMHpl783XcpRWtCm\r\n"
          "w7QDXwIhAKOszfUHSFg/pp+MMtaHk9msAxWOR+00HJy7V4pv/MiO\r\n"
          "-----END CERTIFICATE-----\r\n";
#else
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIIBvjCCAWOgAwIBAgIJAP8jKiMxIzK3MAoGCCqGSM49BAMCMB0xGzAZBgNVBAMM\r\n"
          "EnNhbXBsZS1mYWN0b3J5LWljYTAeFw0xODA0MDIyMzE3NDdaFw0yODAzMzAyMzE3\r\n"
          "NDdaMBcxFTATBgNVBAMMDHNlYW5rLXNhbXBsZTBZMBMGByqGSM49AgEGCCqGSM49\r\n"
          "AwEHA0IABLNkK763FXer/wX/Hvn1ySxC5t7LsAHdxdrqXEQlTiXvtw/Yog6EGnLk\r\n"
          "B3jEuAJYsaRuoY9YFq1sTyZsBrA7uR6jgZEwgY4wHQYDVR0OBBYEFEqJEZ8asHmX\r\n"
          "oj/lbXxkADupbuwEMEsGA1UdIwREMEKAFO4RdMYYPCDSV8jpWO+anPF9+fA5oR+k\r\n"
          "HTAbMRkwFwYDVQQDDBBkZW1vLXByb2R1Y3QtaWNhggkAmHws32VsRgcwCwYDVR0P\r\n"
          "BAQDAgWgMBMGA1UdJQQMMAoGCCsGAQUFBwMCMAoGCCqGSM49BAMCA0kAMEYCIQCY\r\n"
          "S9wDIdgFK6rUJuqRDOYEenfwBr4KKeB3CKgW07iupQIhAOTMQfQx4JUTFK34bbhe\r\n"
          "CU7HVZX0i+ecNmzVMERNPnD/\r\n"
          "-----END CERTIFICATE-----\r\n"
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIIBkjCCATigAwIBAgIJAJh8LN9lbEYHMAoGCCqGSM49BAMCMBsxGTAXBgNVBAMM\r\n"
          "EGRlbW8tcHJvZHVjdC1pY2EwHhcNMTgwNDAyMjMxNzE1WhcNMjgwMzMwMjMxNzE1\r\n"
          "WjAdMRswGQYDVQQDDBJzYW1wbGUtZmFjdG9yeS1pY2EwWTATBgcqhkjOPQIBBggq\r\n"
          "hkjOPQMBBwNCAAQa05c77NGwOsaV1JBP1QvFye/OtfnUFRr7tZ1PtV8vMOQROPfT\r\n"
          "odbyVsGAtT5Y12iO6rI/F1XVIyQaNFZxd6Ebo2MwYTAdBgNVHQ4EFgQU7hF0xhg8\r\n"
          "INJXyOlY75qc8X358DkwHwYDVR0jBBgwFoAUg6y8T2+dic9IiRJe0exAyW7brOow\r\n"
          "CwYDVR0PBAQDAgIEMBIGA1UdEwEB/wQIMAYBAf8CAQEwCgYIKoZIzj0EAwIDSAAw\r\n"
          "RQIgQhpWYnpyAO5GwloPLK9/bm9YMgt77olJ1nQz0S4KcjECIQCHKNM6DfILLbrk\r\n"
          "GrRyFd0HGwMSPVMkKU9+NGYuAOvo2g==\r\n"
          "-----END CERTIFICATE-----\r\n"
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIIBjTCCATOgAwIBAgIJAJXjc3ZoTU/9MAoGCCqGSM49BAMCMBgxFjAUBgNVBAMM\r\n"
          "DXNlYW5rLXJvb3QtY2EwHhcNMTgwNDAyMjMxNzAwWhcNMjgwMzMwMjMxNzAwWjAb\r\n"
          "MRkwFwYDVQQDDBBkZW1vLXByb2R1Y3QtaWNhMFkwEwYHKoZIzj0CAQYIKoZIzj0D\r\n"
          "AQcDQgAEgW25GUiOxJKZUNb1Q+m2poNS3NvwW6IHmu4KoDpJOU7O6qxZukr6LecW\r\n"
          "xEjSr9NrBlt7T6uO2ja3pEF9qZuBEKNjMGEwHQYDVR0OBBYEFIOsvE9vnYnPSIkS\r\n"
          "XtHsQMlu26zqMB8GA1UdIwQYMBaAFO9mFEq1fwiYgEB2OTwYngxIkab1MAsGA1Ud\r\n"
          "DwQEAwICBDASBgNVHRMBAf8ECDAGAQH/AgEBMAoGCCqGSM49BAMCA0gAMEUCIH+R\r\n"
          "yTiFEUf7VbCu8TFkRhcezvKptHkOS6OtkOor35cfAiEAwLV6j7t4rqHlFxg7o08i\r\n"
          "GyVFW7Iu5Ir1SZ2+4HEp2Nw=\r\n"
          "-----END CERTIFICATE-----\r\n"
          "-----BEGIN CERTIFICATE-----\r\n"
          "MIIBijCCATCgAwIBAgIJAJ/xeEIhc7RCMAoGCCqGSM49BAMCMBgxFjAUBgNVBAMM\r\n"
          "DXNlYW5rLXJvb3QtY2EwHhcNMTgwNDAyMjMxNjQzWhcNMjgwMzMwMjMxNjQzWjAY\r\n"
          "MRYwFAYDVQQDDA1zZWFuay1yb290LWNhMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcD\r\n"
          "QgAEbll3j/gbznswO//QyMjoRwW8xpmoKi7h7r9SAV6Ltj1BQ1h2u6fApGFVK0jz\r\n"
          "iLxxW+no27bBjR2dhTCscnw9vKNjMGEwHQYDVR0OBBYEFO9mFEq1fwiYgEB2OTwY\r\n"
          "ngxIkab1MB8GA1UdIwQYMBaAFO9mFEq1fwiYgEB2OTwYngxIkab1MAsGA1UdDwQE\r\n"
          "AwICBDASBgNVHRMBAf8ECDAGAQH/AgEBMAoGCCqGSM49BAMCA0gAMEUCIQCrHCjI\r\n"
          "X9q6a7EjCELb349gHel9pHdAIZG+wALjwTqsQAIgfrQd0f8DxJK19sh4epAuRF0L\r\n"
          "DU0OIuKLhq8j+JOxYG8=\r\n"
          "-----END CERTIFICATE-----\r\n";
#endif
        if (cert == NULL)
        {
            (void)printf("Failure retrieving cert");
            result = NULL;
        }
        else
        {
            size_t length = strlen(cert);
            result = malloc(length + 1);
            if (result == NULL)
            {
                (void)printf("Failure allocating certifiicate");
            }
            else
            {
                strcpy(result, cert);
            }
        }
    }
    return result;
}

// Return Private Key of the Certification
char* custom_hsm_get_alias_key(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* cust_hsm = (CUSTOM_HSM_INFO*)handle;
        cust_hsm->info++;

        const char* private_key =
#ifdef MSR
          "-----BEGIN EC PRIVATE KEY-----\r\n"
          "MHcCAQEEINXd3WSS7LqDEFGdpbQi0V51w01XkpMVPUYlG9V7/ldUoAoGCCq\r\n"
          "GSM49AwEHoUQDQgAEXl8k/JM5Trrd+T2hCzH8J8FzdMo+nqEBctNE0wUQWe\r\n"
          "sM+QbsF1PBmEHxb/FpqegLlleJW4niMxaN8z3J7T/imA==\r\n"
          "-----END EC PRIVATE KEY-----\r\n";
#else
          "-----BEGIN EC PRIVATE KEY-----\r\n"
          "MHcCAQEEIA0Q2CpMECAHfNPPjUH+Y6gX+T8HUOwkdZ+KDcMiEd0NoAoGCCqGSM49\r\n"
          "AwEHoUQDQgAEs2QrvrcVd6v/Bf8e+fXJLELm3suwAd3F2upcRCVOJe+3D9iiDoQa\r\n"
          "cuQHeMS4AlixpG6hj1gWrWxPJmwGsDu5Hg==\r\n"
          "-----END EC PRIVATE KEY-----\r\n";
#endif

        if (private_key == NULL)
        {
            (void)printf("Failure retrieving private key");
            result = NULL;
        }
        else
        {
            size_t length = strlen(private_key);
            result = malloc(length + 1);
            if (result == NULL)
            {
                (void)printf("Failure allocating private key");
            }
            else
            {
                strcpy(result, private_key);
            }
        }
    }
    return result;
}

// Return allocated common name on the x509 certificate
char* custom_hsm_get_common_name(HSM_CLIENT_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        (void)printf("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        CUSTOM_HSM_INFO* cust_hsm = (CUSTOM_HSM_INFO*)handle;
        cust_hsm->info++;

        const char* common_name =
#ifdef MSR
          "riot-device-cert";
#else
          "seank-sample";
#endif

        if (common_name == NULL)
        {
            (void)printf("Failure retrieving common name");
            result = NULL;
        }
        else
        {
            size_t length = strlen(common_name);
            result = malloc(length + 1);
            if (result == NULL)
            {
                (void)printf("Failure allocating common name");
            }
            else
            {
                strcpy(result, common_name);
            }
        }
    }
    return result;
}

// TPM Custom Information handling
// Allocates the endorsement key using as key and the length as key_len
int custom_hsm_get_endorsement_key(HSM_CLIENT_HANDLE handle, unsigned char** key, size_t* key_len)
{
    // E_NOT_IMPL
    return 123456;
}

// Allocates the Storage Root key using as key and the length as key_len
int custom_hsm_get_storage_root_key(HSM_CLIENT_HANDLE handle, unsigned char** key, size_t* key_len)
{
  // E_NOT_IMPL
  return 123456;
}

// Decrypt and Stores the encrypted key
int custom_hsm_activate_id_key(HSM_CLIENT_HANDLE handle, const unsigned char* key, size_t key_len)
{
  // E_NOT_IMPL
  return 123456;
}

// Hashes value specified in data with the key stored in slot 1 and returns the result in signed_value
int custom_hsm_sign_with_identity(HSM_CLIENT_HANDLE handle, const unsigned char* data, size_t data_len, unsigned char** signed_value, size_t* signed_len)
{
  // E_NOT_IMPL
  return 123456;
}

static const HSM_CLIENT_X509_INTERFACE x509_interface =
{
    custom_hsm_create,
    custom_hsm_destroy,
    custom_hsm_get_certificate,
    custom_hsm_get_alias_key,
    custom_hsm_get_common_name
};

static const HSM_CLIENT_TPM_INTERFACE tpm_interface =
{
    custom_hsm_create,
    custom_hsm_destroy,
    custom_hsm_activate_id_key,
    custom_hsm_get_endorsement_key,
    custom_hsm_get_storage_root_key,
    custom_hsm_sign_with_identity
};

const HSM_CLIENT_TPM_INTERFACE* hsm_client_tpm_interface()
{
    return &tpm_interface;
}

const HSM_CLIENT_X509_INTERFACE* hsm_client_x509_interface()
{
    return &x509_interface;
}
