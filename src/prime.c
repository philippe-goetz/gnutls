/*
 * Copyright (C) 2001-2011  Free Software Foundation, Inc.
 *
 * This file is part of GnuTLS.
 *
 * GnuTLS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GnuTLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <config.h>

#ifdef ENABLE_PKI

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <gnutls/gnutls.h>
#include <certtool-common.h>

/* Generates Diffie-Hellman parameters (a prime and a generator
 * of the group). Exports them in PKCS #3 format. Used by certtool.
 */

extern FILE *outfile;
extern FILE *infile;

static const int cparams = 0;

/* If how is zero then the included parameters are used.
 */
int
generate_prime (int how, common_info_st * info)
{
  unsigned int i;
  int ret;
  gnutls_dh_params_t dh_params;
  gnutls_datum_t p, g;
  int bits = get_bits (GNUTLS_PK_DH, info->bits, info->sec_param);
  unsigned int q_bits = 0;

  gnutls_dh_params_init (&dh_params);

  if (how != 0)
    {
      fprintf (stderr, "Generating DH parameters (%d bits)...\n", bits);
      fprintf (stderr, "(might take long time)\n");
    }
  else
    fprintf (stderr, "Retrieving DH parameters...\n");

  if (how != 0)
    {
      ret = gnutls_dh_params_generate2 (dh_params, bits);
      if (ret < 0)
        {
          fprintf (stderr, "Error generating parameters: %s\n",
                   gnutls_strerror (ret));
          exit (1);
        }

      ret = gnutls_dh_params_export_raw (dh_params, &p, &g, &q_bits);
      if (ret < 0)
        {
          fprintf (stderr, "Error exporting parameters: %s\n",
                   gnutls_strerror (ret));
          exit (1);
        }
    }
  else
    {
#ifdef ENABLE_SRP
      if (bits <= 1024)
        {
          p = gnutls_srp_1024_group_prime;
          g = gnutls_srp_1024_group_generator;
          bits = 1024;
        }
      else if (bits <= 1536)
        {
          p = gnutls_srp_1536_group_prime;
          g = gnutls_srp_1536_group_generator;
          bits = 1536;
        }
      else if (bits <= 2048)
        {
          p = gnutls_srp_2048_group_prime;
          g = gnutls_srp_2048_group_generator;
          bits = 2048;
        }
      else if (bits <=3072)
        {
          p = gnutls_srp_3072_group_prime;
          g = gnutls_srp_3072_group_generator;
          bits = 3072;
        }
      else
        {
          p = gnutls_srp_4096_group_prime;
          g = gnutls_srp_4096_group_generator;
          bits = 4096;
        }

      ret = gnutls_dh_params_import_raw (dh_params, &p, &g);
      if (ret < 0)
        {
          fprintf (stderr, "Error exporting parameters: %s\n",
                   gnutls_strerror (ret));
          exit (1);
        }
#else
      fprintf (stderr, "Parameters unavailable as SRP is disabled.\n");
      exit(1);
#endif
    }

  if (cparams)
    {

      fprintf (outfile, "/* generator */\n");
      fprintf (outfile, "\nconst uint8 g[%d] = { ", g.size);

      for (i = 0; i < g.size; i++)
        {
          if (i % 7 == 0)
            fprintf (outfile, "\n\t");
          fprintf (outfile, "0x%.2x", g.data[i]);
          if (i != g.size - 1)
            fprintf (outfile, ", ");
        }

      fprintf (outfile, "\n};\n\n");
    }
  else
    {
      fprintf (outfile, "\nGenerator: ");

      for (i = 0; i < g.size; i++)
        {
          if (i != 0 && i % 12 == 0)
            fprintf (outfile, "\n\t");
          else if (i != 0 && i != g.size)
            fprintf (outfile, ":");

          fprintf (outfile, "%.2x", g.data[i]);
        }

      fprintf (outfile, "\n\n");
    }

  /* print prime */

  if (cparams)
    {
      fprintf (outfile, "/* prime - %d bits */\n", p.size * 8);
      fprintf (outfile, "\nconst uint8 prime[%d] = { ", p.size);

      for (i = 0; i < p.size; i++)
        {
          if (i % 7 == 0)
            fprintf (outfile, "\n\t");
          fprintf (outfile, "0x%.2x", p.data[i]);
          if (i != p.size - 1)
            fprintf (outfile, ", ");
        }

      fprintf (outfile, "\n};\n");
    }
  else
    {
      fprintf (outfile, "Prime (%d bits):", bits);

      for (i = 0; i < p.size; i++)
        {
          if (i % 12 == 0)
            fprintf (outfile, "\n\t");
          else if (i != 0 && i != p.size)
            fprintf (outfile, ":");
          fprintf (outfile, "%.2x", p.data[i]);
        }

      if (q_bits > 0)
        fprintf (outfile, "\n\nRecommended key length: %d bits\n", q_bits);
      fprintf (outfile, "\n");
    }

  if (!cparams)
    {                           /* generate a PKCS#3 structure */

      size_t len = buffer_size;

      ret = gnutls_dh_params_export_pkcs3 (dh_params, GNUTLS_X509_FMT_PEM,
                                           buffer, &len);

      if (ret == 0)
        {
          fprintf (outfile, "\n%s", buffer);
        }
      else
        {
          fprintf (stderr, "Error: %s\n", gnutls_strerror (ret));
        }

    }

  return 0;
}

#endif
