/*
 * Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008, 2010
 * Free Software Foundation, Inc.
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

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "gnutls_auth.h"

#define GNUTLS_RENEGO_PROTECTION_REQUEST_MAJOR 0x00
#define GNUTLS_RENEGO_PROTECTION_REQUEST_MINOR 0xFF

/* Functions for version handling. */
gnutls_protocol_t _gnutls_version_lowest (gnutls_session_t session);
gnutls_protocol_t _gnutls_version_max (gnutls_session_t session);
int _gnutls_version_priority (gnutls_session_t session,
                              gnutls_protocol_t version);
int _gnutls_version_is_supported (gnutls_session_t session,
                                  const gnutls_protocol_t version);
int _gnutls_version_get_major (gnutls_protocol_t ver);
int _gnutls_version_get_minor (gnutls_protocol_t ver);
gnutls_protocol_t _gnutls_version_get (int major, int minor);

/* Functions for feature checks */
int _gnutls_version_has_selectable_prf (gnutls_protocol_t version);
int _gnutls_version_has_selectable_sighash (gnutls_protocol_t version);
int _gnutls_version_has_extensions (gnutls_protocol_t version);
int _gnutls_version_has_explicit_iv (gnutls_protocol_t version);

/* Functions for MACs. */
int _gnutls_mac_is_ok (gnutls_mac_algorithm_t algorithm);
gnutls_mac_algorithm_t _gnutls_x509_oid2mac_algorithm (const char *oid);
const char *_gnutls_x509_mac_to_oid (gnutls_mac_algorithm_t mac);

/* Functions for cipher suites. */
int _gnutls_supported_ciphersuites (gnutls_session_t session,
                                    cipher_suite_st ** ciphers);
int _gnutls_supported_ciphersuites_sorted (gnutls_session_t session,
                                           cipher_suite_st ** ciphers);
const char *_gnutls_cipher_suite_get_name (cipher_suite_st * algorithm);
gnutls_cipher_algorithm_t _gnutls_cipher_suite_get_cipher_algo (const
                                                                cipher_suite_st
                                                                * algorithm);
gnutls_kx_algorithm_t _gnutls_cipher_suite_get_kx_algo (const cipher_suite_st
                                                        * algorithm);
gnutls_mac_algorithm_t _gnutls_cipher_suite_get_mac_algo (const
                                                          cipher_suite_st *
                                                          algorithm);
cipher_suite_st _gnutls_cipher_suite_get_suite_name (cipher_suite_st *
                                                     algorithm);

/* Functions for ciphers. */
int _gnutls_cipher_is_block (gnutls_cipher_algorithm_t algorithm);
int _gnutls_cipher_algo_is_aead (gnutls_cipher_algorithm_t algorithm);
int _gnutls_cipher_is_ok (gnutls_cipher_algorithm_t algorithm);
int _gnutls_cipher_get_iv_size (gnutls_cipher_algorithm_t algorithm);
int _gnutls_cipher_get_export_flag (gnutls_cipher_algorithm_t algorithm);
/* at least for now iv_size == tag_size */
#define _gnutls_cipher_get_tag_size _gnutls_cipher_get_iv_size

/* Functions for key exchange. */
int _gnutls_kx_needs_dh_params (gnutls_kx_algorithm_t algorithm);
int _gnutls_kx_needs_rsa_params (gnutls_kx_algorithm_t algorithm);
mod_auth_st *_gnutls_kx_auth_struct (gnutls_kx_algorithm_t algorithm);
int _gnutls_kx_is_ok (gnutls_kx_algorithm_t algorithm);

/* Type to KX mappings. */
gnutls_kx_algorithm_t _gnutls_map_kx_get_kx (gnutls_credentials_type_t type,
                                             int server);
gnutls_credentials_type_t _gnutls_map_kx_get_cred (gnutls_kx_algorithm_t
                                                   algorithm, int server);

/* KX to PK mapping. */
gnutls_pk_algorithm_t _gnutls_map_pk_get_pk (gnutls_kx_algorithm_t
                                             kx_algorithm);
gnutls_pk_algorithm_t _gnutls_x509_oid2pk_algorithm (const char *oid);
const char *_gnutls_x509_pk_to_oid (gnutls_pk_algorithm_t pk);

enum encipher_type
{ CIPHER_ENCRYPT = 0, CIPHER_SIGN = 1, CIPHER_IGN };

enum encipher_type _gnutls_kx_encipher_type (gnutls_kx_algorithm_t algorithm);

/* Functions for sign algorithms. */
gnutls_sign_algorithm_t _gnutls_x509_oid2sign_algorithm (const char *oid);
gnutls_sign_algorithm_t _gnutls_x509_pk_to_sign (gnutls_pk_algorithm_t pk,
                                                 gnutls_mac_algorithm_t mac);
gnutls_pk_algorithm_t _gnutls_x509_sign_to_pk (gnutls_sign_algorithm_t sign);
const char *_gnutls_x509_sign_to_oid (gnutls_pk_algorithm_t,
                                      gnutls_mac_algorithm_t mac);
gnutls_sign_algorithm_t _gnutls_tls_aid_to_sign (const sign_algorithm_st *
                                                 aid);
const sign_algorithm_st* _gnutls_sign_to_tls_aid (gnutls_sign_algorithm_t sign);
gnutls_mac_algorithm_t
_gnutls_sign_get_hash_algorithm (gnutls_sign_algorithm_t);
gnutls_pk_algorithm_t _gnutls_sign_get_pk_algorithm (gnutls_sign_algorithm_t);

int _gnutls_mac_priority (gnutls_session_t session,
                          gnutls_mac_algorithm_t algorithm);
int _gnutls_cipher_priority (gnutls_session_t session,
                             gnutls_cipher_algorithm_t algorithm);
int _gnutls_kx_priority (gnutls_session_t session,
                         gnutls_kx_algorithm_t algorithm);

unsigned int _gnutls_pk_bits_to_subgroup_bits (unsigned int pk_bits);

/* ECC */
struct gnutls_ecc_curve_entry_st
{
  const char *name;
  ecc_curve_t id;
  int size; /* the size in bytes */

  /** The prime that defines the field the curve is in (encoded in hex) */
  const char *prime;
  /** The fields A param (hex) */
  const char *A;
  /** The fields B param (hex) */
  const char *B;
  /** The order of the curve (hex) */
  const char *order;
  /** The x co-ordinate of the base point on the curve (hex) */
  const char *Gx;
  /** The y co-ordinate of the base point on the curve (hex) */
  const char *Gy;
};
typedef struct gnutls_ecc_curve_entry_st gnutls_ecc_curve_entry_st;

const char * _gnutls_ecc_curve_get_name (ecc_curve_t curve);
const gnutls_ecc_curve_entry_st * _gnutls_ecc_curve_get_params (ecc_curve_t curve);
int _gnutls_ecc_curve_get_size (ecc_curve_t curve);
ecc_curve_t _gnutls_ecc_curve_get_id (const char *name);

#endif