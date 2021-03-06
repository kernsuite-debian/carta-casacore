#ifndef _MEASURES_XML_MEASURES_CMPT_
#define _MEASURES_XML_MEASURES_CMPT_
/******************** generated by xml-casa (v2) from measures.xml ******************
********************* 4cdfb6f6cee807f9bbdc3b71f867d2be *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <measures_forward.h>


using namespace std;

namespace casac {

  class  measures  {
    public:

      measures();
      string dirshow(const record& _v=initialize_record(""""));
      string show(const record& _v=initialize_record(""""), bool _refcode=bool(true));
      record* epoch(const string& _rf=string("UTC"), const variant& _v0=variant( ), const record& _off=initialize_record(""""));
      record* direction(const string& _rf=string("J2000"), const variant& _v0=variant( ), const variant& _v1=variant( ), const record& _off=initialize_record(""""));
      record* getvalue(const record& _v=initialize_record(""""));
      string gettype(const record& _v=initialize_record(""""));
      string getref(const record& _v=initialize_record(""""));
      record* getoffset(const record& _v=initialize_record(""""));
      string cometname();
      string comettype();
      record* cometdist();
      record* cometangdiam();
      record* comettopo();
      bool framecomet(const string& _v=string(""));
      record* position(const string& _rf=string("WGS84"), const variant& _v0=variant( ), const variant& _v1=variant( ), const variant& _v2=variant( ), const record& _off=initialize_record(""""));
      record* observatory(const string& _name=string("ALMA"));
      std::vector<std::string> obslist();
      string linelist();
      record* spectralline(const string& _name=string("HI"));
      string sourcelist();
      record* source(const variant& _name=variant( ));
      record* frequency(const string& _rf=string("LSRK"), const variant& _v0=variant( ), const record& _off=initialize_record(""""));
      record* doppler(const string& _rf=string("RADIO"), const variant& _v0=variant( ), const record& _off=initialize_record(""""));
      record* radialvelocity(const string& _rf=string("LSRK"), const variant& _v0=variant( ), const record& _off=initialize_record(""""));
      record* shift(const record& _v=initialize_record(""""), const variant& _offset=variant( ), const variant& _pa=variant( ));
      record* uvw(const string& _rf=string("ITRF"), const variant& _v0=variant( ), const variant& _v1=variant( ), const variant& _v2=variant( ), const record& _off=initialize_record(""""));
      record* touvw(const record& _v=initialize_record(""""), record& _dot=_dot_touvw, record& _xyz=_xyz_touvw);
      record* expand(const record& _v=initialize_record(""""), record& _xyz=_xyz_expand);
      record* earthmagnetic(const string& _rf=string("IGRF"), const variant& _v0=variant( ), const variant& _v1=variant( ), const variant& _v2=variant( ), const record& _off=initialize_record(""""));
      record* baseline(const string& _rf=string("ITRF"), const variant& _v0=variant( ), const variant& _v1=variant( ), const variant& _v2=variant( ), const record& _off=initialize_record(""""));
      record* asbaseline(const record& _pos=initialize_record(""""));
      record* listcodes(const record& _ms=initialize_record(""""));
      record* measure(const record& _v=initialize_record(""""), const string& _rf=string(""), const record& _off=initialize_record(""""));
      bool doframe(const record& _v=initialize_record(""""));
      bool framenow();
      string showframe();
      record* toradialvelocity(const string& _rf=string(""), const record& _v0=initialize_record(""""));
      record* tofrequency(const string& _rf=string(""), const record& _v0=initialize_record(""""), const record& _rfq=initialize_record(""""));
      record* todoppler(const string& _rf=string(""), const record& _v0=initialize_record(""""), const variant& _rfq=variant( ));
      record* torestfrequency(const record& _v0=initialize_record(""""), const record& _d0=initialize_record(""""));
      record* rise(const variant& _crd=variant( ), const variant& _ev=variant( ));
      record* riseset(const variant& _crd=variant( ), const variant& _ev=variant( ));
      record* posangle(const record& _m1=initialize_record(""""), const record& _m2=initialize_record(""""));
      record* separation(const record& _m1=initialize_record(""""), const record& _m2=initialize_record(""""));
      record* addxvalue(const record& _a=initialize_record(""""));
      string type();
      bool done();
      bool ismeasure(const record& _v=initialize_record(""""));

        ~measures( );

    private:

#include <measures_private.h>


      // --- declarations of static parameter defaults ---
    public:
      static record _dot_touvw;
      static record _xyz_touvw;
      static record _xyz_expand;
  };

}

#endif
