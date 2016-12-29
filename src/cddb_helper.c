#include "cddb_helper.h"

#include <cdio/cdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define libcdio_cddb_error_exit(...) cddb_error_exit(-1, "libcdio: " __VA_ARGS__)

/* free pointer when it's pointing at something */
#define FREE_NOT_NULL(p) if (p) { free(p); p = NULL; }

cddb_disc_t *cddb_read_db(cddb_conn_t *conn, const char *category, int discid, int quiet)
{
    cddb_disc_t *disc = NULL;   /* libcddb disc structure */
    int success;

    disc = cddb_disc_new();

    if (disc) {
        cddb_disc_set_category_str(disc, category);
        cddb_disc_set_discid(disc, discid);

        success = cddb_read(conn, disc);
        if (!success) {
            if (!quiet) {
                cddb_error_print(cddb_errno(conn));
            }
            cddb_disc_destroy(disc);
            return NULL;
        }
    }
    return disc;
}

cddb_disc_t *cddb_read_device(const char *device)
{
    cddb_disc_t *disc = NULL;   /* libcddb disc structure */

    CdIo_t *cdio;               /* libcdio CD access structure */
    track_t cnt, t;             /* track counters */
    lba_t lba;                  /* Logical Block Address */
    int *foffset = NULL;        /* list of frame offsets */

    /* Get the name of the default CD-ROM device. */
    if (!device) {
        device = cdio_get_default_device(NULL);
        if (!device) {
            libcdio_cddb_error_exit("unable to get default CD device");
        }
    }

    cdio = cdio_open(device, DRIVER_UNKNOWN);
    if (!cdio) {
        libcdio_cddb_error_exit("unable to open CD device");
    }

    /* Get the track count for the CD. */
    cnt = cdio_get_num_tracks(cdio);
    if (cnt == 0) {
        libcdio_cddb_error_exit("no audio tracks on CD");
    }

    foffset = (int*)calloc(cnt, sizeof(int));
    for (t = 1; t <= cnt; t++) {
        if (cdio_get_track_format(cdio, t) != TRACK_FORMAT_AUDIO) {
            libcdio_cddb_error_exit("track %d is not an audio track", t);
        }

        lba = cdio_get_track_lba(cdio, t);
        if (lba == CDIO_INVALID_LBA) {
            libcdio_cddb_error_exit("track %d has invalid Logical Block Address", t);
        }

        foffset[t - 1] = lba;
    }

    lba = cdio_get_track_lba(cdio, CDIO_CDROM_LEADOUT_TRACK);
    if (lba == CDIO_INVALID_LBA) {
        libcdio_cddb_error_exit("LEADOUT_TRACK has invalid Logical Block Address");
    }

    disc = cddb_create(FRAMES_TO_SECONDS(lba), cnt, foffset, 0);

    cdio_destroy(cdio);
    FREE_NOT_NULL(foffset);

    return disc;
}

cddb_disc_t *cddb_create(int dlength, int tcount, int *foffset, int use_time)
{
    int i;
    cddb_disc_t *disc;
    cddb_track_t *track;

    disc = cddb_disc_new();

    if (disc) {
        cddb_disc_set_length(disc, dlength);

        for (i = 0; i < tcount; i++) {
            track = cddb_track_new();

            if (!track) {
                cddb_disc_destroy(disc);
                return NULL;
            }

            cddb_disc_add_track(disc, track);

            if (use_time) {
                cddb_track_set_length(track, foffset[i]);
            } else {
                cddb_track_set_frame_offset(track, foffset[i]);
            }
        }
    }
    return disc;
}

void cddb_query_db(cddb_conn_t *conn, cddb_disc_t *disc) {
  cddb_disc_calc_discid(disc);

  if (cddb_query(conn, disc) == -1) {
    cddb_error_exit(EXIT_FAILURE, "Query gone wrong!");
  }
}

void cddb_error_exit(int err, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    exit(err);
}
