#ifndef _COORDSYS_XML_COORDSYS_CMPT_
#define _COORDSYS_XML_COORDSYS_CMPT_
/******************** generated by xml-casa (v2) from coordsys.xml ******************
********************* 6ccf797181fd89300336ce29536b68f8 *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <coordsys_forward.h>


using namespace std;

namespace casac {

  class  coordsys  {
    public:

      coordsys();
      casac::coordsys* newcoordsys(bool _direction=bool(false), bool _spectral=bool(false), const std::vector<std::string>& _stokes=std::vector<std::string>({"I","Q","U","V","XX","YY","XY","YX","RR","LL","RL","LR",""}), int _linear=int(0), bool _tabular=bool(false));
      bool addcoordinate(bool _direction=bool(false), bool _spectral=bool(false), const std::vector<std::string>& _stokes=std::vector<std::string>({"I","Q","U","V","XX","YY","XY","YX","RR","LL","RL","LR",""}), int _linear=int(0), bool _tabular=bool(false));
      std::vector<int> axesmap(bool _toworld=bool(true));
      std::vector<std::string> axiscoordinatetypes(bool _world=bool(true));
      string conversiontype(const string& _type=string("direction"));
      std::vector<double> convert(const std::vector<double>& _coordin=std::vector<double>({}), const std::vector<bool>& _absin=std::vector<bool>({true}), const string& _dopplerin=string("radio"), const std::vector<std::string>& _unitsin=std::vector<std::string>({"Native"}), const std::vector<bool>& _absout=std::vector<bool>({true}), const string& _dopplerout=string("radio"), const std::vector<std::string>& _unitsout=std::vector<std::string>({"Native"}), const std::vector<int>& _shape=std::vector<int>({-1}));
      record* convertdirection(const string& _frame=string(""));
      variant* convertmany(const variant& _coordin=variant( ), const std::vector<bool>& _absin=std::vector<bool>({true}), const string& _dopplerin=string("radio"), const std::vector<std::string>& _unitsin=std::vector<std::string>({"Native"}), const std::vector<bool>& _absout=std::vector<bool>({true}), const string& _dopplerout=string("radio"), const std::vector<std::string>& _unitsout=std::vector<std::string>({"Native"}), const std::vector<int>& _shape=std::vector<int>({-1}));
      std::vector<std::string> coordinatetype(int _which=int(-1));
      casac::coordsys* copy();
      bool done();
      record* epoch();
      record* findaxis(bool _world=bool(true), int _axis=int(0));
      int findaxisbyname(const string& _axisname=string(""), bool _allowfriendlyname=bool(true));
      record* findcoordinate(const string& _type=string("direction"), int _which=int(0));
      std::vector<double> frequencytofrequency(const std::vector<double>& _value=std::vector<double>({}), const string& _frequnit=string(""), const variant& _velocity=variant( ));
      std::vector<double> frequencytovelocity(const std::vector<double>& _value=std::vector<double>({}), const string& _frequnit=string(""), const string& _doppler=string("radio"), const string& _velunit=string("km/s"));
      bool fromrecord(const record& _record=initialize_record(""""));
      record* increment(const string& _format=string("n"), const string& _type=string(""));
      variant* lineartransform(const string& _type=string(""));
      std::vector<std::string> names(const string& _type=string(""));
      int naxes(bool _world=bool(true));
      int ncoordinates();
      string observer();
      record* projection(const string& _type=string(""));
      std::vector<std::string> referencecode(const string& _type=string(""), bool _list=bool(false));
      record* referencepixel(const string& _type=string(""));
      record* referencevalue(const string& _format=string("n"), const string& _type=string(""));
      bool reorder(const std::vector<int>& _order=std::vector<int>({}));
      bool transpose(const std::vector<int>& _order=std::vector<int>({}));
      bool replace(const record& _csys=initialize_record(""""), int _whichin=int(0), int _whichout=int(0));
      record* restfrequency();
      bool setconversiontype(const string& _direction=string(""), const string& _spectral=string(""));
      string getconversiontype(const string& _type=string(""), bool _showconversion=bool(true));
      bool setdirection(const string& _refcode=string(""), const string& _proj=string(""), const std::vector<double>& _projpar=std::vector<double>({-1}), const std::vector<double>& _refpix=std::vector<double>({-1}), const variant& _refval=variant( ), const variant& _incr=variant( ), const variant& _xform=variant( ), const variant& _poles=variant( ));
      bool setepoch(const record& _value=initialize_record(""""));
      bool setincrement(const variant& _value=variant( ), const string& _type=string(""));
      bool setlineartransform(const string& _type=string(""), const variant& _value=variant( ));
      bool setnames(const std::vector<std::string>& _value=std::vector<std::string>({}), const string& _type=string(""));
      bool setobserver(const string& _value=string(""));
      bool setprojection(const string& _type=string(""), const std::vector<double>& _parameters=std::vector<double>({-1}));
      bool setreferencecode(const string& _value=string(""), const string& _type=string("direction"), bool _adjust=bool(true));
      bool setreferencelocation(const std::vector<int>& _pixel=std::vector<int>({-1}), const variant& _world=variant( ), const std::vector<bool>& _mask=std::vector<bool>({false}));
      bool setreferencepixel(const std::vector<double>& _value=std::vector<double>({}), const string& _type=string(""));
      bool setreferencevalue(const variant& _value=variant( ), const string& _type=string(""));
      bool setrestfrequency(const variant& _value=variant( ), int _which=int(0), bool _append=bool(false));
      bool setspectral(const string& _refcode=string(""), const variant& _restfreq=variant( ), const variant& _frequencies=variant( ), const string& _doppler=string(""), const variant& _velocities=variant( ));
      bool setstokes(const std::vector<std::string>& _stokes=std::vector<std::string>({}));
      bool settabular(const std::vector<double>& _pixel=std::vector<double>({-1}), const std::vector<double>& _world=std::vector<double>({-1}), int _which=int(0));
      bool settelescope(const string& _value=string(""));
      bool setunits(const std::vector<std::string>& _value=std::vector<std::string>({}), const string& _type=string(""), bool _overwrite=bool(false), int _which=int(-10));
      std::vector<std::string> stokes();
      std::vector<std::string> summary(const string& _doppler=string("RADIO"), bool _list=bool(true));
      string telescope();
      record* toabs(const variant& _value=variant( ), int _isworld=int(-1));
      record* toabsmany(const variant& _value=variant( ), int _isworld=int(-1));
      record* topixel(const variant& _value=variant( ));
      record* topixelmany(const variant& _value=variant( ));
      record* torecord();
      record* subimage(const variant& _originshft=variant( ), const std::vector<int>& _newshape=std::vector<int>({}));
      record* torel(const variant& _value=variant( ), int _isworld=int(-1));
      record* torelmany(const variant& _value=variant( ), int _isworld=int(-1));
      record* toworld(const variant& _value=variant( ), const string& _format=string("n"));
      record* toworldmany(const variant& _value=variant( ));
      string type();
      std::vector<std::string> units(const string& _type=string(""));
      std::vector<double> velocitytofrequency(const std::vector<double>& _value=std::vector<double>({}), const string& _frequnit=string(""), const string& _doppler=string("radio"), const string& _velunit=string("km/s"));
      string parentname();
      bool setparentname(const string& _imagename=string(""));

        ~coordsys( );

    private:

#include <coordsys_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif
