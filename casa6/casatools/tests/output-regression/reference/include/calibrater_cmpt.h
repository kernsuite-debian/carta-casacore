#ifndef _CALIBRATER_XML_CALIBRATER_CMPT_
#define _CALIBRATER_XML_CALIBRATER_CMPT_
/******************** generated by xml-casa (v2) from calibrater.xml ****************
********************* 48046d4c970151287b18f85e46a7ec61 *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <calibrater_forward.h>


using namespace std;

namespace casac {

  class  calibrater  {
    public:

      calibrater();
      bool open(const string& _filename=string(""), bool _compress=bool(false), bool _addcorr=bool(true), bool _addmodel=bool(true));
      bool selectvis(const variant& _time=variant( ), const variant& _spw=variant( ), const variant& _scan=variant( ), const variant& _field=variant( ), const variant& _intent=variant( ), const variant& _observation=variant( ), const variant& _baseline=variant( ), const variant& _uvrange=variant( ), const string& _chanmode=string("none"), int _nchan=int(1), int _start=int(0), int _step=int(1), const Quantity& _mstart=Quantity(std::vector<double>(1,0.0),"km/s"), const Quantity& _mstep=Quantity(std::vector<double>(1,0.0),"km/s"), const string& _msselect=string(""));
      bool setmodel(const string& _modelimage=string(""));
      bool setptmodel(const std::vector<double>& _stokes=std::vector<double>({0.0,0.0,0.0,0.0}));
      bool setapply(const string& _type=string(""), double _t=double(0.0), const string& _table=string(""), const variant& _field=variant( ), const string& _interp=string("linear"), const string& _select=string(""), bool _calwt=bool(false), const std::vector<int>& _spwmap=std::vector<int>({-1}), const std::vector<double>& _opacity=std::vector<double>({0.0}));
      bool setcallib(const record& _callib=initialize_record(""""));
      bool validatecallib(const record& _callib=initialize_record(""""));
      bool setsolve(const string& _type=string("MF"), const variant& _t=variant( ), const string& _table=string(""), bool _append=bool(false), double _preavg=double(-1.0), bool _phaseonly=bool(false), const string& _apmode=string("AP"), const variant& _refant=variant( ), const string& _refantmode=string("flex"), int _minblperant=int(4), bool _solnorm=bool(false), float _minsnr=float(0.0), const string& _combine=string(""), int _fillgaps=int(0), const string& _cfcache=string(""), float _painc=float(360.0), int _fitorder=int(0), float _fraction=float(0.1), int _numedge=int(-1), const string& _radius=string(""), bool _smooth=bool(true), bool _zerorates=bool(false));
      bool setsolvegainspline(const string& _table=string(""), bool _append=bool(false), const string& _mode=string("PHAS"), double _splinetime=double(10800), double _preavg=double(0.0), int _npointaver=int(10), double _phasewrap=double(250), const variant& _refant=variant( ));
      bool setsolvebandpoly(const string& _table=string(""), bool _append=bool(false), const variant& _t=variant( ), const string& _combine=string(""), int _degamp=int(3), int _degphase=int(3), bool _visnorm=bool(false), bool _solnorm=bool(true), int _maskcenter=int(0), double _maskedge=double(5.0), const variant& _refant=variant( ));
      bool state();
      bool reset(bool _apply=bool(true), bool _solve=bool(true));
      bool initcalset(int _calset=int(0));
      bool delmod(bool _otf=bool(false), const variant& _field=variant( ), const variant& _spw=variant( ), bool _scr=bool(false));
      bool solve();
      bool correct(const string& _applymode=string(""));
      bool corrupt();
      bool initweights(const string& _wtmode=string("nyq"), bool _dowtsp=bool(false), const string& _tsystable=string(""), const string& _gainfield=string(""), const string& _interp=string(""), const std::vector<int>& _spwmap=std::vector<int>({}));
      record* fluxscale(const string& _tablein=string(""), const variant& _reference=variant( ), const string& _tableout=string(""), const variant& _transfer=variant( ), const string& _listfile=string(""), bool _append=bool(false), const std::vector<int>& _refspwmap=std::vector<int>({-1}), float _gainthreshold=float(-1.0), const string& _antenna=string(""), const string& _timerange=string(""), const string& _scan=string(""), bool _incremental=bool(false), int _fitorder=int(1), bool _display=bool(false));
      bool accumulate(const string& _tablein=string(""), const string& _incrtable=string(""), const string& _tableout=string(""), const variant& _field=variant( ), const variant& _calfield=variant( ), const string& _interp=string("linear"), double _t=double(-1.0), const std::vector<int>& _spwmap=std::vector<int>({-1}));
      record* activityrec();
      bool specifycal(const string& _caltable=string(""), const string& _time=string(""), const string& _spw=string(""), const string& _antenna=string(""), const string& _pol=string(""), const string& _caltype=string(""), const std::vector<double>& _parameter=std::vector<double>({1.0}), const string& _infile=string(""), bool _uniform=bool(true));
      bool smooth(const string& _tablein=string(""), const string& _tableout=string(""), const variant& _field=variant( ), const string& _smoothtype=string("median"), double _smoothtime=double(60.0));
      bool rerefant(const string& _tablein=string(""), const string& _tableout=string(""), const string& _refantmode=string("flexible"), const variant& _refant=variant( ));
      bool listcal(const string& _caltable=string(""), const variant& _field=variant( ), const variant& _antenna=variant( ), const variant& _spw=variant( ), const string& _listfile=string(""), int _pagerows=int(50));
      bool posangcal(const std::vector<double>& _posangcor=std::vector<double>({}), const string& _tablein=string(""), const string& _tableout=string(""));
      bool linpolcor(const string& _tablein=string(""), const string& _tableout=string(""), const std::vector<std::string>& _fields=std::vector<std::string>({}));
      bool plotcal(const std::vector<int>& _antennas=std::vector<int>({}), const std::vector<int>& _fields=std::vector<int>({}), const std::vector<int>& _spwids=std::vector<int>({}), const string& _plottype=string("AMP"), const string& _tablename=string(""), int _polarization=int(1), bool _multiplot=bool(false), int _nx=int(1), int _ny=int(1), const string& _psfile=string(""));
      std::vector<double> modelfit(const std::vector<bool>& _vary=std::vector<bool>({}), int _niter=int(0), const string& _compshape=string("P"), const std::vector<double>& _par=std::vector<double>({1.0,0.0,0.0}), const string& _file=string(""));
      bool createcaltable(const string& _caltable=string(""), const string& _partype=string(""), const string& _caltype=string(""), bool _singlechan=bool(false));
      bool updatecaltable(const string& _caltable=string(""));
      bool close();
      bool done();
      record* parsecallibfile(const string& _filein=string(""));
      bool setvi(bool _old=bool(false), bool _quiet=bool(false));

        ~calibrater( );

    private:

#include <calibrater_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif