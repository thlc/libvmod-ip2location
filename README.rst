============
vmod_ip2location
============

----------------------
Varnish IP2Location Module
----------------------

:Author: Thomas Lecomte
:Date: 2015-04-29
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import ip2location;

DESCRIPTION
===========

Implements bindings to IP2Location from the Varnish VCL.

FUNCTIONS
=========

hello
-----

Prototype
        ::

                country_short(STRING S)
Return value
	STRING
Description
	Returns the short country code determined from the IP.
Example
        ::

                set resp.http.X-Country = ip2l.country_short("World");

HISTORY
=======

This manual page was released as part of the libvmod-ip2locatin package.
It is based on the excellent libvmod-example package provided by Varnish
Software.
