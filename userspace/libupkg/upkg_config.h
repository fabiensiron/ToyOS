/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#ifndef _UPKG_CONFIG_H_
# define _UPKG_CONFIG_H_

typedef enum {
	UPKG_STRING = 0,
	UPKG_BOOL,
	UPKG_INT,
	UPKG_NULL
} upkg_type_t;

extern const char *upkg_config_get(const char *key);

#endif /*_UPKG_CONFIG_H_*/
