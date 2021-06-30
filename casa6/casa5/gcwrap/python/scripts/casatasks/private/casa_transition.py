from __future__ import absolute_import

from sys import version_info

# used while CASA 5 and CASA 6 development overlap - CASA5 version
is_CASA6 = False

# allow easy checking against python version 2 vs 3
is_python3 = version_info > (3,)
