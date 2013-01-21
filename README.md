XBMC IPTV Simple Add-on
=====================

This is a PVR add-on for XBMC. It add support for Live TV watching and
EPG TV Guide through IPTV provided by the Internet providers in exUSSR countries.

------------------------------------------

Written by: Anton Fedchin

Based on: XBMC PVR Demo Add-on by Pulse-Eight (http://www.pulse-eight.com/)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
See the file COPYING for more information.

---------------------------------------------

ADDON COMPILATION

=============================
       Linux, OS-X, BSD
=============================

- Clone the GIT repository
- cd xbmc-addon-iptvsimple
- sh autogen.sh
- ./configure
- make dist-zip

This will prepare zip file in current folder to install the plugin via XBMC interface.

=============================
           Windows
=============================

- Install Visual C++ Express 2010 (follow the instructions on the wiki for XBMC itself)
- Clone the GIT repository
- cd xbmc-addon-iptvsimple\project\VS2010Express\buildzip.bat

This will prepare zip file in current folder to install the plugin via XBMC interface.

IMPORTANT:
Please disable *all* PVR addons *before* installing the IPTV Simple addon!

Have fun, ...