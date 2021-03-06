/******************** generated by xml-casa (v2) from logsink.xml *******************
********************* 2c61a1398d9ef908298f9bf297e60b85 *****************************/
%module logsink
%include <casa_typemaps.i>
%feature("kwargs");
%feature("autodoc", "0");

%feature("docstring", "

Summary:
    Set the origin of the message

Description:

Sets the origin of messages to be displayed

Input Parameters:
    fromwhere                 The origin of a log messages

--------------------------------------------------------------------------------
") origin;

%feature("docstring", "

Summary:
    Set the CASA processor origin

Description:

Sets the CASA processor origin which is shown at the end of each log origin

Input Parameters:
    fromwhere                 Input CASA processor origin name

--------------------------------------------------------------------------------
") processorOrigin;

%feature("docstring", "

Summary:
    Set the filter level

Description:

Set the filter level of logging messages to be displayed.
This will determine what log messages go into the log file. The logger itself
can adjust what gets displayed so you could set INFO5 and then filter in the
logger everything above INFO1.

Input Parameters:
    level                     Level of messages to display to the console/log file

--------------------------------------------------------------------------------
") filter;

%feature("docstring", "

Summary:
    Add messages to the filter out list

Description:

Add messages to the filter out list

Input Parameters:
    msgList                   Array of strings identifying messages to filter out

--------------------------------------------------------------------------------
") filterMsg;

%feature("docstring", "

Summary:
    Clear list of messages to be filter out

Description:

Clear list of messages to be filter out

--------------------------------------------------------------------------------
") clearFilterMsgList;

%feature("docstring", "

Summary:
    Post a message

Description:

If the message passes the filter, write it (same as postLocally)

Input Parameters:
    message                   Message to be posted
    priority                  Priority of message to be posted
    origin                    Origin of message to be posted

--------------------------------------------------------------------------------
") post;

%feature("docstring", "

Summary:
    Post locally

Description:

If the message passes the filter, write it

Input Parameters:
    message                   Message to be posted
    priority                  Priority of message to be posted
    origin                    Origin of message to be posted

--------------------------------------------------------------------------------
") postLocally;

%feature("docstring", "

Summary:
    Get local ID

Description:

Returns the id for this class

--------------------------------------------------------------------------------
") localId;

%feature("docstring", "

Summary:
    version of CASA

Description:

Returns the version of CASA as well as sending it to the log

Example:

casalog.version()

--------------------------------------------------------------------------------
") version;

%feature("docstring", "

Summary:
    Get ID

Description:

Returns the ID of the LogSink in use

--------------------------------------------------------------------------------
") id;

%feature("docstring", "

Summary:
    Set this logger to be the global logger

Input Parameters:
    isglobal                  Use as global logger

--------------------------------------------------------------------------------
") setglobal;

%feature("docstring", "

Summary:
    Set the name of file for logger output

Input Parameters:
    filename                  filename for logger

--------------------------------------------------------------------------------
") setlogfile;

%feature("docstring", "

Summary:
    Choose to send messages to the console/terminal

Input Parameters:
    onconsole                 All messages to the console as well as log file

--------------------------------------------------------------------------------
") showconsole;

%feature("docstring", "

Summary:
    Returns the full path of the log file

Description:

Returns the full path of the log file

Example:

logfile = casalog.logfile()

--------------------------------------------------------------------------------
") logfile;

%feature("docstring", "

Summary:
    Determines the number of OpenMP threads in the current parallel region using an OpenMP reduction pragma

Example:

omp_num_thread = casalog.ompNumThreadsTest()

--------------------------------------------------------------------------------
") ompNumThreadsTest;

%feature("docstring", "

Summary:
    Returns the number of OpenMP threads in the current parallel region

Example:

omp_num_thread = casalog.ompNumThreadsTest()

--------------------------------------------------------------------------------
") ompGetNumThreads;

%feature("docstring", "

Summary:
    Specifies the number of OpenMP threads used by default in subsequent parallel regions

Input Parameters:
    numThreads                

Example:

casalog.ompSetNumThreads(2)

--------------------------------------------------------------------------------
") ompSetNumThreads;

%feature("docstring", "

Summary:
    Sets the memory total value to be returned by HostInfo

Input Parameters:
    memory                    

Example:

casalog.setMemoryTotal(4*1024)

--------------------------------------------------------------------------------
") setMemoryTotal;

%feature("docstring", "

Summary:
    Sets the memory fraction value to be returned by HostInfo

Input Parameters:
    memfrac                   

Example:

casalog.setMemoryFraction(50)

--------------------------------------------------------------------------------
") setMemoryFraction;

%feature("docstring", "

Summary:
    Sets the number of CPUs to be returned by HostInfo

Input Parameters:
    cores                     

Example:

casalog.setNumCPUs(4)

--------------------------------------------------------------------------------
") setNumCPUs;

%feature("docstring", "

Summary:
    Returns the memory total value from HostInfo

Input Parameters:
    use_aipsrc                

Example:

casalog.getMemoryTotal(true)

--------------------------------------------------------------------------------
") getMemoryTotal;

%feature("docstring", "

Summary:
    Returns the number of CPUs from HostInfo

Input Parameters:
    use_aipsrc                

Example:

casalog.getNumCPUs(true)

--------------------------------------------------------------------------------
") getNumCPUs;

%exception {
   try {
      $action
      } catch (const casacore::AipsError &ae) {
         PyErr_SetString(PyExc_RuntimeError, ae.what());
	 //PyErr_Print();
         return NULL;
      }
}
%include "logsink_cmpt.h"

%{
#include <exception>
#include <logsink_cmpt.h>
%}

