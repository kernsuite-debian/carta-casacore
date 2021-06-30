# CASA - Common Astronomy Software Applications
# Copyright (C) 2019 by Associated Universities Inc.
#
# This file is part of CASA.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_library (casa_imageanalysis

Annotations/AnnAnnulus.cc
Annotations/AnnCenterBox.cc
Annotations/AnnCircle.cc
Annotations/AnnEllipse.cc
Annotations/AnnLine.cc
Annotations/AnnotationBase.cc
Annotations/AnnPolygon.cc
Annotations/AnnPolyline.cc
Annotations/AnnRectBox.cc
Annotations/AnnRegion.cc
Annotations/AnnRotBox.cc
Annotations/AnnSymbol.cc
Annotations/AnnText.cc
Annotations/AnnVector.cc
Annotations/RegionTextList.cc

Images/ComponentListImage.cc

ImageAnalysis/CasaImageBeamSet.cc
ImageAnalysis/ComponentImager.cc
ImageAnalysis/ImageCollapserData.cc
ImageAnalysis/ImageCollapser2.cc
ImageAnalysis/ImageFactory2.cc
ImageAnalysis/ImageInputProcessor.cc
ImageAnalysis/ImageMask.cc
ImageAnalysis/ImageMetaDataConstants.cc
ImageAnalysis/ImageMomentsProgress.cc
ImageAnalysis/ImageMomentsProgressMonitor.cc
ImageAnalysis/ImagePadder.cc
ImageAnalysis/ImagePolarimetry.cc
ImageAnalysis/ImagePolProxy.cc
ImageAnalysis/ImagePolTask.cc
ImageAnalysis/ImagePrimaryBeamCorrector.cc
ImageAnalysis/ImageProfileFitter.cc
ImageAnalysis/ImageTotalPolarization.cc
ImageAnalysis/ImageTransposer.cc
ImageAnalysis/LinearPolarizationAngleCalculator.cc
ImageAnalysis/LinearPolarizationCalculator.cc
ImageAnalysis/PixelValueManipulatorData.cc
ImageAnalysis/ProfileFitResults.cc
ImageAnalysis/PVGenerator.cc
ImageAnalysis/SpectralCollapser.cc
ImageAnalysis/SpectralFitter.cc
ImageAnalysis/StatImageCreator.cc

IO/AsciiAnnotationFileLine.cc
IO/FitterEstimatesFileParser.cc
IO/ImageProfileFitterResults.cc
IO/LogFile.cc
IO/OutputDestinationChecker.cc
IO/ParameterParser.cc
IO/ProfileFitterEstimatesFileParser.cc
IO/RegionTextParser.cc

Regions/CasacRegionManager.cc
Utilities/SpectralImageUtil.cc

SpectralComponents/SpectralList.cc
SpectralComponents/SpectralListFactory.cc
SpectralComponents/SpectralElement2.cc
SpectralComponents/SpectralElementFactory.cc
SpectralComponents/SpectralElement.cc
SpectralComponents/GaussianSpectralElement.cc
SpectralComponents/GaussianMultipletSpectralElement.cc
SpectralComponents/SpectralFit.cc
SpectralComponents/PCFSpectralElement.cc
SpectralComponents/CompiledSpectralElement.cc
SpectralComponents/SpectralEstimate.cc
SpectralComponents/PolynomialSpectralElement.cc
SpectralComponents/LorentzianSpectralElement.cc
SpectralComponents/PowerLogPolynomialSpectralElement.cc
SpectralComponents/LogTransformedPolynomialSpectralElement.cc

ComponentModels/SkyComponentFactory.cc
ComponentModels/SkyComponent.cc
ComponentModels/ComponentList.cc
ComponentModels/GaussianShape.cc
ComponentModels/ComponentShape.cc
ComponentModels/TwoSidedShape.cc
ComponentModels/SpectralModel.cc
ComponentModels/ComponentType2.cc
ComponentModels/ComponentType.cc
ComponentModels/DiskShape.cc
ComponentModels/LimbDarkenedDiskShape.cc
ComponentModels/SkyCompRep.cc
ComponentModels/ConstantSpectrum.cc
ComponentModels/SpectralIndex.cc
ComponentModels/PointShape.cc
ComponentModels/TabularSpectrum.cc
ComponentModels/GaussianDeconvolver.cc
ComponentModels/SkyCompBase.cc
ComponentModels/C11Timer.cc

