/*
 *      Copyright (C) 2001,2002 Nikos Mavroyanopoulos
 *
 * This file is part of GNUTLS.
 *
 *  The GNUTLS library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public   
 *  License as published by the Free Software Foundation; either 
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */

#include <gnutls_int.h>
#include <gnutls_num.h>
#include <gnutls_datum.h>
#include <gnutls_errors.h>

/* contains functions that make it easier to
 * write vectors of <size|data>. The destination size
 * should be preallocated (datum.size+(bits/8))
 */

void _gnutls_write_datum16( opaque* dest, gnutls_datum dat) {
	_gnutls_write_uint16( dat.size, dest);
	memcpy( &dest[2], dat.data, dat.size);
}
void _gnutls_write_datum24( opaque* dest, gnutls_datum dat) {
	_gnutls_write_uint24( dat.size, dest);
	memcpy( &dest[3], dat.data, dat.size);
}
void _gnutls_write_datum32( opaque* dest, gnutls_datum dat) {
	_gnutls_write_uint32( dat.size, dest);
	memcpy( &dest[4], dat.data, dat.size);
}
void _gnutls_write_datum8( opaque* dest, gnutls_datum dat) {
	dest[0] = (uint8) dat.size;
	memcpy( &dest[1], dat.data, dat.size);
}

int gnutls_set_datum( gnutls_datum* dat, const void* data, int data_size) {
	dat->data = gnutls_malloc(data_size);
	if (dat->data==NULL) return GNUTLS_E_MEMORY_ERROR;
	
	dat->size = data_size;
	memcpy( dat->data, data, data_size);
	
	return 0;
}

int gnutls_datum_append( gnutls_datum* dst, const void* data, int data_size) {

	dst->data = gnutls_realloc(dst->data, data_size+dst->size);
	if (dst->data==NULL) return GNUTLS_E_MEMORY_ERROR;
	
	memcpy( &dst->data[dst->size], data, data_size);
	dst->size += data_size;

	return 0;
}

int gnutls_sset_datum( gnutls_datum* dat, const void* data, int data_size) {
	dat->data = gnutls_secure_malloc(data_size);
	if (dat->data==NULL) return GNUTLS_E_MEMORY_ERROR;
	
	dat->size = data_size;
	memcpy( dat->data, data, data_size);
	
	return 0;
}

void gnutls_free_datum( gnutls_datum* dat) {
	if (dat->data!=NULL && dat->size!=0)
		gnutls_free( dat->data);

	dat->data = NULL;
	dat->size = 0;
}

void gnutls_sfree_datum( gnutls_datum* dat) {
	if (dat->data!=NULL && dat->size!=0)
		gnutls_free( dat->data);

	dat->data = NULL;
	dat->size = 0;
}
