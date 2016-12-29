#ifndef __CDDB_HELPER__
#define __CDDB_HELPER__

#include <cddb/cddb.h>

void cddb_error_exit(int err, const char *fmt, ...);
void cddb_query_db(cddb_conn_t *conn, cddb_disc_t *disc);
cddb_disc_t *cddb_read_db(cddb_conn_t *conn, const char *category, int discid, int quiet);
cddb_disc_t *cddb_read_device(const char *device);
cddb_disc_t *cddb_create(int dlength, int tcount, int *foffset, int use_time);

#endif