StdCasa/CasacSupport.cc
StdCasa/variant.cc
StdCasa/record.cc

)

target_link_libraries (
casa_imageanalysis
casa_images
casa_coordinates
casa_mirlib
casa_lattices
${CASACORE_ARCH_LIBS}
${GSL_LIBRARIES}
)

add_subdirectory (apps)

install (
TARGETS casa_imageanalysis
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib${LIB_SUFFIX}
ARCHIVE DESTINATION lib${LIB_SUFFIX}
LIBRARY PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)


set(CASA_CODE_PATH "casa6/casa5/code")


install (FILES
    ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/imageanalysis/ImageTypedefs.h
     DESTINATION include/casacode/imageanalysis
    )

install (FILES
	Annotations/AnnAnnulus.h
	Annotations/AnnCenterBox.h
	Annotations/AnnCircle.h
	Annotations/AnnEllipse.h
	Annotations/AnnLine.h
	Annotations/AnnPolygon.h
	Annotations/AnnPolyline.h
	Annotations/AnnRectBox.h
	Annotations/AnnRegion.h
	Annotations/AnnRotBox.h
	Annotations/AnnSymbol.h
	Annotations/AnnText.h
	Annotations/AnnVector.h
	Annotations/AnnotationBase.h
	Annotations/RegionTextList.h
	DESTINATION include/casacode/imageanalysis/Annotations
	)
install (FILES
	IO/AsciiAnnotationFileLine.h
	IO/FitterEstimatesFileParser.h
	IO/FitterEstimatesFileParser2.tcc
	IO/LogFile.h
    IO/ImageFitterResults.h
    IO/ImageFitterResults.tcc
    IO/ImageProfileFitterResults.h
    IO/OutputDestinationChecker.h
	IO/ParameterParser.h
	IO/ProfileFitterEstimatesFileParser.h
	IO/RegionTextParser.h
	DESTINATION include/casacode/imageanalysis/IO
	)
install (FILES
        Utilities/SpectralImageUtil.h
	DESTINATION include/casacode/imageanalysis/Utilities
         )
