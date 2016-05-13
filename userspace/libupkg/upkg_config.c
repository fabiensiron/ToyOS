/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libupkg/upkg_config.h"

struct config_entry {
	uint8_t type;
	const char *key;
	const char *def;
};

static struct config_entry config_entries[] = {
	{
		UPKG_STRING,
		"UPKG_DBDIR",
		"/var/db/upkg"
	},
	{
		UPKG_STRING,
		"UPKG_ROOT",
		""
	},
	{
		UPKG_STRING,
		"UPKG_CACHE",
		"/var/cache/upkg"
	},
	{
		UPKG_STRING,
		"UPKG_TMPDIR",
		"/tmp"
	},
	{
		UPKG_STRING,
		"UPKG_LOCALPATH",
		"/usr/share/upkg/ports"
	},
	{
		UPKG_STRING,
		"UPKG_PATH",
		"8.8.8.8"
	},
	{
		UPKG_NULL,
		NULL,
		NULL
	}
};

static inline int
upkg_entry_is_null(struct config_entry *entry) {
	return (entry->type == UPKG_NULL &&
		entry->key == NULL &&
		entry->def == NULL);
}

static inline int
upkg_is_entry(struct config_entry *entry, const char *key) {
	return (strcmp(entry->key, key) == 0);
}

const char *
upkg_config_get(const char *key) {
	struct config_entry *entry;
	const char *conf_def;

	/* first, we look in the environment variables */
	if ((conf_def = getenv(key)))
		return conf_def;

	/* otherwise, we check the default configurations */
	for (entry = config_entries;
	     !upkg_entry_is_null(entry) && !upkg_is_entry(entry, key);
	     entry++)
		;

	return upkg_entry_is_null(entry) ? NULL : entry->def;
}
