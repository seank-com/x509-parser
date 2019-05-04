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
"MIIBNjCB3qADAgECAgkA5cOSJdwEK48wCgYIKoZIzj0EAwIwFjEUMBIGA1UEAwwL\r\n"
"cHJvZHVjdC1pY2EwHhcNMTkwNTAzMjMzODI4WhcNMjkwNDMwMjMzODI4WjAUMRIw\r\n"
"EAYDVQQDDAlzZWFuay1kcHMwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASSQu8r\r\n"
"yDHPeFEohSn3HwTI37DyFu2CGoPjEicfYyTWnpuKc0P/H5a4aukpjsrzxIwuKZRz\r\n"
"9xyJwshxSpRBUJKNoxcwFTATBgNVHSUEDDAKBggrBgEFBQcDAjAKBggqhkjOPQQD\r\n"
"AgNHADBEAiB5uy1KEdfV1VFHc4PQncXsnT16nLka1LzLdZQEHBan6QIgA6NjzGtd\r\n"
"Momsjlkhu1OkiuVGrGSInV4djcl7yxVNiJ0=\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBQDCB5qADAgECAgkAg1EQ7SJpcLYwCgYIKoZIzj0EAwIwEzERMA8GA1UEAwwI\r\n"
"c2VhbmstY2EwHhcNMTkwNTAzMjMzNDAwWhcNMjkwNDMwMjMzNDAwWjAWMRQwEgYD\r\n"
"VQQDDAtwcm9kdWN0LWljYTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABH/dt89X\r\n"
"f5s42C/yoYwGKAjOb16VZxCmzYC4nRP5kz4WqYLE1yr37JRs7nztRV8HlUX1/uQ1\r\n"
"JSONeyaWJ9PjprSjIDAeMAsGA1UdDwQEAwICBDAPBgNVHRMBAf8EBTADAQH/MAoG\r\n"
"CCqGSM49BAMCA0kAMEYCIQCaYH5CuiNyZ4qe7zShw38l9WSDRJdEML9kumwDG/9i\r\n"
"jwIhAIbokxDmjYVmjUCam0H8ET0Zg87+qE6nVYyhFYsgvmPA\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"MIIBOzCB46ADAgECAgkA9hHBy07V8REwCgYIKoZIzj0EAwIwEzERMA8GA1UEAwwI\r\n"
"c2VhbmstY2EwHhcNMTkwNTAzMjMzMzIzWhcNMjkwNDMwMjMzMzIzWjATMREwDwYD\r\n"
"VQQDDAhzZWFuay1jYTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABAjSoSCwta24\r\n"
"zbPIPDMrIm3xMJpz5lgUZEIeMifl7Y2utHK3B8s8LLPb2XF1ko9UhMaxDXhd5jPh\r\n"
"olQO+CCcFKyjIDAeMAsGA1UdDwQEAwICBDAPBgNVHRMBAf8EBTADAQH/MAoGCCqG\r\n"
"SM49BAMCA0cAMEQCIH4cbN8kblkfp/CNfbzGI2J3aBE0ss8HmpHaY7UEKZBMAiAP\r\n"
"n4lzrBZkOcLRtTlu5hGKllzQ4fzQepzwQmexRoCQtg==\r\n"
"-----END CERTIFICATE-----\r\n";

static const char* s_private_key =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"MHcCAQEEICA0Ybk5zD6n0oaeYlk35qvi2JG3BhodCkFuap5bT5zuoAoGCCqGSM49\r\n"
"AwEHoUQDQgAEkkLvK8gxz3hRKIUp9x8EyN+w8hbtghqD4xInH2Mk1p6binND/x+W\r\n"
"uGrpKY7K88SMLimUc/ccicLIcUqUQVCSjQ==\r\n"
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

int custom_hsm_activate_identity_key(HSM_CLIENT_HANDLE handle, const unsigned char* key, size_t key_len)
{
  // E_NOT_IMPL
  return 123456;
}

char* custom_hsm_symm_key(HSM_CLIENT_HANDLE handle)
{
  // E_NOT_IMPL
  return "E_NOT_IMPL";
}

char* custom_hsm_get_registration_name(HSM_CLIENT_HANDLE handle)
{
  // E_NOT_IMPL
  return "E_NOT_IMPL";
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

static const HSM_CLIENT_KEY_INTERFACE symm_key_interface =
{
    custom_hsm_create,
    custom_hsm_destroy,
    custom_hsm_symm_key,
    custom_hsm_get_registration_name
};

const HSM_CLIENT_TPM_INTERFACE* hsm_client_tpm_interface()
{
    return &tpm_interface;
}

const HSM_CLIENT_X509_INTERFACE* hsm_client_x509_interface()
{
    return &x509_interface;
}

const HSM_CLIENT_KEY_INTERFACE* hsm_client_key_interface()
{
    return &symm_key_interface;
}