install (FILES
	ImageAnalysis/BeamManipulator.h
	ImageAnalysis/BeamManipulator.tcc
	ImageAnalysis/CasaImageBeamSet.h
	ImageAnalysis/ComponentImager.h
	ImageAnalysis/ComponentListDeconvolver.h
    ImageAnalysis/ComponentListDeconvolver.tcc
	ImageAnalysis/Image1DSmoother.h
	ImageAnalysis/Image1DSmoother.tcc
	ImageAnalysis/Image2DConvolver.h
	ImageAnalysis/Image2DConvolver.tcc
    ImageAnalysis/ImageBoxcarSmoother.h
    ImageAnalysis/ImageBoxcarSmoother.tcc
	ImageAnalysis/ImageCollapser.h
	ImageAnalysis/ImageCollapser.tcc
	ImageAnalysis/ImageCollapserData.h
	ImageAnalysis/ImageConcatenator.h
    ImageAnalysis/ImageConcatenator.tcc
    ImageAnalysis/ImageConvolver.h
	ImageAnalysis/ImageConvolver.tcc
    ImageAnalysis/ImageConvolverTask.h
    ImageAnalysis/ImageConvolverTask.tcc
	ImageAnalysis/ImageCropper.h
	ImageAnalysis/ImageCropper.tcc
	ImageAnalysis/ImageDecimator.h
    ImageAnalysis/ImageDecimator.tcc
    ImageAnalysis/ImageDecimatorData.h
	ImageAnalysis/ImageDecomposer.h
	ImageAnalysis/ImageDecomposer.tcc
    ImageAnalysis/ImageDecomposerTask.h
    ImageAnalysis/ImageDecomposerTask.tcc
	ImageAnalysis/ImageExprCalculator.h
    ImageAnalysis/ImageExprCalculator.tcc
	ImageAnalysis/ImageFactory.h
	ImageAnalysis/ImageFactory.tcc
	ImageAnalysis/ImageFFT.h
    ImageAnalysis/ImageFFT.tcc
	ImageAnalysis/ImageFFTer.h
	ImageAnalysis/ImageFFTer.tcc
	ImageAnalysis/ImageFitter.h
    ImageAnalysis/ImageFitter.tcc
	ImageAnalysis/ImageFit1D.h
	ImageAnalysis/ImageFit1D.tcc
    ImageAnalysis/ImageHanningSmoother.h
    ImageAnalysis/ImageHanningSmoother.tcc
    ImageAnalysis/ImageHistograms.h
    ImageAnalysis/ImageHistograms.tcc
    ImageAnalysis/ImageHistogramsCalculator.h
    ImageAnalysis/ImageHistogramsCalculator.tcc
    ImageAnalysis/ImageHistory.h
    ImageAnalysis/ImageHistory.tcc
	ImageAnalysis/ImageInputProcessor.h
	ImageAnalysis/ImageInputProcessor2.tcc
	ImageAnalysis/ImageMaskAttacher.h
    ImageAnalysis/ImageMaskAttacher.tcc
    ImageAnalysis/ImageMaskedPixelReplacer.h
    ImageAnalysis/ImageMaskedPixelReplacer.tcc
    ImageAnalysis/ImageMask.h
    ImageAnalysis/ImageMask2.tcc
    ImageAnalysis/ImageMaskHandler.h
    ImageAnalysis/ImageMaskHandler.tcc
    ImageAnalysis/ImageMaxFitter.h
    ImageAnalysis/ImageMaxFitter.tcc
	ImageAnalysis/ImageMetaData.h
    ImageAnalysis/ImageMetaData.tcc
    ImageAnalysis/ImageMetaDataBase.h
    ImageAnalysis/ImageMetaDataBase.tcc
    ImageAnalysis/ImageMetaDataRW.h
    ImageAnalysis/ImageMetaDataRW.tcc
	ImageAnalysis/ImageMoments.h
	ImageAnalysis/ImageMoments.tcc
	ImageAnalysis/ImageMomentsTask.h
    ImageAnalysis/ImageMomentsTask.tcc
    ImageAnalysis/ImageMomentsProgress.h
    ImageAnalysis/ImageMomentsProgressMonitor.h
	ImageAnalysis/ImagePadder.h
	ImageAnalysis/ImagePolarimetry.h
	ImageAnalysis/ImagePolProxy.h
	ImageAnalysis/ImagePolTask.h
	ImageAnalysis/ImagePrimaryBeamCorrector.h
	ImageAnalysis/ImageProfileFitter.h
    ImageAnalysis/ImageRebinner.h
    ImageAnalysis/ImageRebinner.tcc
    ImageAnalysis/ImageRegridder.h
    ImageAnalysis/ImageRegridder.tcc
    ImageAnalysis/ImageRegridderBase.h
    ImageAnalysis/ImageRegridderBase.tcc
    ImageAnalysis/ImageRotator.h
    ImageAnalysis/ImageRotator.tcc
	ImageAnalysis/ImageSourceFinder.h
	ImageAnalysis/ImageSourceFinder.tcc
	ImageAnalysis/ImageStatsCalculator.h
    ImageAnalysis/ImageStatsCalculator.tcc
	ImageAnalysis/ImageStatsBase.h
	ImageAnalysis/ImageStatsBase.tcc
    ImageAnalysis/ImageStatsData.h
	ImageAnalysis/ImageTask.h
    ImageAnalysis/ImageTask.tcc
    ImageAnalysis/ImageTotalPolarization.h
	ImageAnalysis/ImageTransposer.h
	ImageAnalysis/ImageTwoPtCorr.h
	ImageAnalysis/ImageTwoPtCorr.tcc
	ImageAnalysis/LinearPolarizationAngleCalculator.h
	ImageAnalysis/LinearPolarizationCalculator.h
	ImageAnalysis/MomentsBase.h
	ImageAnalysis/MomentsBase.tcc
	ImageAnalysis/MomentCalcBase.h
	ImageAnalysis/MomentCalcBase.tcc
	ImageAnalysis/MomentClip.h
    ImageAnalysis/MomentClip.tcc
    ImageAnalysis/MomentFit.h
	ImageAnalysis/MomentFit.tcc
    ImageAnalysis/MomentWindow.h
    ImageAnalysis/MomentWindow.tcc
	ImageAnalysis/PeakIntensityFluxDensityConverter.h
	ImageAnalysis/PeakIntensityFluxDensityConverter.tcc
	ImageAnalysis/PixelValueManipulator.h
	ImageAnalysis/PixelValueManipulator.tcc
	ImageAnalysis/PixelValueManipulatorData.h
	ImageAnalysis/ProfileFitResults.h
	ImageAnalysis/PVGenerator.h
	ImageAnalysis/SepImageConvolver.h
	ImageAnalysis/SepImageConvolver.tcc
	ImageAnalysis/SepImageConvolverTask.h
    ImageAnalysis/SepImageConvolverTask.tcc
	ImageAnalysis/SpectralCollapser.h
    ImageAnalysis/StatImageCreator.h
	ImageAnalysis/SubImageFactory.h
	ImageAnalysis/SubImageFactory.tcc
	ImageAnalysis/TwoPointCorrelator.h
    ImageAnalysis/TwoPointCorrelator.tcc
	DESTINATION include/casacode/imageanalysis/ImageAnalysis
	)
