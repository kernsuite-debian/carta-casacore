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
# Test suite for the CASA tool method ia.pv()
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
# Test for the ia.pv() tool method
# </etymology>
#
# <synopsis>
# Test the ia.pv() tool method
# </synopsis> 
#
# <example>
#
# This test runs as part of the CASA python unit test suite and can be run from
# the command line via eg
# 
# `echo $CASAPATH/bin/casa | sed -e 's$ $/$'` --nologger --log2term -c `echo $CASAPATH | awk '{print $1}'`/code/xmlcasa/scripts/regressions/admin/runUnitTest.py test_ia_pv[test1,test2,...]
#
# </example>
#
# <motivation>
# To provide a test standard for the ia.pv() tool method to ensure
# coding changes do not break the associated bits 
# </motivation>
#

###########################################################################

import os
import shutil
import numpy
import unittest

try:
    from casatools import image as iatool
    from casatools import quanta
    from casatools import table, ctsys
    ctsys_resolve = ctsys.resolve
except ImportError:
    from __main__ import default
    from tasks import *
    from taskinit import *
    def ctsys_resolve(apath):
        dataPath = os.path.join(os.environ['CASAPATH'].split()[0],'data')
        return os.path.join(dataPath,apath)

_tb = table( )

datapath = ctsys_resolve('regression/unittest/imageanalysis/ImageAnalysis/')

def run_ia_pv(
    imagename, outfile, start, end, width,
    center=[], length=[], pa=[]
):
    myia = iatool()
    myia.open(imagename)
    if (not myia.isopen()):
        myia.done()
        raise Exception
    res = myia.pv(
        outfile=outfile, start=start, end=end, width=width,
        center=center, length=length, pa=pa
    )
    myia.done()
    return res

