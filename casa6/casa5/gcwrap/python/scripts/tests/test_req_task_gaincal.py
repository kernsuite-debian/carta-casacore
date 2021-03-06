##########################################################################
# test_req_task_gaincal.py
#
# Copyright (C) 2018
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
# [Add the link to the JIRA ticket here once it exists]
#
# Based on the requirements listed in plone found here:
# https://casa.nrao.edu/casadocs-devel/stable/global-task-list/task_gaincal/about
#
#
##########################################################################

CASA6 = False
try:
    import casatools
    from casatasks import gaincal, mstransform, casalog
    CASA6 = True
    tb = casatools.table()

except ImportError:
    from __main__ import default
    from tasks import *
    from taskinit import *

import sys
import os
import testhelper as th
import unittest
import shutil
import numpy as np
import pylab as pl

if CASA6:
    datapath = casatools.ctsys.resolve('visibilities/vla/gaincaltest2.ms')
    compCal = casatools.ctsys.resolve('caltables/gaincaltest2.ms.G0')
    tCal = casatools.ctsys.resolve('caltables/gaincaltest2.ms.T0')
    # Reference Cals
    combinedRef = casatools.ctsys.resolve('caltables/genDataCombine.G0')
    preTRef = casatools.ctsys.resolve('caltables/genDataPreT.G0')
    preGRef = casatools.ctsys.resolve('caltables/genDataPreG.T0')
    calModeP = casatools.ctsys.resolve('caltables/calModeTest.G0')
    calModeA = casatools.ctsys.resolve('caltables/calModeTest.G1')
    typeCalK = casatools.ctsys.resolve('caltables/gaintypek.G0')
    typeCalSpline = casatools.ctsys.resolve('caltables/gaintypeSpline.G0')
    spwMapCal = casatools.ctsys.resolve('caltables/spwMap.G0')
    # From merged test
    merged_dataset1 = casatools.ctsys.resolve('visibilities/vla/ngc5921.ms')
    merged_refcal1 = casatools.ctsys.resolve('caltables/ngc5921.ref1a.gcal')
    merged_refcal2 = casatools.ctsys.resolve('caltables/ngc5921.ref2a.gcal')
    merged_dataset2 = casatools.ctsys.resolve('visibilities/vla/ngc4826.ms')
    merged_refcal3 = casatools.ctsys.resolve('caltables/ngc4826.ref1b.gcal')
    
    