install (FILES
    Images/ComponentListImage.h
    DESTINATION include/casacode/imageanalysis/Images
    )
install (FILES
	Regions/CasacRegionManager.h
	DESTINATION include/casacode/imageanalysis/Regions
	)

install (FILES 
    ComponentModels/ComponentList.h
    ComponentModels/ComponentShape.h
    ComponentModels/ComponentType.h
    ComponentModels/ConstantSpectrum.h
    ComponentModels/DiskShape.h
    ComponentModels/FluxCalcLogFreqPolynomial.h
    ComponentModels/FluxCalcLogFreqPolynomial.tcc
    ComponentModels/FluxCalcQS.h
    ComponentModels/FluxCalc_SS_JPL_Butler.h
    ComponentModels/FluxCalcVQS.h
    ComponentModels/Flux.h
    ComponentModels/Flux.tcc
    ComponentModels/FluxStandard.h
    ComponentModels/FluxStdsQS2.h
    ComponentModels/FluxStdsQS.h
    ComponentModels/FluxStdSrcs.h
    ComponentModels/GaussianDeconvolver.h
    ComponentModels/GaussianShape.h
    ComponentModels/LimbDarkenedDiskShape.h
    ComponentModels/PointShape.h
    ComponentModels/SkyCompBase.h
    ComponentModels/SkyComponent.h
    ComponentModels/SkyComponentFactory.h
    ComponentModels/SkyComponentFactory2.tcc
    ComponentModels/SkyCompRep.h
    ComponentModels/SpectralIndex.h
    ComponentModels/SpectralModel.h
    ComponentModels/TabularSpectrum.h
    ComponentModels/C11Timer.h
    ComponentModels/TwoSidedShape.h
        DESTINATION include/casacode/components/ComponentModels
)

install (FILES 
    SpectralComponents/CompiledSpectralElement.h
    SpectralComponents/GaussianMultipletSpectralElement.h
    SpectralComponents/GaussianSpectralElement.h
    SpectralComponents/LogTransformedPolynomialSpectralElement.h
    SpectralComponents/LorentzianSpectralElement.h
    SpectralComponents/PCFSpectralElement.h
    SpectralComponents/PolynomialSpectralElement.h
    SpectralComponents/PowerLogPolynomialSpectralElement.h
    SpectralComponents/ProfileFit1D.h
    SpectralComponents/ProfileFit1D.tcc
    SpectralComponents/Spectral2Estimate.tcc
    SpectralComponents/SpectralElementFactory.h
    SpectralComponents/SpectralElement.h
    SpectralComponents/SpectralEstimate.h
    SpectralComponents/SpectralFit2.tcc
    SpectralComponents/SpectralFit.h
    SpectralComponents/SpectralList2.tcc
    SpectralComponents/SpectralListFactory.h
    SpectralComponents/SpectralList.h
        DESTINATION include/casacode/components/SpectralComponents
)


add_custom_target(
    patch_stdcasa_variant
    COMMAND sed -i.orig 's/throw[(]error[)]//g' ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/variant.h ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/StdCasa/variant.cc
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/variant.h.orig ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/StdCasa/variant.cc.orig
)

add_dependencies(casa_imageanalysis patch_stdcasa_variant)


install (FILES
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/xerces.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/Quantity.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/array.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/record.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/variant.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/casac.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/version.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/optionparser.h
         DESTINATION include/casacode/stdcasa
        )

install (FILES
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/StdCasa/CasacSupport.h
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/StdCasa/string_conversions
         DESTINATION include/casacode/stdcasa/StdCasa
        )

install (FILES
         ${CMAKE_SOURCE_DIR}/${CASA_CODE_PATH}/stdcasa/thread/Barrier.h
         DESTINATION include/casacode/stdcasa/thread
        )

