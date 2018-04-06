// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hsm_client_data.h"
#include "x509.h"
#include "pem2bin.h"

typedef struct CUSTOM_HSM_INFO_TAG
{
    int info;
} CUSTOM_HSM_INFO;

static const char* s_certificate =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBPDCB4qADAgECAgkAk/eZ6AyAmjwwCgYIKoZIzj0EAwIwGTEXMBUGA1UEAwwO\r\n"
"Y29udHJhY3Rvci1pY2EwHhcNMTgwNDA1MjM0ODA3WhcNMjgwNDAyMjM0ODA3WjAV\r\n"
"MRMwEQYDVQQDDApzZWFuay10ZXN0MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE\r\n"
"ZW+hmWPHA5vqHLb+2jG98x+yYvUNolhIAEl077YbCxixHfiHB/bKnSwFs5ABPTj3\r\n"
"JYeQWnt5qTwxPIaFFGEm16MXMBUwEwYDVR0lBAwwCgYIKwYBBQUHAwIwCgYIKoZI\r\n"
"zj0EAwIDSQAwRgIhAKxYRmy/PDA9I5AnONzTehHOSJJo5dt+MTJVbSHSZ6utAiEA\r\n"
"m8YTL9hrsol4HTZU0xlRM2CaSg3gAM5pf27mu4thHWM=\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBRTCB7KADAgECAgkAtQ06po5V84swCgYIKoZIzj0EAwIwFjEUMBIGA1UEAwwL\r\n"
"ZmFjdG9yeS1pY2EwHhcNMTgwNDA1MjM0ODAzWhcNMjgwNDAyMjM0ODAzWjAZMRcw\r\n"
"FQYDVQQDDA5jb250cmFjdG9yLWljYTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IA\r\n"
"BKxOndWPzdwzxeveRG6S3nkioa3SfKGESin181r3eXQybrXzhwJ/Tk1wamqhL8GL\r\n"
"c8v834TkzMqxOed+8zlyZDWjIDAeMAsGA1UdDwQEAwICBDAPBgNVHRMBAf8EBTAD\r\n"
"AQH/MAoGCCqGSM49BAMCA0gAMEUCIHPnPDtWlHnU6+27D7N3PebKMeJW2zlV8WOu\r\n"
"ojxtov4AAiEAqudHidclgtrA7ZmgLFWituNVhtRnoGFtr8djwc75508=\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBPjCB5aADAgECAgkAmgxM4KR7rtcwCgYIKoZIzj0EAwIwEjEQMA4GA1UEAwwH\r\n"
"c2t1LWljYTAeFw0xODA0MDUyMzQ4MDBaFw0yODA0MDIyMzQ4MDBaMBYxFDASBgNV\r\n"
"BAMMC2ZhY3RvcnktaWNhMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEsGHMvedY\r\n"
"ShGgg03AA+CeHbw3qEh3tVbJrpPhVzh7xXL3R4yI7P9/1Nl70M4Am/9HJjb13n1Q\r\n"
"iNnyG2tEg1+esqMgMB4wCwYDVR0PBAQDAgIEMA8GA1UdEwEB/wQFMAMBAf8wCgYI\r\n"
"KoZIzj0EAwIDSAAwRQIhAIn8PjJR0iXGIFeKC8tcQCktL56+bMeQgjUtkbTMk5+L\r\n"
"AiAaW1ES2wu+HLC/CUqY1BjiSBeUq+AOCxBid8GS7MzU3A==\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBPzCB5aADAgECAgkA88Z6QcqFIvowCgYIKoZIzj0EAwIwFjEUMBIGA1UEAwwL\r\n"
"cHJvZHVjdC1pY2EwHhcNMTgwNDA1MjM0NzU2WhcNMjgwNDAyMjM0NzU2WjASMRAw\r\n"
"DgYDVQQDDAdza3UtaWNhMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEoEmSdKx5\r\n"
"2NqOxACTFli60dsGVSk1DHhQ8OnZP+KMAEmiNeOmgSOIOzxKSIChDrgObO+BNYaG\r\n"
"RzmBU3wBFn2r3aMgMB4wCwYDVR0PBAQDAgIEMA8GA1UdEwEB/wQFMAMBAf8wCgYI\r\n"
"KoZIzj0EAwIDSQAwRgIhAKPKFxpYji2xZXlEpohoy/gfH/n+CyMTAn7m6xIIuRwH\r\n"
"AiEAgJLiL68vwjoJqLUp70WrYrkq4PW0vXj8lFsWGFgSYYs=\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBRDCB66ADAgECAgkA1oQZrXKZE0QwCgYIKoZIzj0EAwIwGDEWMBQGA1UEAwwN\r\n"
"c2Vhbmstcm9vdC1jYTAeFw0xODA0MDUyMzQ3NTNaFw0yODA0MDIyMzQ3NTNaMBYx\r\n"
"FDASBgNVBAMMC3Byb2R1Y3QtaWNhMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE\r\n"
"ABSieNiVZaZ6PlY6sN/cSD7rNk0UQbtps8Abf/LJ3ZJndEGFUPrlFr2dO5zp4IdX\r\n"
"8dPAYG84FxB2wHbQJZ+dP6MgMB4wCwYDVR0PBAQDAgIEMA8GA1UdEwEB/wQFMAMB\r\n"
"Af8wCgYIKoZIzj0EAwIDSAAwRQIhAIZuUmp7YUxrnlrdBPSZDim8YAk3N9RyPmf9\r\n"
"fDAqyf4zAiAwEZvhuJl8O8wfZtWiBMqPHUV2hvPu8aiDYYbbmQXHFA==\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBRzCB7aADAgECAgkAotrHnwYyOI4wCgYIKoZIzj0EAwIwGDEWMBQGA1UEAwwN\r\n"
"c2Vhbmstcm9vdC1jYTAeFw0xODA0MDUyMzQ3NDlaFw0yODA0MDIyMzQ3NDlaMBgx\r\n"
"FjAUBgNVBAMMDXNlYW5rLXJvb3QtY2EwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNC\r\n"
"AARsPBukfg97bXTM1WkPTehJDXiG0US2CSJG1/hMn38uOOuV2v6apIp/4Ws12NwB\r\n"
"nFnIRU5St/9c/ZX9YJiF58ChoyAwHjALBgNVHQ8EBAMCAgQwDwYDVR0TAQH/BAUw\r\n"
"AwEB/zAKBggqhkjOPQQDAgNJADBGAiEAkAmsv2GKpD9l+tqDmMbeHdn+dN7dPx+b\r\n"
"ivFZ8rIzgZgCIQDd9TRityYhG4XJD2mKH1d2Rx1WESxCLSxLRkpnlwqgwQ==\r\n"
"-----END CERTIFICATE-----\r\n";