else:
    if os.path.exists(os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req'):
        datapath = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/visibilities/vla/gaincaltest2.ms'
        compCal = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/gaincaltest2.ms.G0'
        tCal = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/gaincaltest2.ms.T0'
        
        combinedRef = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/genDataCombine.G0'
        preTRef = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/genDataPreT.G0'
        preGRef = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/genDataPreG.T0'
        calModeP = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/calModeTest.G0'
        calModeA = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/calModeTest.G1'
        typeCalK = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/gaintypek.G0'
        typeCalSpline = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/gaintypeSpline.G0'
        spwMapCal = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/spwMap.G0'
        # From merged test
        merged_dataset1 = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/visibilities/vla/ngc5921.ms'
        merged_refcal1 = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/ngc5921.ref1a.gcal'
        merged_refcal2 = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/ngc5921.ref2a.gcal'
        merged_dataset2 = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/visibilities/vla/ngc4826.ms'
        merged_refcal3 = os.environ.get('CASAPATH').split()[0] + '/data/casa-data-req/caltables/ngc4826.ref1b.gcal'
        
        
    else:
        datapath = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/visibilities/vla/gaincaltest2.ms'
        compCal = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/gaincaltest2.ms.G0'
        tCal = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/gaincaltest2.ms.T0'
        
        combinedRef = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/genDataCombine.G0'
        preTRef = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/genDataPreT.G0'
        preGRef = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/genDataPreG.T0'
        calModeP = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/calModeTest.G0'
        calModeA = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/calModeTest.G1'
        typeCalK = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/gaintypek.G0'
        typeCalSpline = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/gaintypeSpline.G0'
        spwMapCal = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/spwMap.G0'
        # From merged test
        merged_dataset1 = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/visibilities/vla/ngc5921.ms/'
        merged_refcal1 = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/ngc5921.ref1a.gcal'
        merged_refcal2 = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/ngc5921.ref2a.gcal'
        merged_dataset2 = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/visibilities/vla/ngc4826.ms'
        merged_refcal3 = os.environ.get('CASAPATH').split()[0] + '/casa-data-req/caltables/ngc4826.ref1b.gcal'
        
        
fullRangeCal = 'testgaincal.cal'
maxScanCal = 'testScan.cal'
int70Cal = 'int70.cal'
int30Cal = 'int30.cal'

tempCal = 'temp.cal'
tempCal2 = 'temp2.cal'
selectCal = 'select.cal'

datacopy = 'gaincalTestCopy.ms'
merged_copy1 = 'merged_copy1.ms'
merged_copy2 = 'merged_copy2.ms'


def getparam(caltable, colname='CPARAM'):
    ''' Open a caltable and get the provided column '''

    tb.open(caltable)
    outtable = tb.getcol(colname)
    tb.close()

    return outtable


def tableComp(table1, table2, cols=[], rtol=8e-5, atol=1e-6):
    ''' Compare two caltables '''

    tableVal1 = {}
    tableVal2 = {}

    tb.open(table1)
    colname1 = tb.colnames()

    for col in colname1:
        try:
            tableVal1[col] = tb.getcol(col)
        except RuntimeError:
            pass
    tb.close()

    tb.open(table2)
    colname2 = tb.colnames()

    for col in colname2:
        try:
            tableVal2[col] = tb.getcol(col)
        except RuntimeError:
            pass
    tb.close()

    truthDict = {}

    for col in tableVal1.keys():

        try:
            truthDict[col] = np.isclose(tableVal1[col], tableVal2[col], rtol=rtol, atol=atol)
        except TypeError:
            print(col, 'ERROR in finding truth value')
            casalog.post(message=col+': ERROR in determining the truth value')

    if len(cols) == 0:
        
        truths = [[x, np.all(truthDict[x] == True)] for x in truthDict.keys()]

    else:

        truths = [[x, np.all(truthDict[x] == True)] for x in cols]

    return np.array(truths)

def change_perms(path):
    os.chmod(path, 0o777)
    for root, dirs, files in os.walk(path):
        for d in dirs:
            os.chmod(os.path.join(root,d), 0o777)
        for f in files:
            os.chmod(os.path.join(root,f), 0o777)


class gaincal_test(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        shutil.copytree(datapath, datacopy)
        shutil.copytree(merged_dataset1, merged_copy1)
        shutil.copytree(merged_dataset2, merged_copy2)
        #change permissions
        change_perms(datacopy)
        change_perms(merged_copy1)
        change_perms(merged_copy2)

        
        gaincal(vis=datacopy, caltable=fullRangeCal, combine='scan', solint='inf', field='0', refant='0',
                smodel=[1, 0, 0, 0], scan='0~9')

        gaincal(vis=datacopy, caltable=maxScanCal, solint='inf', field='0', refant='0',
                smodel=[1, 0, 0, 0], scan='0~9')

        gaincal(vis=datacopy, caltable=int70Cal, solint='70s', field='0', refant='0',
                smodel=[1, 0, 0, 0], scan='0~9')

        gaincal(vis=datacopy, caltable=int30Cal, solint='30s', field='0', refant='0',
                smodel=[1, 0, 0, 0], scan='0~9')
        
        gaincal(vis=datacopy, caltable=selectCal, solint='inf', field='0', refant='0',
                smodel=[1, 0, 0, 0], scan='2', spw='2')

    def tearDown(self):
        if os.path.exists(tempCal):
            shutil.rmtree(tempCal)
            
        if os.path.exists(tempCal2):
            shutil.rmtree(tempCal2)
        if os.path.exists('testcorrdepflags.ms'):
            shutil.rmtree('testcorrdepflags.ms')
        if os.path.exists('testcorrdepflagsF.G'):
            shutil.rmtree('testcorrdepflagsF.G')
        if os.path.exists('testcorrdepflagsT.G'):
            shutil.rmtree('testcorrdepflagsT.G')

        if os.path.exists('testspwmap.ms'):
            shutil.rmtree('testspwmap.ms')
                
        if os.path.exists('testspwmap.G0'):
            shutil.rmtree('testspwmap.G0')
    
        if os.path.exists('testspwmap.G1'):
            shutil.rmtree('testspwmap.G1')

        if os.path.exists('testspwmap.G2'):
            shutil.rmtree('testspwmap.G2')

        if os.path.exists('testspwmap.G3'):
            shutil.rmtree('testspwmap.G3')
            

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(datacopy)
        shutil.rmtree(merged_copy1)
        shutil.rmtree(merged_copy2)
        
        if os.path.exists(fullRangeCal):
            shutil.rmtree(fullRangeCal)

        if os.path.exists(maxScanCal):
            shutil.rmtree(maxScanCal)

        if os.path.exists(int70Cal):
            shutil.rmtree(int70Cal)

        if os.path.exists(int30Cal):
            shutil.rmtree(int30Cal)
            
        if os.path.exists(selectCal):
            shutil.rmtree(selectCal)

    def test_correctGains(self):
        '''
            test_correctGains
            -------------------
            
            Check that the gaincal results match a reference gaincal table
        '''

        self.assertTrue(np.all(tableComp(fullRangeCal, combinedRef)[:,1] == 'True'))
        #self.assertTrue(ch.Compare.compare_CASA_tables(fullRangeCal, combinedRef))


    def test_intervalSNR(self):
        '''
            test_intervalSNR
            ------------------
            
            Check that shorter interval times result in a lower signal to noise
        '''

        snrCombine = np.mean(getparam(fullRangeCal, 'SNR'))
        snrScans = np.mean(getparam(maxScanCal, 'SNR'))
        int70Snr = np.mean(getparam(int70Cal, 'SNR'))
        int30Snr = np.mean(getparam(int30Cal, 'SNR'))

        self.assertTrue(int30Snr < int70Snr < snrScans < snrCombine)

    def test_minSNR(self):
        '''
            test_minSNR
            -------------
            
            Check that values below the provided SNR threshold are flagged
        '''

        gaincal(vis=datacopy, caltable=tempCal, solint='30s', field='0', refant='0',
                smodel=[1, 0, 0, 0], minsnr=1000)

        flagged = getparam(tempCal, 'FLAG')

        self.assertTrue(np.all(flagged == 1))

    def test_fieldSelect(self):
        '''
            test_fieldSelect
            ------------------
            
            Check that the field selection parameter functions properly
        '''

        fields = getparam(fullRangeCal, 'FIELD_ID')

        self.assertTrue(np.all(fields == 0))

    def test_refantSelect(self):
        '''
            test_refantSelect
            -------------------
            
            Check that the refant selection functions properly
        '''

        refants = getparam(fullRangeCal, 'ANTENNA2')

        self.assertTrue(np.all(refants == 0))

    def test_scanSelect(self):
        '''
            test_scanSelect
            -----------------
            
            Check that the scan selection functions properly
        '''

        scans = getparam(selectCal, 'SCAN_NUMBER')

        self.assertTrue(np.all(scans == 2))

    def test_spwSelect(self):
        '''
            test_spwSelect
            ----------------
            
            Check that the spw selection parameter functions properly
        '''

        spws = getparam(selectCal, 'SPECTRAL_WINDOW_ID')

        self.assertTrue(np.all(spws == 2))

    def test_refantDiff(self):
        '''
            test_refantDiff
            -----------------
            
            Check that selecting refant will cause that refant to be set at 0
        '''

        gaincal(vis=datacopy, caltable=tempCal, solint='inf', field='0', combine='scan', refant='1',
                smodel=[1, 0, 0, 0])

        gAmp = getparam(tempCal)
        refs = [[np.mean(gAmp.imag[j,0,i::10]) for i in range(10)] for j in range(2)]

        self.assertTrue(np.isclose(refs[0][1], 0) and np.isclose(refs[1][1], 0))

    def test_preapplyT0(self):
        '''
            test_preapplyT0
            -----------------
            
            Check that pre applying the T table results in the regular G table calibration
        '''

        gaincal(vis=datacopy, caltable=tempCal, refant='0', solint='inf', smodel=[1, 0, 0, 0], gaintype='G', field='0', combine='scan',
                  gaintable=[tCal])
        
        self.assertTrue(np.all(tableComp(preTRef, tempCal)[:,1] == 'True'))

    def test_preapplyG0(self):
        '''
            test_preapplyG0
            -----------------
            
            Check that pre applying the G table results in the regular T table calibration
        '''

        gaincal(vis=datacopy, caltable=tempCal, refant='0', solint='int', smodel=[1, 0, 0, 0],
                gaintype='T', gaintable=[compCal])
        
        self.assertTrue(np.all(tableComp(preGRef, tempCal)[:,1] == 'True'))
        
    def test_antennaSelect(self):
        '''
            test_antennaSelect
            --------------------
            
            Check that antennas that aren't selected are flagged
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, refant='0', field='0', solint='inf', combine='scan', antenna='0~5&', smodel=[1,0,0,0], gaintype='G')
        
        flags = getparam(tempCal, colname='FLAG')
        flagnum = np.sum(flags)
        
        self.assertTrue(flagnum == 32)
        
    def test_minBl(self):
        '''
            test_minBl
            ------------
            
            Check that if the min baseline threshold isn't met those antennas aren't used. If no antennas have enough then a file is not written.
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, refant='0', solint='int', smodel=[1,0,0,0], gaintype='G', combine='scan', antenna='0~5&', minblperant=6)
        
        self.assertFalse(os.path.exists(tempCal))
        
    def test_preboth(self):
        '''
            test_preboth
            --------------
            
            Check that when preapplying both then gaintype T increases the SNR
            
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, refant='0', solint='inf', smodel=[1, 0, 0, 0], gaintype='G', field='0',
                  gaintable=[tCal, compCal], gainfield=['0','0'], interp=[''])
        
        gaincal(vis=datacopy, caltable=tempCal2, refant='0', solint='inf', smodel=[1, 0, 0, 0], gaintype='T', field='0',
                  gaintable=[tCal, compCal])
        
        SNR1 = np.mean(getparam(tempCal, colname='SNR'))
        SNR2 = np.mean(getparam(tempCal2, colname='SNR'))
        
        self.assertTrue(SNR1 < SNR2)
        
    def test_calModeP(self):
        '''
            test_calModeP
            ---------------
            
            Check that the output with calmode 'p' is equal to a reference calibration table
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, field='0', smodel=[1,0,0,0], solint='inf', combine='scan', calmode='p')
        
        self.assertTrue(np.all(tableComp(tempCal, calModeP)[:,1] == 'True'))
        
    def test_calModeA(self):
        '''
            test_calModeA
            ---------------
            
            Check that the output with calmode 'a' is equal to a reference calibration table
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, field='0', smodel=[1,0,0,0], solint='inf', combine='scan', calmode='a')
        
        self.assertTrue(np.all(tableComp(tempCal, calModeA)[:,1] == 'True'))
        
    def test_gainTypeK(self):
        '''
            test_gainTypeK
            ----------------
            
            Check that the output with gaintype k is equal to a reference calibration table
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, field='0', smodel=[1,0,0,0], solint='inf', combine='scan', gaintype='KCROSS', refant='0')
        
        self.assertTrue(np.all(tableComp(tempCal, typeCalK)[:,1] == 'True'))
        
    def test_gainTypeSpline(self):
        '''
            test_gainTypeSpline
            ----------------
            
            Check that the output with gaintype GSPLINE is equal to a reference calibration table
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, field='0', smodel=[1,0,0,0], solint='inf', combine='scan', gaintype='GSPLINE', refant='0')
        
        self.assertTrue(np.all(tableComp(tempCal, typeCalSpline)[:,1] == 'True'))
        
    def test_spwMap(self):
        '''
            test_spwMap
            -------------
            
            Check that the output with spwMap matches to a reference calibration table
        '''
        
        gaincal(vis=datacopy, caltable=tempCal, field='0', smodel=[1,0,0,0], solint='inf', combine='scan', refant='0',spwmap=[0,0,1,1])
        
        self.assertTrue(np.all(tableComp(tempCal, spwMapCal)[:,1] == 'True'))
        


        # Add more interesting test, including test of CAS-12591 fix

        tsmdata='testspwmap.ms'

        # slice out just scan 2
        mstransform(vis=datacopy,outputvis=tsmdata,scan='2',datacolumn='data')

        # Run gaincal w/ solint='inf' to get solutions for all spws
        tsmcal0='testspwmap.G0'
        gaincal(vis=tsmdata,caltable=tsmcal0,solint='inf',refant='0',smodel=[1,0,0,0])

        # change spws in tsmcal0 [0,1,2,3] to [2,3,0,1], so we can use spwmap non-trivially
        tb.open(tsmcal0,nomodify=False)
        spwid=tb.getcol('SPECTRAL_WINDOW_ID')
        spwid = [(i+2)%4 for i in spwid]
        tb.putcol('SPECTRAL_WINDOW_ID',spwid)
        tb.close()

        # Solve for gains using tsmcal0 with spwmap=[2,3,0,1], which should "undo"
        #  spwid change made above, expecting all solutions ~= (1,0)
        tsmcal1='testspwmap.G1'
        gaincal(vis=tsmdata,caltable=tsmcal1,solint='inf',refant='0',smodel=[1,0,0,0],
                gaintable=[tsmcal0],spwmap=[2,3,0,1])

        # test that output calibration is ~(1,0)
        #  gains-1.0 ~ zero (to within precision and solve convergence fuzz
        tb.open(tsmcal1)
        g1=tb.getcol('CPARAM')
        tb.close()
        self.assertTrue(np.absolute(np.mean(g1-1.0))<2e-6)


        # Run gaincal to get solutions for spw=0,1
        tsmcal2='testspwmap.G2'
        gaincal(vis=tsmdata,caltable=tsmcal2,spw='0,1',solint='inf',refant='0',smodel=[1,0,0,0])

        # Reset spwid  0,1->3,2 so we can exercise spwmap=[3,2,0,1]
        # also fix FLAG_ROW in SPECTRAL_WINDOW subtable
        tb.open(tsmcal2,nomodify=False)
        spwid=tb.getcol('SPECTRAL_WINDOW_ID')
        spwid[spwid==0]=3
        spwid[spwid==1]=2
        tb.putcol('SPECTRAL_WINDOW_ID',spwid)
        tb.close()
        tb.open(tsmcal2+'/SPECTRAL_WINDOW',nomodify=False)
        fr=tb.getcol('FLAG_ROW')
        fr=[1,1,0,0]
        tb.putcol('FLAG_ROW',fr)
        tb.close()

        # solve again with unselected spws all mapped to unavailable solutions
        #  this tests the fix for CAS-12591, wherein the solution-availability check
        #  was applying the spwmap twice, causing a mysterious exception and 
        #  failure to calibrate
        #  (In this case, if spw 2,3 are mapped twice (to 0,1), the availability check
        #   would fail)
        #  (expecting g~=(1,0) if applied solutions mapped correctly)
        tsmcal3='testspwmap.G3'
        gaincal(vis=tsmdata,caltable=tsmcal3,spw='0,1',solint='inf',refant='0',smodel=[1,0,0,0],
                gaintable=[tsmcal2],spwmap=[3,2,0,1])

        # test that output calibration is ~(1,0)
        #  gains-1.0 ~ zero (to within precision and solve convergence fuzz
        tb.open(tsmcal3)
        g3=tb.getcol('CPARAM')
        tb.close()
        self.assertTrue(np.absolute(np.mean(g1-1.0))<2e-6)



    
    def test_mergedCreatesGainTable(self):
        ''' Gaincal 1a: Default values to create a gain table '''
        
        gaincal(vis=merged_copy1, caltable=tempCal, uvrange='>0.0')
        self.assertTrue(os.path.exists(tempCal))
        
        self.assertTrue(th.compTables(tempCal, merged_refcal1, ['WEIGHT']))
        
    def test_mergedFieldSelect(self):
        ''' Gaincal 2a: Create a gain table using field selection '''
        
        gaincal(vis=merged_copy1, caltable=tempCal, uvrange='>0.0', field='0', gaintype='G', solint='int', combine='', refant='VA02')
        self.assertTrue(os.path.exists(tempCal))
        
        self.assertTrue(th.compTables(tempCal, merged_refcal2, ['WEIGHT']))
        
    def test_mergedSpwSelect(self):
        ''' Gaincal 1b: Create a gain table for an MS with many spws '''
        
        
        gaincal(vis=merged_copy2, caltable=tempCal, uvrange='>0.0', field='0,1', spw='0', gaintype='G', minsnr=2.0, refant='ANT5', solint='inf', combine='')
        self.assertTrue(os.path.exists(tempCal))
        
        self.assertTrue(th.compTables(tempCal, merged_refcal3, ['WEIGHT']))


    def test_corrDepFlags(self):
        '''
            test_corrDepFlags
            -----------------
        '''


        # This test exercises the corrdepflags parameter 
        #
        #  With corrdepflags=False (the default), one (or more) flagged correlations causes
        #  all correlations (per channel, per baseline) to behave as flagged, thereby
        #  causing both polarizations to be flagged in the output cal table
        #
        #  With corrdepflags=True, unflagged correlations will be used as normal, and
        #  only the implicated polarization will be flagged in the output cal table
        #
        #  NB: when some data are flagged, we expect solutions to change slightly,
        #      since available data is different.  For now, we are testing only the
        #      resulting flags.

        cdfdata='testcorrdepflags.ms'
        # slice out just scan 2
        mstransform(vis=datacopy,outputvis=cdfdata,scan='2',datacolumn='data')

        # modify flags in interesting corr-dep ways in scan 2 for subset of antennas
        tb.open(cdfdata,nomodify=False)

        # we modify the flags as follows:
        #  spw=0:  one antenna, one correlation (YY)
        #  spw=1:  one antenna, one correlation (XX)
        #  spw=2:  two antennas, opposite correlations
        #  spw=3:  one antenna, both cross-hands flagged

        # set flags for spw=0, antenna=3, corr=YY
        st=tb.query('SCAN_NUMBER==2 && DATA_DESC_ID==0 && (ANTENNA1==3 || ANTENNA2==3)')
        fl=st.getcol('FLAG')
        fl[3,:,:]=True
        st.putcol('FLAG',fl)
        st.close()

        # set flags for spw=1, antenna=6, corr=XX
        st=tb.query('SCAN_NUMBER==2 && DATA_DESC_ID==1 && (ANTENNA1==6 || ANTENNA2==6)')
        fl=st.getcol('FLAG')
        fl[0,:,:]=True
        st.putcol('FLAG',fl)
        st.close()

        # set flags for spw=2, antenna=2, corr=XX
        st=tb.query('SCAN_NUMBER==2 && DATA_DESC_ID==2 && (ANTENNA1==2 || ANTENNA2==2)')
        fl=st.getcol('FLAG')
        fl[0,:,:]=True
        st.putcol('FLAG',fl)
        st.close()
        # set flags for spw=2, antenna=7, corr=YY
        st=tb.query('SCAN_NUMBER==2 && DATA_DESC_ID==2 && (ANTENNA1==7 || ANTENNA2==7)')
        fl=st.getcol('FLAG')
        fl[3,:,:]=True
        st.putcol('FLAG',fl)
        st.close()

        # set flags for spw=3, antenna=8, corr=XY,YX
        st=tb.query('SCAN_NUMBER==2 && DATA_DESC_ID==3 && (ANTENNA1==8 || ANTENNA2==8)')
        fl=st.getcol('FLAG')
        fl[1:3,:,:]=True
        st.putcol('FLAG',fl)
        st.close()
        
        tb.close()
        
        # Run gaincal on scan 2, solint='inf' with corrdepflags=False
        #   expect both pols to be flagged for ants with one or more corr flagged
        cdfF='testcorrdepflagsF.G'
        gaincal(vis=cdfdata,caltable=cdfF,solint='inf',refant='0',smodel=[1,0,0,0],corrdepflags=False)

        tb.open(cdfF)
        flF=tb.getcol('FLAG')
        tb.close()

        # flag count per spw  (both pols in every case)
        self.assertTrue(np.sum(flF[:,0,0:10])==2)    
        self.assertTrue(np.sum(flF[:,0,10:20])==2)
        self.assertTrue(np.sum(flF[:,0,20:30])==4)
        self.assertTrue(np.sum(flF[:,0,30:40])==2)

        # check flags set for specific antennas, each spw  (both pols each antenna)
        self.assertTrue(np.all(flF[:,0,0:10][:,3]))        # spw 0
        self.assertTrue(np.all(flF[:,0,10:20][:,6]))       # spw 1
        self.assertTrue(np.all(flF[:,0,20:30][:,[2,7]]))   # spw 2
        self.assertTrue(np.all(flF[:,0,30:40][:,8]))       # spw 3

        # Run gaincal on scan 2, solint='inf' with corrdepflags=True
        #   expect unflagged solutions for unflagged pol
        cdfT='testcorrdepflagsT.G'
        gaincal(vis=cdfdata,caltable=cdfT,solint='inf',refant='0',smodel=[1,0,0,0],corrdepflags=True)

        tb.open(cdfT)
        flT=tb.getcol('FLAG')
        tb.close()

        # flag count per spw (one pol per antenna, at most)
        self.assertTrue(np.sum(flT[:,0,0:10])==1)
        self.assertTrue(np.sum(flT[:,0,10:20])==1)
        self.assertTrue(np.sum(flT[:,0,20:30])==2)
        self.assertTrue(np.sum(flT[:,0,30:40])==0)

        # check flags set for specific antennas, each spw (one pol per antenna, at most)
        self.assertTrue(flT[1,0,0:10][3])        # spw 0, antenna 3, pol=Y
        self.assertTrue(flT[0,0,10:20][6])       # spw 1, antenna 6, pol=X
        self.assertTrue(flT[0,0,20:30][2])       # spw 2, antenna 2, pol=X
        self.assertTrue(flT[1,0,20:30][7])       # spw 2, antenna 7, pol=Y
        # (spw 3 tested above)









def suite():
    return[gaincal_test]


if __name__ == '__main__':
    unittest.main()
