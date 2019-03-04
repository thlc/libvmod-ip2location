#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <IP2Location.h>

#include "cache/cache.h"
#include "vmod_ip2location.h"

#define VMOD_LOG(...) VSLb(ctx->vsl, SLT_VCL_Log, __VA_ARGS__);

char * path = NULL;

void
i2pl_free(void *d)
{
  ip2location_data_t *data = d;

    if (data->ip2l_handle != NULL) {
        IP2Location_close(data->ip2l_handle);
    }
}

/**
 * reload the IP2Location database if the file on disk is more recent than the one we loaded, or if it has not been loaded yet.
 */
void
reload_db(ip2location_data_t *data)
{
    struct stat s;

    if (!data) {
        WRONG("reload_db: data is NULL");
    }

    if (stat(path, &s) == 0) {
        if (data->ip2l_handle == NULL || s.st_mtime > data->ip2l_db_ts) {
            // try to obtain the lock to reload the database.
            // if we can't, it means another thread is already reloading the database.
            if (pthread_mutex_trylock(&data->lock) == 0) {
                if (data->ip2l_handle != NULL) {
                    IP2Location_close(data->ip2l_handle);
                }

                data->ip2l_handle = IP2Location_open(path);
                IP2Location_open_mem(data->ip2l_handle, IP2LOCATION_CACHE_MEMORY);
                data->ip2l_db_ts = s.st_mtime;
                pthread_mutex_unlock(&data->lock);
            }
        }
    }
}


int
event_handler(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
    ip2location_data_t *data = NULL;

    switch (e) {
        case VCL_EVENT_LOAD:
            data = calloc(1, sizeof(*data));
            if (!data)
                WRONG("unable to allocate memory for ip2location data structure");
            data->ip2l_handle = NULL;
            pthread_mutex_init(&data->lock, NULL);
            reload_db(data);
            priv->priv = data;
            priv->free = i2pl_free;
            break;
        case VCL_EVENT_DISCARD:
            i2pl_free(priv->priv);
            break;
        case VCL_EVENT_WARM:
        case VCL_EVENT_COLD:
        default:
            break;
    }

    return (0);
}

VCL_VOID
vmod_set_path(VRT_CTX, struct vmod_priv *priv, const char *new_path)
{
    path = (char *) new_path;
}

IP2Location*
get_handle(struct vmod_priv *priv) {

    if (priv->priv == NULL) {
        WRONG("ip2location: priv->priv is NULL");
    }

    reload_db(priv->priv);

    return ((ip2location_data_t *) priv->priv)->ip2l_handle;
}

VCL_STRING
vmod_country_short(VRT_CTX, struct vmod_priv *priv, VCL_STRING ip)
{
    IP2LocationRecord *r;
    IP2Location *handle;
    const char *country_short = NULL;

    handle = get_handle(priv);

    if (handle != NULL) {
        r = IP2Location_get_country_short(handle, (char *) ip);

        if (r != NULL) {
            country_short = WS_Copy(ctx->ws, r->country_short, -1);
            IP2Location_free_record(r);

            return (country_short);
        }
    }

    VMOD_LOG("ERROR: IP2Location database failed to load");

    return WS_Copy(ctx->ws, "-", -1);
}

VCL_STRING
vmod_region(VRT_CTX, struct vmod_priv *priv, VCL_STRING ip)
{
    IP2LocationRecord *r;
    IP2Location *handle;
    const char *region = NULL;

    handle = get_handle(priv);

    if (handle != NULL) {
        r = IP2Location_get_region(handle, (char *) ip);

        if (r != NULL) {
            region = WS_Copy(ctx->ws, r->region, -1);
            IP2Location_free_record(r);

            return (region);
        }
    }

    VMOD_LOG("ERROR: IP2Location database failed to load");

    return WS_Copy(ctx->ws, "-", -1);
}

VCL_STRING
vmod_usagetype(VRT_CTX, struct vmod_priv *priv, VCL_STRING ip)
{
    IP2LocationRecord *r;
    IP2Location *handle;
    const char *usagetype = NULL;

    handle = get_handle(priv);

    if (handle != NULL) {
        r = IP2Location_get_usagetype(handle, (char *) ip);

        if (r != NULL) {
            usagetype = WS_Copy(ctx->ws, r->usagetype, -1);
            IP2Location_free_record(r);

            return (usagetype);
        }
    }

    VMOD_LOG("ERROR: IP2Location database failed to load");

    return WS_Copy(ctx->ws, "-", -1);
}
