#include "audiocd.h"

#include <string.h>

#include "cddb_helper.h"

void audiocd_read_metadata() {
  cddb_conn_t *conn;
  cddb_disc_t *disc = NULL;
  disc = cddb_read_device("/dev/sr1");

  conn = cddb_new();
  cddb_http_enable(conn);
  cddb_set_server_port(conn, 80);

  cddb_query_db(conn, disc);
  audiocd_disc = cddb_read_db(conn, cddb_disc_get_category_str(disc), cddb_disc_get_discid(disc), 0);
  audiocd_track = cddb_disc_get_track_first(audiocd_disc);
}

const char *audiocd_get_title() {
  return cddb_disc_get_title(audiocd_disc);
}

const char *audiocd_get_artist() {
  return cddb_disc_get_artist(audiocd_disc);
}

int audiocd_next_track() {
  audiocd_track = cddb_disc_get_track_next(audiocd_disc);
  if (audiocd_track) {
    return 1;
  } else {
    return 0;
  }
}

const char *audiocd_get_track_title() {
  return cddb_track_get_title(audiocd_track);
}

int audiocd_get_track_length() {
  return cddb_track_get_length(audiocd_track);
}
