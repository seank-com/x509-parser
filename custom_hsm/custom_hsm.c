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
"Your complete...\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"...certificate chain...\r\n"
"-----END CERTIFICATE-----\r\n"
"-----BEGIN CERTIFICATE-----\r\n"
"...goes here.\r\n"
"-----END CERTIFICATE-----\r\n";

static const char* s_private_key =
"-----BEGIN EC PRIVATE KEY-----\r\n"
"Your private key goes here\r\n"
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
