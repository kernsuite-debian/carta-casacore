import sys
import os
import string
from locatescript import copydata
from locatescript import locatescript
import inspect

a=inspect.stack()
stacklevel=0
for k in range(len(a)):
    if (string.find(a[k][1], 'ipython console') > 0):
        stacklevel=k
        break
gl=sys._getframe(stacklevel).f_globals

# Short description
def description():
    return "Initial VLA Pipeline Regression Testing"

def data():
    ### return the data files that is needed by the regression script
    return []

def run( fetch=False ):

    #####locate the regression script
    lepath=locatescript('pipelineTest_regression_vla.py')
    print 'Script used is ',lepath
    gl['regstate']=True
    execfile(lepath, gl)
    print 'regstate =', gl['regstate']
    if not gl['regstate']:
        raise Exception, 'regstate = False'

###return the images that will be templated and compared in future runs

    return []