static const char* s_private_key =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"MHcCAQEEIDxc8zENslY0CrH4RYKU6db39Df9G1ViLU/23B9LF2hroAoGCCqGSM49\r\n"
"AwEHoUQDQgAEZW+hmWPHA5vqHLb+2jG98x+yYvUNolhIAEl077YbCxixHfiHB/bK\r\n"
"nSwFs5ABPTj3JYeQWnt5qTwxPIaFFGEm1w==\r\n"
"-----END EC PRIVATE KEY-----\r\n";

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

        if (s_certificate == NULL)
        {
            (void)printf("Failure retrieving cert");
            result = NULL;
        }
        else
        {
            size_t length = strlen(s_certificate);
            result = malloc(length + 1);
            if (result == NULL)
            {
                (void)printf("Failure allocating certifiicate");
            }
            else
            {
                strcpy(result, s_certificate);
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

        if (s_private_key == NULL)
        {
            (void)printf("Failure retrieving private key");
            result = NULL;
        }
        else
        {
            size_t length = strlen(s_private_key);
            result = malloc(length + 1);
            if (result == NULL)
            {
                (void)printf("Failure allocating private key");
            }
            else
            {
                strcpy(result, s_private_key);
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
        static char common_name[129] = { 0 };

        CUSTOM_HSM_INFO* cust_hsm = (CUSTOM_HSM_INFO*)handle;
        cust_hsm->info++;

        if (s_certificate != NULL)
        {
          uint8_t* bin;
          uint32_t binlen;
          size_t length = strlen(s_certificate);
          int hr = pem2bin(s_certificate, length, &bin, &binlen);
          if (hr == 0)
          {
            hr = getCommonName(bin, common_name, 128);
            if (hr != 0)
            {
              common_name[0] = '\0';
            }
            free(bin);
          }
        }

        if (common_name[0] == '\0')
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
                result[length] = '\0';
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
