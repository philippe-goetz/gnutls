/*
 * Copyright (C) 2011 Free Software Foundation, Inc.
 *
 * Author: Nikos Mavrogiannopoulos
 *
 * This file is part of GnuTLS.
 *
 * The GnuTLS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA
 *
 */

#include <gnutls_int.h>
#include <gnutls_errors.h>
#include <auth/cert.h>
#include <gnutls_x509.h>
#include "x509/x509_int.h"
#ifdef ENABLE_OPENPGP
#include "openpgp/gnutls_openpgp.h"
#endif

/**
 * gnutls_pcert_import_x509:
 * @pcert: The pcert structure
 * @crt: The raw certificate to be imported
 * @flags: zero for now
 *
 * This convenience function will import the given certificate to a
 * #gnutls_pcert_st structure. The structure must be deinitialized
 * afterwards using gnutls_pcert_deinit();
 *
 * Returns: On success, %GNUTLS_E_SUCCESS is returned, otherwise a
 *   negative error value.
 **/
int gnutls_pcert_import_x509 (gnutls_pcert_st* pcert,
	gnutls_x509_crt_t crt, unsigned int flags)
{
int ret;
size_t sz;

  memset(pcert, 0, sizeof(*pcert));

  pcert->type = GNUTLS_CRT_X509;
  pcert->sign_algo = gnutls_x509_crt_get_signature_algorithm(crt);
  pcert->cert.data = NULL;

  sz = 0;
  ret = gnutls_x509_crt_export(crt, GNUTLS_X509_FMT_DER, NULL, &sz);
  if (ret < 0 && ret != GNUTLS_E_SHORT_MEMORY_BUFFER)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  pcert->cert.data = gnutls_malloc(sz);
  if (pcert->cert.data == NULL)
    {
      ret = gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
      goto cleanup;
    }

  ret = gnutls_x509_crt_export(crt, GNUTLS_X509_FMT_DER, pcert->cert.data, &sz);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }
  pcert->cert.size = sz;

  ret = gnutls_pubkey_init(&pcert->pubkey);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  ret = gnutls_pubkey_import_x509(pcert->pubkey, crt, 0);
  if (ret < 0)
    {
      gnutls_pubkey_deinit(pcert->pubkey);
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  return 0;

cleanup:
  gnutls_free(pcert->cert.data);

  return ret;
}

/**
 * gnutls_pcert_import_x509_raw:
 * @pcert: The pcert structure
 * @cert: The raw certificate to be imported
 * @format: The format of the certificate
 * @flags: zero for now
 *
 * This convenience function will import the given certificate to a
 * #gnutls_pcert_st structure. The structure must be deinitialized
 * afterwards using gnutls_pcert_deinit();
 *
 * Returns: On success, %GNUTLS_E_SUCCESS is returned, otherwise a
 *   negative error value.
 **/
int gnutls_pcert_import_x509_raw (gnutls_pcert_st *pcert,
	const gnutls_datum_t* cert, 
	gnutls_x509_crt_fmt_t format, unsigned int flags)
{
int ret;
gnutls_x509_crt_t crt;

  memset(pcert, 0, sizeof(*pcert));

  ret = gnutls_x509_crt_init(&crt);
  if (ret < 0)
    return gnutls_assert_val(ret);

  ret = gnutls_x509_crt_import(crt, cert, format);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  ret = gnutls_pcert_import_x509(pcert, crt, flags);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  ret = 0;

cleanup:
  gnutls_x509_crt_deinit(crt);

  return ret;
}

/**
 * gnutls_pcert_import_openpgp:
 * @pcert: The pcert structure
 * @cert: The raw certificate to be imported
 * @flags: zero for now
 *
 * This convenience function will import the given certificate to a
 * #gnutls_pcert_st structure. The structure must be deinitialized
 * afterwards using gnutls_pcert_deinit();
 *
 * Returns: On success, %GNUTLS_E_SUCCESS is returned, otherwise a
 *   negative error value.
 **/
