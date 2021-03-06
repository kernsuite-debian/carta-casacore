#ifndef _IMAGEPOL_XML_IMAGEPOL_CMPT_
#define _IMAGEPOL_XML_IMAGEPOL_CMPT_
/******************** generated by xml-casa (v2) from imagepol.xml ******************
********************* 3b7437629b45930681a8977ca52048f2 *****************************/

#include <vector>
#include <string>
#include <complex>
#include <stdcasa/record.h>
#include <casaswig_types.h>
#include <casa/Exceptions/Error.h>
#include <imagepol_forward.h>

#include <image_cmpt.h>


using namespace std;

namespace casac {

  class  imagepol  {
    public:

      imagepol();
      bool imagepoltestimage(const string& _outfile=string("imagepol.iquv"), const std::vector<double>& _rm=std::vector<double>({0.0}), double _pa0=double(0.0), double _sigma=double(0.01), int _nx=int(32), int _ny=int(32), int _nf=int(32), double _f0=double(1.4e9), double _bw=double(128.0e6));
      bool complexlinpol(const string& _outfile=string(""));
      bool complexfraclinpol(const string& _outfile=string(""));
      casac::image* depolratio(const string& _infile=string(""""), bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      bool close();
      bool done();
      bool fourierrotationmeasure(const string& _complex=string(""), const string& _amp=string(""), const string& _pa=string(""), const string& _real=string(""), const string& _imag=string(""), bool _zerolag0=bool(false));
      casac::image* fraclinpol(bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      casac::image* fractotpol(bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      casac::image* linpolint(bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""), const variant& _region=variant( ), const string& _mask=string(""), bool _stretch=bool(false));
      casac::image* linpolposang(const string& _outfile=string(""), const variant& _region=variant( ), const string& _mask=string(""), bool _stretch=bool(false));
      bool makecomplex(const string& _complex=string(""), const string& _real=string(""), const string& _imag=string(""), const string& _amp=string(""), const string& _phase=string(""));
      bool open(const variant& _image=variant( ));
      casac::image* pol(const string& _which=string(""), bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      bool rotationmeasure(const string& _rm=string(""), const string& _rmerr=string(""), const string& _pa0=string(""), const string& _pa0err=string(""), const string& _nturns=string(""), const string& _chisq=string(""), double _sigma=double(-1), double _rmfg=double(0.0), double _rmmax=double(0.0), double _maxpaerr=double(1e30), const string& _plotter=string(""), int _nx=int(5), int _ny=int(5));
      double sigma(double _clip=double(10.0));
      casac::image* sigmadepolratio(const string& _infile=string(""""), bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      casac::image* sigmafraclinpol(double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      casac::image* sigmafractotpol(double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      double sigmalinpolint(double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      casac::image* sigmalinpolposang(double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""));
      double sigmastokes(const string& _which=string(""), double _clip=double(10.0));
      double sigmastokesi(double _clip=double(10.0));
      double sigmastokesq(double _clip=double(10.0));
      double sigmastokesu(double _clip=double(10.0));
      double sigmastokesv(double _clip=double(10.0));
      double sigmatotpolint(double _clip=double(10.0), double _sigma=double(-1));
      casac::image* stokes(const string& _which=string(""), const string& _outfile=string(""));
      casac::image* stokesi(const string& _outfile=string(""));
      casac::image* stokesq(const string& _outfile=string(""));
      casac::image* stokesu(const string& _outfile=string(""));
      casac::image* stokesv(const string& _outfile=string(""));
      bool summary();
      casac::image* totpolint(bool _debias=bool(false), double _clip=double(10.0), double _sigma=double(-1), const string& _outfile=string(""), const variant& _region=variant( ), const string& _mask=string(""), bool _stretch=bool(false));

        ~imagepol( );

    private:

#include <imagepol_private.h>


      // --- declarations of static parameter defaults ---
    public:

  };

}

#endif
