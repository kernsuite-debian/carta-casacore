#ifndef _LOGSINK_XML_LOGSINK_CMPT_
#define _LOGSINK_XML_LOGSINK_CMPT_
/******************** generated by xml-casa (v2) from logsink.xml *******************
********************* 2c61a1398d9ef908298f9bf297e60b85 *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <logsink_forward.h>


using namespace std;

namespace casac {

  class  logsink  {
    public:

      logsink(const string& _filename=string("casa.log"));
      bool origin(const string& _fromwhere=string(""));
      bool processorOrigin(const string& _fromwhere=string(""));
      bool filter(const string& _level=string("DEBUG2"));
      void filterMsg(const std::vector<std::string>& _msgList=std::vector<std::string>({}));
      void clearFilterMsgList();
      bool post(const string& _message=string(""), const string& _priority=string("INFO"), const string& _origin=string(""));
      bool postLocally(const string& _message=string(""), const string& _priority=string("INFO"), const string& _origin=string(""));
      string localId();
      string version();
      string id();
      bool setglobal(bool _isglobal=bool(true));
      bool setlogfile(const string& _filename=string("casapy.log"));
      bool showconsole(bool _onconsole=bool(false));
      string logfile();
      int ompNumThreadsTest();
      int ompGetNumThreads();
      bool ompSetNumThreads(int _numThreads=int(1));
      int setMemoryTotal(int _memory=int(0));
      int setMemoryFraction(int _memfrac=int(0));
      int setNumCPUs(int _cores=int(0));
      int getMemoryTotal(bool _use_aipsrc=bool(true));
      int getNumCPUs(bool _use_aipsrc=bool(true));

        ~logsink( );

    private:

#include <logsink_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif
