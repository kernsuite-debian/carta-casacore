#ifndef _VPMANAGER_XML_VPMANAGER_CMPT_
#define _VPMANAGER_XML_VPMANAGER_CMPT_
/******************** generated by xml-casa (v2) from vpmanager.xml *****************
********************* 5d35ac075fad57789813a8b6b1890749 *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <vpmanager_forward.h>


using namespace std;

namespace casac {

  class  vpmanager  {
    public:

      vpmanager();
      bool saveastable(const string& _tablename=string(""));
      bool loadfromtable(const string& _tablename=string(""));
      bool summarizevps(bool _verbose=bool(false));
      record* setcannedpb(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const string& _commonpb=string("DEFAULT"), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbairy(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const variant& _dishdiam=variant( ), const variant& _blockagediam=variant( ), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbcospoly(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const std::vector<double>& _coeff=std::vector<double>({-1}), const std::vector<double>& _scale=std::vector<double>({-1}), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const string& _isthispb=string("PB"), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbgauss(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const variant& _halfwidth=variant( ), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const string& _isthispb=string("PB"), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbinvpoly(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const std::vector<double>& _coeff=std::vector<double>({-1}), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const string& _isthispb=string("PB"), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbnumeric(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const std::vector<double>& _vect=std::vector<double>({-1}), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const string& _isthispb=string("PB"), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      record* setpbimage(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const string& _realimage=string(""), const string& _imagimage=string(""), const string& _compleximage=string(""), const std::vector<std::string>& _antnames=std::vector<std::string>({"*"}));
      record* setpbpoly(const string& _telescope=string("VLA"), const string& _othertelescope=string(""), bool _dopb=bool(true), const std::vector<double>& _coeff=std::vector<double>({-1}), const variant& _maxrad=variant( ), const variant& _reffreq=variant( ), const string& _isthispb=string("PB"), const variant& _squintdir=variant( ), const variant& _squintreffreq=variant( ), bool _dosquint=bool(false), const variant& _paincrement=variant( ), bool _usesymmetricbeam=bool(false));
      bool setpbantresptable(const string& _telescope=string(""), const string& _othertelescope=string(""), bool _dopb=bool(true), const string& _antresppath=string(""));
      bool reset();
      bool setuserdefault(int _vplistnum=int(-1), const string& _telescope=string(""), const string& _anttype=string(""));
      int getuserdefault(const string& _telescope=string(""), const string& _anttype=string(""));
      std::vector<std::string> getanttypes(const string& _telescope=string(""), const variant& _obstime=variant( ), const variant& _freq=variant( ), const variant& _obsdirection=variant( ));
      int numvps(const string& _telescope=string(""), const variant& _obstime=variant( ), const variant& _freq=variant( ), const variant& _obsdirection=variant( ));
      record* getvp(const string& _telescope=string(""), const string& _antennatype=string(""), const variant& _obstime=variant( ), const variant& _freq=variant( ), const variant& _obsdirection=variant( ));
      record* getvps(const string& _telescope=string(""), const std::vector<std::string>& _antennas=std::vector<std::string>({}), const variant& _obstimestart=variant( ), const variant& _obstimeend=variant( ), const variant& _minfreq=variant( ), const variant& _maxfreq=variant( ), const variant& _obsdirection=variant( ));
      bool createantresp(const string& _imdir=string(""), const string& _starttime=string(""), const std::vector<std::string>& _bandnames=std::vector<std::string>({}), const std::vector<std::string>& _bandminfreq=std::vector<std::string>({}), const std::vector<std::string>& _bandmaxfreq=std::vector<std::string>({}));
      string getrespimagename(const string& _telescope=string(""), const string& _starttime=string(""), const string& _frequency=string(""), const string& _functype=string("ANY"), const string& _anttype=string(""), const string& _azimuth=string("0deg"), const string& _elevation=string("45deg"), const string& _rectype=string(""), int _beamnumber=int(0));

        ~vpmanager( );

    private:

#include <vpmanager_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif
