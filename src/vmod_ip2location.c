#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <IP2Location.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "vcc_if.h"
#include "vmod_ip2location.h"

#define VMOD_LOG(...) WSP(sp, SLT_VCL_Log, __VA_ARGS__);

void
i2pl_free(void *d)
{
  ip2location_data_t *data = d;

  if (data->ip2l_handle != NULL)
    IP2Location_close(data->ip2l_handle);
}

/*
 * reload the IP2Location database if the file on disk is
 * more recent than the one we loaded, or if it has not been
 * loaded yet.
 */
void
reload_db(ip2location_data_t *data)
{
  struct stat s;

  if (stat(IP2LOCATION_DB_PATH, &s) == 0) {
    if (data->ip2l_handle == NULL || s.st_mtime > data->ip2l_db_ts) {
      // try to obtain the lock to reload the database.
      // if we can't, it means another thread is already reloading the database.
      if (pthread_mutex_trylock(&data->lock) == 0) {
        if (data->ip2l_handle != NULL)
          IP2Location_close(data->ip2l_handle);
        data->ip2l_handle = IP2Location_open(IP2LOCATION_DB_PATH);
        IP2Location_open_mem(data->ip2l_handle, IP2LOCATION_CACHE_MEMORY);
        data->ip2l_db_ts = s.st_mtime;
	pthread_mutex_unlock(&data->lock);
      }
    }
  }
}


int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
	struct stat s;

        ip2location_data_t *data = calloc(1, sizeof(*data));
        data->ip2l_handle = NULL;
	pthread_mutex_init(&data->lock, NULL);
        reload_db(data);
        priv->priv = data;
        priv->free = i2pl_free;
	return (0);
}

const char *
vmod_country_short(struct sess *sp, struct vmod_priv *priv, const char *ip)
{
	IP2LocationRecord *r;
	IP2Location *handle;
	const char *code = NULL;

	reload_db(priv->priv);
        handle = ((ip2location_data_t *)priv->priv)->ip2l_handle;
        if (handle != NULL) {
		r = IP2Location_get_country_short(handle, (char *)ip);
		if (r != NULL) {
			code = WS_Dup(sp->wrk->ws, r->country_short);
        		IP2Location_free_record(r);
			return (code);
		}
	}

	VMOD_LOG("ERROR: IP2Location database failed to load");
	code = WS_Dup(sp->wrk->ws, "-");

	return (code);
}

const char *
vmod_region(struct sess *sp, struct vmod_priv *priv, const char *ip)
{
	IP2LocationRecord *r;
	IP2Location *handle;
	const char *code = NULL;

	reload_db(priv->priv);
        handle = ((ip2location_data_t *)priv->priv)->ip2l_handle;
        if (handle != NULL) {
		r = IP2Location_get_region(handle, (char *)ip);
                if (r != NULL) {
			code = WS_Dup(sp->wrk->ws, r->region);
        		IP2Location_free_record(r);
			return (code);
                }
	}

	VMOD_LOG("ERROR: IP2Location database failed to load");
	code = WS_Dup(sp->wrk->ws, "-");

	return (code);
}
