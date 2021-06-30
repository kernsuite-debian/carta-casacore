/******************** generated by xml-casa (v2) from utils.xml *********************
********************* e581fa84576fcfcd247fa38ed8148354 *****************************/
%module utils
%include <casa_typemaps.i>
%feature("kwargs");
%feature("autodoc", "0");

%feature("docstring", "

Summary:
    Verifies an input record against an XML interface description of took or task

Input Parameters:
    input                     The input record to verify
    xmldescriptor             XML description, either record or url
    throwexecpt               Throw an exception if the verification fails

--------------------------------------------------------------------------------
") verify;

%feature("docstring", "

Summary:
    Sets the internal contraints records from an XML file

Input Parameters:
    xmldescriptor             XML description, either record or url

--------------------------------------------------------------------------------
") setconstraints;

%feature("docstring", "

Summary:
    Verifies an individual parameter against a prespecified constraints record

Input Parameters:
    param                     Parameter name:value pair to verify

--------------------------------------------------------------------------------
") verifyparam;

%feature("docstring", "

Summary:
    If a param is an enum it will minmatch the enum value and return the full value otherwise it just returns the value

Input Parameters:
    name                      Parameter name to expand
    value                     Value of the parameter

--------------------------------------------------------------------------------
") expandparam;

%feature("docstring", "

Summary:
    Transforms an XML string into a record

Input Parameters:
    input                     XML string, maybe a URL

--------------------------------------------------------------------------------
") torecord;

%feature("docstring", "

Summary:
    Turns a record into an xml string

Input Parameters:
    input                     name of thing to viewe
    asfile                    write the xml as a file
    filename                  if asfile is true then output filename

--------------------------------------------------------------------------------
") toxml;

%feature("docstring", "

Input Parameters:
    rcvar                     Returns the value of the rc variable given. If no value is give it returns the root directory of CASA.

--------------------------------------------------------------------------------
") getrc;

%feature("docstring", "

Input Parameters:
    tablenames                Removes tables safely

--------------------------------------------------------------------------------
") removetable;

%feature("docstring", "

Summary:
    Get information about a particular table

Description:


Currently this only returns the pid of the process locking the table (lockpid), if the lock
is permanent (lockperm), and the status (lockstatus) -- 'not in use', 'open', 'read', 'write',
or 'unknown'. However, the hope is that this will eventually return a complete description of
the table.


Input Parameters:
    tablename                 path to table

--------------------------------------------------------------------------------
") tableinfo;

%feature("docstring", "

Summary:
    get the tables locked by this process

--------------------------------------------------------------------------------
") lockedtables;

%feature("docstring", "

Summary:
    returns host information

--------------------------------------------------------------------------------
") hostinfo;

%feature("docstring", "

Summary:
    Returns detailed information about last C-level exception

Description:

Returns detailed information from the last CASA C++ exception (i.e., AipsError).  The
exception message and the stack trace (mangled; use the shell's c++filt to demangle)
from the last CASA C++ exception.  The information is from the last one generated
and may not represent an exception from the last action; c_exception_clear can be
used to remove stale information.  The information's exception might also
have been caught in the C++ code and not have been translated into a Python-level
exception.


--------------------------------------------------------------------------------
") c_exception;

%feature("docstring", "

Summary:
    Clears information about last C-level exception

Description:

Clears the CASA C++ exception information.  This allows the user to be sure that
information retrieved using c_exception is not from an exception in the
distant past.


--------------------------------------------------------------------------------
") c_exception_clear;

%feature("docstring", "

Summary:
    Initializes the crash reporter.

Description:


Initializes the crash reporter which will generate a crash report if casapy
crashes.  For reporter purposes a crash is the reception of an signal by
casapy which would normally result in the program being terminated.  This includes
segfaults, aborts, etc., plus any unhandled C++ exceptions (C++ generates an
abort signal for unhandled exceptions).  This method is intended for use by the
casapy infrastructure and should not be called by other code or by users; however,
the call will only install the crash reporter the first time it is called so any
subsequent calls should be no-ops.  Returns true if initialization occurred and
false if the crash reporter was stubbed out (i.e., symbol UseCrashReporter was
not defined).


Input Parameters:
    crashDumpDirectory        Directory to write crash dumps into.
    crashDumpPosterApplicatio Application to post crash dumps to http server.
    crashPostingUrl           URL to use when posting crash report.
    logFile                   Full name of initial logfile

--------------------------------------------------------------------------------
") _crash_reporter_initialize;

%feature("docstring", "

Summary:
    Crashes casa with segfault.

Description:


This triggers a segfault for testing the crash reporter.  Obviously you
shouldn't call this unless that's what you want.  It's in here for
development/debugging purposes and ought to be removed before you see this.


Input Parameters:
    faultType                 How to kill the program

--------------------------------------------------------------------------------
") _trigger_segfault;

%feature("docstring", "

Summary:
    initialize CASAtools

Description:


returns true if initalization was performed; returns false if initialization was already done

Input Parameters:
    default_path              directories that should constitute the default data path

--------------------------------------------------------------------------------
") initialize;

%feature("docstring", "

Summary:
    returns the default data path

Description:


Returns the default data path. This path is used unless the user has set the current path to something else using the setpath function.

--------------------------------------------------------------------------------
") defaultpath;

%feature("docstring", "

Summary:
    sets the data path to the specified list of directories

Description:


Sets the data path to the specified list of directories. Returns true if all directories were added
returns false otherwise.

Input Parameters:
    dirs                      directories that should constitute the data path

--------------------------------------------------------------------------------
") setpath;

%feature("docstring", "

Summary:
    retrieves the data path

Description:


Returns the list of directories that are currently in the data path.

--------------------------------------------------------------------------------
") getpath;

%feature("docstring", "

Summary:
    removes all directories from the data path

Description:


Removes all directories from the data path.

--------------------------------------------------------------------------------
") clearpath;

%feature("docstring", "

Summary:
    resolve a complete path from a subdirectory using the data path

Description:


If the provided path already represents a file or a directory, it is returned. If it does not,
this function tries to find a complete path by matching up this partial directory with the
elements of the data path.

Input Parameters:
    path                      path to be expanded

--------------------------------------------------------------------------------
") resolve;

%feature("docstring", "

Summary:
    retrieve registry information

Description:


returns record containing the URI for the CASAtools registry which can be used by other unix processes to access the registry

--------------------------------------------------------------------------------
") registry;

%feature("docstring", "

Summary:
    retrieve registered services

Description:


returns record containing the information about the services that have been registered with CASAtools

--------------------------------------------------------------------------------
") services;

%feature("docstring", "

Summary:
    shutdown signal from python

Description:


python is shutting down cleanup anything that is outstanding

--------------------------------------------------------------------------------
") shutdown;

%feature("docstring", "

Summary:
    returns four element vector for the version

Description:



Returns a four element vector representing the version (major, minor, patch and feature).

--------------------------------------------------------------------------------
") version;

%feature("docstring", "

Summary:
    returns the descriptive version string, e.g. DEV or REL

Description:



The descriptive string describes a particular packaged version. During a development
cycle there are different sorts of packaged distributions. For example, a development
version ('DEV') or a release version ('REL').

--------------------------------------------------------------------------------
") version_desc;

%feature("docstring", "

Summary:
    Returns the complete version description as a string.

Description:



Returns a description string that includes the version information and the descriptive string..

--------------------------------------------------------------------------------
") version_info;

%feature("docstring", "

Summary:
    Returns the complete version description as a string but without the description (i.e. git hash) string.

Description:



Returns a description string that includes the version information and the descriptive string..

--------------------------------------------------------------------------------
") version_string;

%feature("docstring", "

Summary:
    Returns the complete version description as a string.

Description:



Returns a description string that includes the version information and the descriptive string..

Input Parameters:
    comparitor                what sort of comparison to do, one of >, <, <=, >=, ==, = !=
    vec                       vector to use to compare current version number against vec

--------------------------------------------------------------------------------
") compare_version;

%exception {
   try {
      $action
      } catch (const casacore::AipsError &ae) {
         PyErr_SetString(PyExc_RuntimeError, ae.what());
	 //PyErr_Print();
         return NULL;
      }
}
%include "utils_cmpt.h"

%{
#include <exception>
#include <utils_cmpt.h>
%}
