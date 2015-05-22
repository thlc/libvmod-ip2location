================
vmod_ip2location
================

--------------------------
Varnish IP2Location Module
--------------------------

:Author: Thomas Lecomte
:Date: 2015-04-29
:Version: 1.1
:Manual section: 3

SYNOPSIS
========

import ip2location;

DESCRIPTION
===========

Implements bindings to IP2Location from the Varnish VCL.

FUNCTIONS
=========

country_short
-------------

Prototype
        ::

                country_short(STRING ip)
Return value
	STRING
Description
	Returns the short country code determined from the IP.
Example
        ::

                set req.http.X-Country = ip2location.country_short("" + client.ip);

region
------

Prototype
        ::

                region(STRING ip)
Return value
	STRING
Description
	Returns the region/state code determined from the IP.
Example
        ::

                set req.http.X-Region = ip2location.region("" + client.ip);

HISTORY
=======

This manual page was released as part of the libvmod-ip2location package.
It is based on the excellent libvmod-example package provided by Varnish
Software.
