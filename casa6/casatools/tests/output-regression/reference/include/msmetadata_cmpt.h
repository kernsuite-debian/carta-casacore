#ifndef _MSMETADATA_XML_MSMETADATA_CMPT_
#define _MSMETADATA_XML_MSMETADATA_CMPT_
/******************** generated by xml-casa (v2) from msmetadata.xml ****************
********************* 070d15f712e50541f3c7b75887c70a2f *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <msmetadata_forward.h>


using namespace std;

namespace casac {

  class  msmetadata  {
    public:

      msmetadata();
      std::vector<int> almaspws(bool _chavg=bool(false), bool _fdm=bool(false), bool _sqld=bool(false), bool _tdm=bool(false), bool _wvr=bool(false), bool _complement=bool(false));
      record* antennadiameter(const variant& _antenna=variant( ));
      std::vector<int> antennaids(const variant& _name=variant( ), const variant& _mindiameter=variant( ), const variant& _maxdiameter=variant( ), int _obsid=int(-1));
      std::vector<std::string> antennanames(const variant& _antennaids=variant( ));
      record* antennaoffset(const variant& _which=variant( ));
      record* antennaposition(const variant& _which=variant( ));
      std::vector<std::string> antennastations(const variant& _which=variant( ), int _obsid=int(-1));
      std::vector<int> antennasforscan(int _scan=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      variant* bandwidths(const variant& _spw=variant( ));
      int baseband(int _spw=int(0));
      variant* baselines();
      std::vector<int> chanavgspws();
      std::vector<double> chaneffbws(int _spw=int(0), const string& _unit=string(""), bool _asvel=bool(false));
      std::vector<double> chanfreqs(int _spw=int(0), const string& _unit=string("Hz"));
      std::vector<double> chanres(int _spw=int(0), const string& _unit=string(""), bool _asvel=bool(false));
      std::vector<double> chanwidths(int _spw=int(0), const string& _unit=string("Hz"));
      bool close();
      variant* corrprodsforpol(int _pol=int(-1));
      std::vector<int> corrtypesforpol(int _pol=int(-1));
      std::vector<int> datadescids(int _spw=int(-1), int _pol=int(-1));
      bool done();
      record* effexposuretime();
      record* exposuretime(int _scan=int(0), int _spwid=int(0), int _polid=int(-1), int _obsid=int(0), int _arrayid=int(0));
      std::vector<int> fdmspws();
      std::vector<std::string> fieldnames();
      variant* fieldsforintent(const string& _intent=string(""), bool _asnames=bool(false));
      std::vector<int> fieldsforname(const string& _name=string(""));
      variant* fieldsforscan(int _scan=int(-1), bool _asnames=bool(false), int _obsid=int(-1), int _arrayid=int(-1));
      variant* fieldsforscans(const std::vector<int>& _scans=std::vector<int>({}), bool _asnames=bool(false), int _obsid=int(-1), int _arrayid=int(-1), bool _asmap=bool(false));
      variant* fieldsforsource(int _source=int(-1), bool _asnames=bool(false));
      record* fieldsforsources(bool _asnames=bool(false));
      variant* fieldsforspw(int _spw=int(-1), bool _asnames=bool(false));
      std::vector<int> fieldsfortimes(double _time=double(-1), double _tol=double(0));
      std::vector<std::string> intents();
      std::vector<std::string> intentsforfield(const variant& _field=variant( ));
      std::vector<std::string> intentsforscan(int _scan=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      std::vector<std::string> intentsforspw(int _spw=int(-1));
      double meanfreq(int _spw=int(0), const string& _unit=string("Hz"));
      string name();
      std::vector<std::string> namesforfields(const variant& _fieldids=variant( ));
      std::vector<std::string> namesforspws(const variant& _spwids=variant( ));
      int nantennas();
      int narrays();
      int nbaselines(bool _ac=bool(false));
      int nchan(int _spw=int(0));
      variant* ncorrforpol(int _polid=int(-1));
      int nfields();
      int nobservations();
      int nspw(bool _includewvr=bool(true));
      int nstates();
      int nscans();
      int nsources();
      double nrows(bool _autoc=bool(true), bool _flagged=bool(true));
      std::vector<std::string> observers();
      std::vector<std::string> observatorynames();
      record* observatoryposition(int _which=int(0));
      bool open(const string& _msfile=string(""), float _maxcache=float(50));
      record* phasecenter(int _fieldid=int(0), const record& _epoch=initialize_record(""""));
      record* pointingdirection(int _rownum=int(0), bool _interpolate=bool(false), int _initialrow=int(0));
      variant* polidfordatadesc(int _ddid=int(-1));
      std::vector<std::string> projects();
      record* propermotions();
      record* refdir(const variant& _field=variant( ), const record& _epoch=initialize_record(""""));
      record* reffreq(int _spw=int(-1));
      variant* restfreqs(int _sourceid=int(0), int _spw=int(0));
      std::vector<int> scannumbers(int _obsid=int(-1), int _arrayid=int(-1));
      std::vector<int> scansforfield(const variant& _intent=variant( ), int _obsid=int(-1), int _arrayid=int(-1));
      record* scansforfields(int _obsid=int(0), int _arrayid=int(0));
      std::vector<int> scansforintent(const string& _intent=string(""), int _obsid=int(-1), int _arrayid=int(-1));
      std::vector<int> scansforspw(int _spw=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      record* scansforspws(int _obsid=int(0), int _arrayid=int(0));
      std::vector<int> scansforstate(int _state=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      std::vector<int> scansfortimes(double _time=double(-1), double _tol=double(0), int _obsid=int(-1), int _arrayid=int(-1));
      std::vector<std::string> schedule(int _obsid=int(-1));
      int sideband(int _spw=int(0));
      record* sourcedirs();
      record* sourcetimes();
      int sourceidforfield(int _field=int(-1));
      std::vector<int> sourceidsfromsourcetable();
      std::vector<std::string> sourcenames();
      variant* spwsforbaseband(int _baseband=int(-1), const string& _sqldmode=string("include"));
      variant* spwfordatadesc(int _ddid=int(-1));
      std::vector<int> spwsforfield(const variant& _field=variant( ));
      record* spwsforfields();
      std::vector<int> spwsforintent(const string& _intent=string(""));
      record* spwsfornames(const variant& _spwids=variant( ));
      std::vector<int> spwsforscan(int _scan=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      record* spwsforscans(int _obsid=int(0), int _arrayid=int(0));
      std::vector<int> statesforscan(int _scan=int(-1), int _obsid=int(-1), int _arrayid=int(-1));
      record* statesforscans(int _obsid=int(0), int _arrayid=int(0));
      record* summary();
      std::vector<int> tdmspws();
      record* timerangeforobs(int _obsid=int(-1));
      std::vector<double> timesforfield(int _field=int(-1));
      std::vector<double> timesforintent(const string& _intent=string(""));
      variant* timesforscan(int _scan=int(-1), int _obsid=int(-1), int _arrayid=int(-1), bool _perspw=bool(false));
      std::vector<double> timesforscans(const std::vector<int>& _scans=std::vector<int>({-1}), int _obsid=int(-1), int _arrayid=int(-1));
      variant* timesforspws(const variant& _spw=variant( ));
      variant* transitions(int _sourceid=int(0), int _spw=int(0));
      std::vector<int> wvrspws(bool _complement=bool(false));

        ~msmetadata( );

    private:

#include <msmetadata_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif