/******************** generated by xml-casa (v2) from calanalysis.xml ***************
********************* 78255864a2b8535063cf2fc5161fc628 *****************************/
%module calanalysis
%include <casa_typemaps.i>
%feature("kwargs");
%feature("autodoc", "0");

%feature("docstring", "

Summary:
     Open a calibration table.

Description:


This member function opens a calibration table.

Input Parameters:
    caltable                   Python string containing the calibration table name. 

Example:

ca.open( '<caltable name>' )

--------------------------------------------------------------------------------
") open;

%feature("docstring", "

Summary:
     Close a calibration table.

Description:


This member function closes a calibration table.

Example:

ca.close()

--------------------------------------------------------------------------------
") close;

%feature("docstring", "

Summary:
     Return the calibration table name.

Description:


This member function returns calibration table name.

Example:

caltable = ca.calname()

--------------------------------------------------------------------------------
") calname;

%feature("docstring", "

Summary:
     Return the name of the MS that created this calibration table.

Description:


This member function returns the name of the MS that created this calibration
table.

Example:

msname = ca.msname()

--------------------------------------------------------------------------------
") msname;

%feature("docstring", "

Summary:
     Return the type of calibration table ('B', 'G', 'T', etc.).

Description:


This member function returns the type of calibration table ('B', 'G', 'T',
etc.).

Example:

viscal = ca.viscal()

--------------------------------------------------------------------------------
") viscal;

%feature("docstring", "

Summary:
     Return the parameter column type in the calibration table ('Complex' or 'Float').

Description:


This member function returns the parameter column type in the calibration table
('Complex' or 'Float').

Example:

partype = ca.partype()

--------------------------------------------------------------------------------
") partype;

%feature("docstring", "

Summary:
     Return the polarization basis in the calibration table ('L' for linear or 'C' for circular).

Description:


This member function returns the polarization basis in the calibration table
('L' for linear or 'C' for circular).

Example:

polbasis = ca.polbasis()

--------------------------------------------------------------------------------
") polbasis;

%feature("docstring", "

Summary:
     Return the number of fields in the calibration table.

Description:


This member function returns the number of fields in the calibration table.

Example:

numfield = ca.numfield()

--------------------------------------------------------------------------------
") numfield;

%feature("docstring", "

Summary:
     Return the fields in the calibration table.

Description:


This member function returns the fields in the calibration table.

Input Parameters:
    name                       The python boolean which determines whether field names (True) or field numbers (False) are returned. 

Example:

field = ca.field()

--------------------------------------------------------------------------------
") field;

%feature("docstring", "

Summary:
     Return the number of antennas in the calibration table.

Description:


This member function returns the number of antennas in the calibration table.

Example:

numantenna = ca.numantenna()

--------------------------------------------------------------------------------
") numantenna;

%feature("docstring", "

Summary:
     Return the number of antenna 1s in the calibration table.

Description:


This member function returns the number of antenna 1s in the calibration table.

Example:

numantenna1 = ca.numantenna1()

--------------------------------------------------------------------------------
") numantenna1;

%feature("docstring", "

Summary:
     Return the number of antenna 2s in the calibration table.

Description:


This member function returns the number of antenna 2s in the calibration table.

Example:

numantenna2 = ca.numantenna2()

--------------------------------------------------------------------------------
") numantenna2;

%feature("docstring", "

Summary:
     Return the antennas in the calibration table.

Description:


This member function returns the antennas in the calibration table.

Input Parameters:
    name                       The python boolean which determines whether antenna names (True) or antenna numbers (False) are returned. 

Example:

antenna = ca.antenna()

--------------------------------------------------------------------------------
") antenna;

%feature("docstring", "

Summary:
     Return the antenna 1s in the calibration table.

Description:


This member function returns the antenna 1s in the calibration table.

Input Parameters:
    name                       The python boolean which determines whether antenna 1 names (True) or antenna 1 numbers (False) are returned. 

Example:

antenna1 = ca.antenna1()

--------------------------------------------------------------------------------
") antenna1;

%feature("docstring", "

Summary:
     Return the antenna 2s in the calibration table.

Description:


This member function returns the antenna 2s in the calibration table.

Input Parameters:
    name                       The python boolean which determines whether antenna 2 names (True) or antenna 2 numbers (False) are returned. 

Example:

antenna2 = ca.antenna2()

--------------------------------------------------------------------------------
") antenna2;

%feature("docstring", "

Summary:
     Return the number of feeds in the calibration table.

Description:


This member function returns the number of feeds in the calibration table.

Example:

numfeed = ca.numfeed()

--------------------------------------------------------------------------------
") numfeed;

%feature("docstring", "

Summary:
     Return the feeds in the calibration table.

Description:


This member function returns the feeds in the calibration table.

Example:

feed = ca.feed()

--------------------------------------------------------------------------------
") feed;

%feature("docstring", "

Summary:
     Return the number of times in the calibration table.

Description:


This member function returns the number of times in the calibration table.

Example:

numtime = ca.numtime()

--------------------------------------------------------------------------------
") numtime;

%feature("docstring", "

Summary:
     Return the times (in MJD seconds) in the calibration table.

Description:


This member function returns the times (in MJD seconds) in the calibration
table.

Example:

time = ca.time()

--------------------------------------------------------------------------------
") time;

%feature("docstring", "

Summary:
     Return the number of spectral windows in the calibration table.

Description:


This member function returns the number of spectral windows in the calibration
table.

Example:

numspw = ca.numspw()

--------------------------------------------------------------------------------
") numspw;

%feature("docstring", "

Summary:
     Return the spectral windows in the calibration table.

Description:


This member function returns the spectral windows in the calibration table.

Input Parameters:
    name                       The python boolean which determines whether spectral window names (True) or spectral window numbers (False) are returned. 

Example:

spw = ca.spw()

--------------------------------------------------------------------------------
") spw;

%feature("docstring", "

Summary:
     Return the number of channels per spectral window in the calibration table.

Description:


This member function returns the number of channels per spectral window in the
calibration table.

Example:

numChannel = ca.numchannel()

--------------------------------------------------------------------------------
") numchannel;

%feature("docstring", "

Summary:
     Return the frequencies per spectral window in the calibration table.

Description:


This member function returns the frequencies per spectral window in the
calibration table.

Example:

freq = ca.freq()

--------------------------------------------------------------------------------
") freq;

%feature("docstring", "

Summary:
     Return the calibration data.

Description:


This member function returns the calibration data.

Input Parameters:
    field                      The python comma-delimited string or list of strings containing the field names or numbers.  The default is '' (all fields). 
    antenna                    The python comma-delimited string or list of strings containing the antenna 1s and antenna 2s.  The default is '' (all antenna 1s and antenna 2s). 
    timerange                  The python list of floats of length two containing the start and stop times (in MJD seconds).  The default is [] (the minimum start time and the maximum stop time). 
    spw                        The python comma-delimited string containing the spectral window names and numbers along with their channel numbers.  The default is '' (all spectral windows and channels). 
    feed                       The python comma-delimited string or list of strings containing the feeds. The default is '' (all feeds). 
    axis                       The python string containing the user-specified iteration axis.  The allowed values are 'TIME' and 'FREQ'.  The default is '' ('FREQ'). 
    ap                         The python string which determines whether complex gains are converted to amplitudes or phases.  The allowed values are 'AMPLITUDE' and 'PHASE'. The default is '' ('AMPLITUDE').  This parameter is ignored when the 'gain' values in the calibration table are real. 
    norm                       The python boolean which determines whether the amplitudes are normalized along each non-iteration axis.  The default is False.  This parameter is ignored when the 'gain' values in the calibration table are real or ap='PHASE'. 
    unwrap                     The python boolean which determines whether the phases are unwrapped along each non-iteration axis.  The default is False.  This parameter is ignored when the 'gain' values in the calibration table are real or ap='AMPLITUDE'. 
    jumpmax                    The python float which determines the maximum phase jump near +/- PI before unwrapping is performed.  E.g., jumpmax = 0.1.  The default is 0.0. It is ignored if the 'gain' values in the calibration table are real or ap = 'AMPLITUDE'.  If the non-iteration axis is frequency: 1) if jumpmax == 0.0, use fringe fitting (only available when the non-iteration axis is time); 2) if jumpmax != 0.0, use simple unwrapping (same algorithm as used when the non-iteration axis is time or frequency). 

Example:

# All data limited only by the spectral window and channel input
data = ca.get( spw='0:4~15,1,2:10~20' )

--------------------------------------------------------------------------------
") get;

%feature("docstring", "

Summary:
     Return the calibration data and fits along the non-iteration axis.

Description:


This member function returns the calibration data and fits along the
non-iteration axis.

Input Parameters:
    field                      The python comma-delimited string or list of strings containing the field names or numbers.  The default is '' (all fields). 
    antenna                    The python comma-delimited string or list of strings containing the antenna 1s and antenna 2s.  The default is '' (all antenna 1s and antenna 2s). 
    timerange                  The python list of floats of length two containing the start and stop times (in MJD seconds).  The default is [] (the minimum start time and the maximum stop time). 
    spw                        The python comma-delimited string containing the spectral window names and numbers along with their channel numbers.  The default is '' (all spectral windows and channels). 
    feed                       The python comma-delimited string or list of strings containing the feeds. The default is '' (all feeds). 
    axis                       The python string containing the user-specified iteration axis.  The allowed values are 'TIME' and 'FREQ'.  The default is '' ('FREQ'). 
    ap                         The python string which determines whether complex gains are converted to amplitudes or phases.  The allowed values are 'AMPLITUDE' and 'PHASE'. The default is '' ('AMPLITUDE').  This parameter is ignored when the 'gain' values in the calibration table are real. 
    norm                       The python boolean which determines whether the amplitudes are normalized along each non-iteration axis.  The default is False.  This parameter is ignored when the 'gain' values in the calibration table are real or ap='PHASE'. 
    unwrap                     The python boolean which determines whether the phases are unwrapped along each non-iteration axis.  The default is False.  This parameter is ignored when the 'gain' values in the calibration table are real or ap='AMPLITUDE'. 
    jumpmax                    The python float which determines the maximum phase jump near +/- PI before unwrapping is performed.  E.g., jumpmax = 0.1.  The default is 0.0. It is ignored if the 'gain' values in the calibration table are real or ap = 'AMPLITUDE'.  If the non-iteration axis is frequency: 1) if jumpmax == 0.0, use fringe fitting (only available when the non-iteration axis is time); 2) if jumpmax != 0.0, use simple unwrapping (same algorithm as used when the non-iteration axis is time or frequency). 
    order                      The python string containing the fit order.  The allowed values are 'AVERAGE', 'LINEAR', and 'QUADRATIC'.  The default is '' ('AVERAGE').  NB: 'QUADRATIC' is not allowed when type = 'ROBUST'. 
    type                       The python string containing the fit type.  The allowed values are 'LSQ' and 'ROBUST'.  The default is '' ('LSQ').  NB: Robust fitting is experimental.  It flags outliers. 
    weight                     The python boolean which determines the weighting.  The default is False. 

Example:

# All data limited only by the spectral window and channel input.  The fit order
# is linear.
data_fit = ca.fit( spw='0:4~15,1,2:10~20', order='LINEAR' )

--------------------------------------------------------------------------------
") fit;

%exception {
   try {
      $action
      } catch (const casacore::AipsError &ae) {
         PyErr_SetString(PyExc_RuntimeError, ae.what());
	 //PyErr_Print();
         return NULL;
      }
}
%include "calanalysis_cmpt.h"

%{
#include <exception>
#include <calanalysis_cmpt.h>
%}

