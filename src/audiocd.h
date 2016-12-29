#ifndef __AUDIOCD_H__
#define __AUDIOCD_H__

#include <stdlib.h>

#include <cddb/cddb.h>

static cddb_disc_t *audiocd_disc;
static cddb_track_t *audiocd_track = NULL;

void audiocd_read_metadata();
const char *audiocd_get_title();
const char *audiocd_get_artist();
int audiocd_next_track();
const char *audiocd_get_track_title();
int audiocd_get_track_length();

#endif
