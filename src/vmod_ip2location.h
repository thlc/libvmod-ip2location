typedef struct vmod_ip2location_data {
  time_t		ip2l_db_ts;     /* timestamp of the database file */
  IP2Location		*ip2l_handle;
  pthread_mutex_t	lock;
} ip2location_data_t;

/* FIXME: put this in a variable/config/default file */
#define IP2LOCATION_DB_PATH "/usr/lib/ip2location/IP2LOCATION.BIN"
