#ifndef _TABLE_XML_TABLE_CMPT_
#define _TABLE_XML_TABLE_CMPT_
/******************** generated by xml-casa (v2) from table.xml *********************
********************* 14a5aad1a3c523d174f26c900bb9548f *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <table_forward.h>


using namespace std;

namespace casac {

  class  table  {
    public:

      table();
      casac::table* fromfits(const string& _tablename=string(""), const string& _fitsfile=string(""), int _whichhdu=int(1), const string& _storage=string("standard"), const string& _convention=string("none"), bool _nomodify=bool(true), bool _ack=bool(true));
      bool fromascii(const string& _tablename=string(""), const string& _asciifile=string(""), const string& _headerfile=string(""), bool _autoheader=bool(false), const std::vector<int>& _autoshape=std::vector<int>({-1}), const string& _sep=string(""), const string& _commentmarker=string(""), int _firstline=int(0), int _lastline=int(-1), bool _nomodify=bool(true), const std::vector<std::string>& _columnnames=std::vector<std::string>({}), const std::vector<std::string>& _datatypes=std::vector<std::string>({}));
      bool open(const string& _tablename=string(""), const record& _lockoptions=initialize_record(""""), bool _nomodify=bool(true));
      bool create(const string& _tablename=string(""), const record& _tabledesc=initialize_record(""""), const record& _lockoptions=initialize_record(""""), const string& _endianformat=string(""), const string& _memtype=string(""), int _nrow=int(0), const record& _dminfo=initialize_record(""""));
      bool flush();
      bool fromASDM(const string& _tablename=string(""), const string& _xmlfile=string(""));
      bool resync();
      bool close();
      casac::table* copy(const string& _newtablename=string(""), bool _deep=bool(false), bool _valuecopy=bool(false), const record& _dminfo=initialize_record(""""), const string& _endian=string("aipsrc"), bool _memorytable=bool(false), bool _returnobject=bool(false), bool _norows=bool(false));
      bool copyrows(const string& _outtable=string(""), int _startrowin=int(0), int _startrowout=int(-1), int _nrow=int(-1));
      bool done();
      bool iswritable();
      string endianformat();
      bool lock(bool _write=bool(true), int _nattempts=int(0));
      bool unlock();
      bool datachanged();
      bool haslock(bool _write=bool(true));
      record* lockoptions();
      bool ismultiused(bool _checksubtables=bool(false));
      bool browse();
      string name();
      bool createmultitable(const string& _outputTableName=string(""), const std::vector<std::string>& _tables=std::vector<std::string>({}), const string& _subdirname=string(""));
      bool toasciifmt(const string& _asciifile=string(""), const string& _headerfile=string(""), const std::vector<std::string>& _columns=std::vector<std::string>({}), const string& _sep=string(""));
      casac::table* taql(const string& _taqlcommand=string("TaQL expression"));
      casac::table* query(const string& _query=string("String"), const string& _name=string(""), const string& _sortlist=string(""), const string& _columns=string(""), const string& _style=string(""));
      variant* calc(const string& _expr=string(""), const string& _prefix=string("using style base0, endincl, fortranorder"), bool _showtaql=bool(false));
      casac::table* selectrows(const std::vector<int>& _rownrs=std::vector<int>({}), const string& _name=string(""));
      record* info();
      bool putinfo(const record& _value=initialize_record(""""));
      bool addreadmeline(const string& _value=string(""));
      bool summary(bool _recurse=bool(false));
      std::vector<std::string> colnames();
      std::vector<int> rownumbers(const record& _tab=initialize_record(""""), int _nbytes=int(0));
      bool setmaxcachesize(const string& _columnname=string(""), int _nbytes=int(0));
      bool isscalarcol(const string& _columnname=string(""));
      bool isvarcol(const string& _columnname=string(""));
      string coldatatype(const string& _columnname=string(""));
      string colarraytype(const string& _columnname=string(""));
      int ncols();
      int nrows();
      bool addrows(int _nrow=int(1));
      bool removerows(const std::vector<int>& _rownrs=std::vector<int>({}));
      bool addcols(const record& _desc=initialize_record(""""), const record& _dminfo=initialize_record(""""));
      bool renamecol(const string& _oldname=string(""), const string& _newname=string(""));
      bool removecols(const std::vector<std::string>& _columnames=std::vector<std::string>({}));
      bool iscelldefined(const string& _columnname=string(""), int _rownr=int(0));
      variant* getcell(const string& _columnname=string(""), int _rownr=int(0));
      variant* getcellslice(const string& _columnname=string(""), int _rownr=int(0), const std::vector<int>& _blc=std::vector<int>({}), const std::vector<int>& _trc=std::vector<int>({}), const std::vector<int>& _incr=std::vector<int>({1}));
      variant* getcol(const string& _columnname=string(""), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      record* getvarcol(const string& _columnname=string(""), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      variant* getcolslice(const string& _columnname=string(""), const std::vector<int>& _blc=std::vector<int>({}), const std::vector<int>& _trc=std::vector<int>({}), const std::vector<int>& _incr=std::vector<int>({}), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      bool putcell(const string& _columnname=string(""), const std::vector<int>& _rownr=std::vector<int>({}), const variant& _thevalue=variant( ));
      bool putcellslice(const string& _columnname=string(""), int _rownr=int(0), const variant& _value=variant( ), const std::vector<int>& _blc=std::vector<int>({}), const std::vector<int>& _trc=std::vector<int>({}), const std::vector<int>& _incr=std::vector<int>({1}));
      bool putcol(const string& _columnname=string(""), const variant& _value=variant( ), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      bool putvarcol(const string& _columnname=string(""), const record& _value=initialize_record(""""), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      bool putcolslice(const string& _columnname=string(""), const variant& _value=variant( ), const std::vector<int>& _blc=std::vector<int>({}), const std::vector<int>& _trc=std::vector<int>({}), const std::vector<int>& _incr=std::vector<int>({1}), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      std::vector<std::string> getcolshapestring(const string& _columnname=string(""), int _startrow=int(0), int _nrow=int(-1), int _rowincr=int(1));
      variant* getkeyword(const variant& _keyword=variant( ));
      record* getkeywords();
      variant* getcolkeyword(const string& _columnname=string(""), const variant& _keyword=variant( ));
      variant* getcolkeywords(const string& _columnname=string(""));
      bool putkeyword(const variant& _keyword=variant( ), const variant& _value=variant( ), bool _makesubrecord=bool(false));
      bool putkeywords(const record& _value=initialize_record(""""));
      bool putcolkeyword(const string& _columnname=string(""), const variant& _keyword=variant( ), const variant& _value=variant( ));
      bool putcolkeywords(const string& _columnname=string(""), const record& _value=initialize_record(""""));
      bool removekeyword(const variant& _keyword=variant( ));
      bool removecolkeyword(const string& _columnname=string(""), const variant& _keyword=variant( ));
      record* getdminfo();
      std::vector<std::string> keywordnames();
      std::vector<std::string> fieldnames(const string& _keyword=string(""));
      std::vector<std::string> colkeywordnames(const string& _columnname=string(""));
      std::vector<std::string> colfieldnames(const string& _columnname=string(""), const string& _keyword=string(""));
      record* getdesc(bool _actual=bool(true));
      record* getcoldesc(const string& _columnname=string(""));
      bool ok();
      bool clearlocks();
      bool listlocks();
      record* statistics(const string& _column=string(""), const string& _complex_value=string(""), bool _useflags=bool(true));
      std::vector<std::string> showcache(bool _verbose=bool(true));
      bool testincrstman(const string& _column=string(""));

        ~table( );

    private:

#include <table_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif
