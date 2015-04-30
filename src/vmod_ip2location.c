#include <stdlib.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"

#include "vcc_if.h"

#include <IP2Location.h>

#include "vmod_ip2location.h"

void
i2pl_free(void *d)
{
  IP2Location_close(((struct vmod_ip2location_data *)d)->ip2l_handle);
}

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
        struct vmod_ip2location_data *data = calloc(1, sizeof(*data));
        data->ip2l_handle = IP2Location_open(IP2LOCATION_DB_PATH);
        priv->priv = data;
        priv->free = i2pl_free;
	return (0);
}

const char *
vmod_country_short(struct sess *sp, struct vmod_priv *priv, const char *ip)
{
	IP2LocationRecord *r;
	const char *code;

	r = IP2Location_get_country_short(((struct vmod_ip2location_data *)priv)->ip2l_handle, (char *)ip);
	code = WS_Dup(sp->wrk->ws, r->country_short);
        IP2Location_free_record(r);

	return (code);
}