int gnutls_pcert_import_openpgp (gnutls_pcert_st* pcert,
	gnutls_openpgp_crt_t crt, unsigned int flags)
{
int ret;
size_t sz;

  memset(pcert, 0, sizeof(*pcert));

  pcert->type = GNUTLS_CRT_OPENPGP;
  pcert->sign_algo = GNUTLS_SIGN_UNKNOWN;
  pcert->cert.data = NULL;

  sz = 0;
  ret = gnutls_openpgp_crt_export(crt, GNUTLS_OPENPGP_FMT_RAW, NULL, &sz);
  if (ret < 0 && ret != GNUTLS_E_SHORT_MEMORY_BUFFER)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  pcert->cert.data = gnutls_malloc(sz);
  if (pcert->cert.data == NULL)
    {
      ret = gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
      goto cleanup;
    }

  ret = gnutls_openpgp_crt_export(crt, GNUTLS_X509_FMT_DER, pcert->cert.data, &sz);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }
  pcert->cert.size = sz;

  ret = gnutls_pubkey_init(&pcert->pubkey);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  ret = gnutls_pubkey_import_openpgp(pcert->pubkey, crt, 0);
  if (ret < 0)
    {
      gnutls_pubkey_deinit(pcert->pubkey);
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  return 0;

cleanup:
  gnutls_free(pcert->cert.data);

  return ret;
}

/**
 * gnutls_pcert_import_openpgp_raw:
 * @pcert: The pcert structure
 * @cert: The raw certificate to be imported
 * @format: The format of the certificate
 * @keyid: The key ID to use (NULL for the master key)
 * @flags: zero for now
 *
 * This convenience function will import the given certificate to a
 * #gnutls_pcert_st structure. The structure must be deinitialized
 * afterwards using gnutls_pcert_deinit();
 *
 * Returns: On success, %GNUTLS_E_SUCCESS is returned, otherwise a
 *   negative error value.
 **/
int gnutls_pcert_import_openpgp_raw (gnutls_pcert_st *pcert,
	const gnutls_datum_t* cert, 
	gnutls_openpgp_crt_fmt_t format, 
	gnutls_openpgp_keyid_t keyid,
	unsigned int flags)
{
int ret;
gnutls_openpgp_crt_t crt;

  memset(pcert, 0, sizeof(*pcert));

  pcert->cert.data = NULL;

  ret = gnutls_openpgp_crt_init(&crt);
  if (ret < 0)
    return gnutls_assert_val(ret);

  ret = gnutls_openpgp_crt_import(crt, cert, format);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }

  if (keyid != NULL)
    {
      ret = gnutls_openpgp_crt_set_preferred_key_id(crt, keyid);
      if (ret < 0)
        {
          ret = gnutls_assert_val(ret);
          goto cleanup;
        }
    }

  ret = gnutls_pcert_import_openpgp(pcert, crt, flags);
  if (ret < 0)
    {
      ret = gnutls_assert_val(ret);
      goto cleanup;
    }
  ret = 0;

cleanup:
  gnutls_openpgp_crt_deinit(crt);

  return ret;
}

/**
 * gnutls_pcert_deinit:
 * @pcert: The structure to be deinitialized
 *
 * This function will deinitialize a pcert structure.
 **/
void
gnutls_pcert_deinit (gnutls_pcert_st *pcert)
{
  gnutls_pubkey_deinit(pcert->pubkey);
  pcert->pubkey = NULL;
  gnutls_free(pcert->cert.data);
  pcert->cert.data = NULL;
}

/* Converts the first certificate for the cert_auth_info structure
 * to a pcert.
 */
int
_gnutls_get_auth_info_pcert (gnutls_pcert_st* pcert,
                             gnutls_certificate_type_t type,
                             cert_auth_info_t info)
{
  switch (type)
    {
    case GNUTLS_CRT_X509:
      return gnutls_pcert_import_x509_raw(pcert, &info->raw_certificate_list[0], 
        GNUTLS_X509_FMT_DER, GNUTLS_PCERT_NO_CERT);
#ifdef ENABLE_OPENPGP
    case GNUTLS_CRT_OPENPGP:
      return gnutls_pcert_import_openpgp_raw(pcert,
                                               &info->raw_certificate_list[0],
                                               GNUTLS_OPENPGP_FMT_RAW,
                                               info->use_subkey ? info->
                                               subkey_id : NULL, GNUTLS_PCERT_NO_CERT);
#endif
    default:
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }
}