class ia_pv_test(unittest.TestCase):
    
    def setUp(self):
        self.ia = iatool()
    
    def tearDown(self):
        #pass
        self.assertTrue(len(_tb.showcache()) == 0)
    
    def test_pv(self):
        """ ia.pv(): Test pv()"""
        myia = self.ia
        imagename = "zxye.im"
        myia.fromshape(imagename, [10, 10, 10])
        bb = myia.getchunk()
        # basic sanity test, no rotation involved
        for i in range(10):
            bb[i,5,:] = i
            bb[i,0:5,:] = i+1
            bb[i,6:10,:] = i+2
        myia.putchunk(bb)
        expeccoord = myia.toworld([1,5,0])['numeric'][2]
        mycsys = myia.coordsys()
        units = mycsys.units()
        expinc = mycsys.increment()["numeric"]
        qa = quanta()
        expinc = [
            abs(
                qa.convert(
                    qa.quantity(expinc[0], units[0]), "arcsec"
                )["value"]
            ),
            expinc[2]
        ]
        myia.done()
        self.assertTrue(len(_tb.showcache())== 0)
        pv = iatool()
        # no width
        for i in range(7):
            if i == 0:
                start = [2, 5]
                end = [7, 5]
                mode = "coords"
            elif i == 1:
                start = ["3.00000038arcmin", "0'"]
                end = ["2.15980000e+04'", "0arcmin"]
                mode = "coords"
            if i == 2:
                start = ["0h0m12s", "0d0m0s" ]
                end = ["23:59:52", "0.0.0"]
                mode = "coords"
            if i == 3:
                center = [4.5, 5]
                length = 5
                pa = "90deg"
                mode = "length"
            if i == 4:
                center = ["0:0:02", "0.0.0"]
                length = 5
                pa = "90deg"
                mode = "length"
            if i == 5:
                center = ["0:0:02", "0.0.0"]
                length = "5arcmin"
                pa = "90deg"
                mode = "length"
            if i == 6:
                center = [4.5, 5]
                length = "5arcmin"
                pa = "90deg"
                mode = "length"
            outfile = "test_pv_" + str(i)
            if i <= 2:
                xx = run_ia_pv(
                    imagename=imagename, outfile=outfile, start=start,
                    end=end, width=1
                )
            else:
                xx = run_ia_pv(
                    imagename=imagename, outfile=outfile, start=[],
                    end=[], width=1, center=center, length=length,
                    pa=pa
                )
            ia = iatool()
            if (type(xx) == type(ia)):
                xx.done()
            self.assertTrue(len(_tb.showcache())== 0)
            pv.open(outfile)
            expec = [6, 10]
            got = pv.shape()
            self.assertTrue((got == expec).all())
            expec = numpy.zeros(got)
            for i in range(10):
                expec[:,i] = range(2,8)
            got = pv.getchunk()
            self.assertTrue((got == expec).all())
            self.assertTrue(pv.getchunk(getmask=True).all())
            got = pv.toworld([0,0,0])['numeric'][1]
            self.assertTrue(abs(got - expeccoord) < 1e-6)
            gotinc = pv.coordsys().increment()["numeric"]
            # the position offset axis always has units of arcsec, the units
            # in the input image were arcmin
            self.assertTrue((abs(gotinc - expinc) < 1e-5).all())
            pv.done()
            
        # width > 1
        for i in range(5):
            outfile = "test_pv_1_" + str(i)
            if i == 0:
                width = 3;
            elif i == 1:
                width = "3arcmin"
            elif i == 2:
                width = "1.1arcmin"
            elif i == 3:
                width = qa.quantity("1.2arcmin")
            elif i == 4:
                # width units different from axis units, CAS-5975
                width = qa.quantity("72000marcsec")
            xx = run_ia_pv(
                imagename=imagename, outfile=outfile, start=[2, 5],
                end=[7, 5], width=width
            )
            if (type(xx) == type(ia)):
                xx.done()
            pv.open(outfile)
            expec = [6, 10]
            got = pv.shape()
            self.assertTrue((got == expec).all())
            expec = numpy.zeros(got)
            for i in range(10):
                expec[:,i] = range(3,9)
            got = pv.getchunk()
            self.assertTrue((got == expec).all())
            self.assertTrue(pv.getchunk(getmask=True).all())
            pv.done()
        
    def test_stretch(self):
        """ia.pv(): Test stretch parameter"""
        yy = iatool()
        mymask = "maskim"
        yy.fromshape(mymask, [200, 200, 1, 1])
        yy.addnoise()
        yy.done()
        shape = [200,200,1,20]
        yy.fromshape("kk", shape)
        yy.addnoise()
        self.assertRaises(
            Exception,
            yy.pv, start=[2,2], end=[20,2],
            mask=mymask + ">0", stretch=False
        )
        zz = yy.pv(
            start=[2,2], end=[20,2], mask=mymask + ">0", stretch=True
        )
        mytype = type(yy)
        self.assertTrue(zz and type(zz) == mytype)
        yy.done()
        zz.done()
    
    def test_CAS_2996(self):
        """ia.pv(): Test issues raised in CAS-2996"""
        # the only tests necessary here are to ensure ia.pv() runs 
        # successfully for the provided inputs
        # calculate stats to make sure region determination code doesn't segfault (CAS-4881)
        myia = self.ia
        myia.open(datapath + "pv1.im")
        xx = myia.pv(start = [30, 30], end = [250, 250])
        xx.statistics()
        xx = myia.pv(start = [30, 250], end = [250, 30])
        xx.statistics()
        xx = myia.pv(start = [250, 250], end = [30, 30])
        xx.statistics()
        xx = myia.pv(start = [250, 30], end = [30, 250])
        xx.statistics()

        myia.open(datapath + "pv2.im")
        x1 = 264.865854
        x2 = 166.329268
        y1 = 142.914634
        y2 = 232.670732
        # success or failure should not depend on the end pixel's location relative to the start pixel.
        xx = myia.pv(start=[x1, y1], end=[x2, y2])
        # test units while we're at it
        self.assertTrue(xx.coordsys().units()[0] == "arcsec")
        xx = myia.pv(start=[x2, y1], end=[x1, y2])
        xx = myia.pv(start=[x2, y2], end=[x1, y1])
        xx = myia.pv(start=[x1, y2], end=[x2, y1], unit="rad")
        self.assertTrue(xx.coordsys().units()[0] == "rad")

        myia.done()
        xx.done()
        
    def test_fits(self):
        """ia.pv(): Test exporting and importing to/from FITS"""
        myia = self.ia
        myia.open(datapath + "pv1.im")
        xx = myia.pv(start = [30, 30], end = [250, 250])
        expec = ["OFFSET", "Frequency", "Stokes"]
        outfile = "zz.fits"
        xx.tofits(outfile)
        myia.open(outfile)
        got = myia.coordsys().names()
        self.assertTrue(got == expec)
        xx.tofits(outfile, velocity=True, overwrite=True)
        xx.done()
        myia.open(outfile)
        got = myia.coordsys().names()
        myia.done()
        self.assertTrue(got == expec)
        
    def test_refpix_far_outside_image(self):
        """Test refpix far outside image doesn't lead to malloc error, CAS-5251"""
        myia = self.ia
        myia.fromshape("",[50,50,1000])
        csys = myia.coordsys()
        csys.setreferencepixel([2000, 2000, 500])
        myia.setcoordsys(csys.torecord())
        pv = myia.pv(start=[5,5], end=[10,10])

    def test_history(self):
        """Verify history is written to created image"""
        myia = self.ia
        imagename = "zz.im"
        myia.fromshape(imagename, [30, 30, 30])
        length = "14arcmin"
        center = [15, 15]
        bb = myia.pv("", center=center, length=length, pa="45deg")
        myia.done()
        msgs = bb.history()
        bb.done()
        teststr = "ia.pv"
        self.assertTrue(teststr in msgs[-2], "'" + teststr + "' not found")
        self.assertTrue(teststr in msgs[-1], "'" + teststr + "' not found")
       
    def test_CAS10968(self):
        """Fix for pa=90,270 when segment y pixel falls on half pixel"""
        myia = self.ia
        myia.fromshape("", [30, 30, 30])
        ary = myia.getchunk()
        for i in range(30):
            ary[i, :, :] = i
        myia.putchunk(ary)
        length = "14arcmin"
        center = [15.5, 15.5]
        bb = myia.pv("", center=center, length=length, pa="90deg")
        ary = bb.getchunk()
        bb.done()
        for i in range(14):
            self.assertTrue((ary[i, :] == i+9).all(), "incorrect values for pa=90 deg")
        bb = myia.pv("", center=center, length=length, pa="270deg")
        ary = bb.getchunk()
        bb.done()
        myia.done()
        for i in range(14):
            self.assertTrue(numpy.isclose(ary[i, :], 22-i).all(), "incorrect values for pa=270 deg")

def suite():
    return [ia_pv_test]

if __name__ == '__main__':
    unittest.main()

