##########################################################################
#
# Copyright (C) 2008, 2009
# Associated Universities, Inc. Washington DC, USA.
#
# This script is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
# License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
#
# Correspondence concerning AIPS++ should be adressed as follows:
#        Internet email: aips2-request@nrao.edu.
#        Postal address: AIPS++ Project Office
#                        National Radio Astronomy Observatory
#                        520 Edgemont Road
#                        Charlottesville, VA 22903-2475 USA
#
# <author>
# Dave Mehringer
# </author>
#
# <summary>
# Test suite for the CASA tool method ia.makecomplex()
# </summary>
#
# <reviewed reviwer="" date="" tests="" demos="">
# </reviewed
#
# <prerequisite>
# <ul>
# </ul>
# </prerequisite>
#
# <etymology>
# Test for the ia.makecomplex() tool method
# </etymology>
#
# <synopsis>
# Test the ia.makecomplex() tool method
# </synopsis> 
#
# <example>
#
# This test runs as part of the CASA python unit test suite and can be run from
# the command line via eg
# 
# `echo $CASAPATH/bin/casa | sed -e 's$ $/$'` --nologger --log2term -c `echo $CASAPATH | awk '{print $1}'`/code/xmlcasa/scripts/regressions/admin/runUnitTest.py test_ia_makecomplex[test1,test2,...]
#
# </example>
#
# <motivation>
# To provide a test standard for the ia.makecomplex() tool method to ensure
# coding changes do not break the associated bits 
# </motivation>
#

###########################################################################
import shutil
import unittest

from casatools import image as iatool
from casatools import table

class ia_makecomplex_test(unittest.TestCase):
    
    def setUp(self):
        self.ia = iatool()
    
    def tearDown(self):
        tb = table( )
        self.assertTrue(len(tb.showcache()) == 0)
        tb.done( )
    
    def test_history(self):
        """Verify ia.makecomplex() writes history to image"""
        myia = self.ia
        imag = "hist_zxye.im"
        shape = [20, 20]
        myia.fromshape(imag, shape)
        myia.set(10)
        myia.done()
        myia.fromshape("", shape)
        myia.set(20)
        image = "myc.im"
        self.assertTrue(myia.makecomplex(image, imag=imag))
        myia.done()
        myia.open(image)
        msgs = myia.history()
        myia.done()
        self.assertTrue("ia.makecomplex" in msgs[-2]) 
        self.assertTrue("ia.makecomplex" in msgs[-1])
   
def suite():
    return [ia_makecomplex_test]

if __name__ == '__main__':
    unittest.main()
