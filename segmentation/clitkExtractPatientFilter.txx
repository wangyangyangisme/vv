/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

#ifndef CLITKEXTRACTPATIENTFILTER_TXX
#define CLITKEXTRACTPATIENTFILTER_TXX

// clitk
#include "clitkImageCommon.h"
#include "clitkSetBackgroundImageFilter.h"
#include "clitkDecomposeAndReconstructImageFilter.h"
#include "clitkAutoCropFilter.h"

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkCastImageFilter.h"

//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
clitk::ExtractPatientFilter<TInputImageType, TOutputImageType>::
ExtractPatientFilter():
  clitk::FilterBase(),
  itk::ImageToImageFilter<TInputImageType, TOutputImageType>()
{
  this->SetNumberOfRequiredInputs(1);
  SetBackgroundValue(0); // Must be zero
  SetForegroundValue(1);

  // Step 1: Threshold + CC + sort (Find low density areas)
  SetUpperThreshold(-300);
  SetLowerThreshold(-1000);
  UseLowerThresholdOff();

  // Step 2: DecomposeAndReconstructImageFilter (optional)
  DecomposeAndReconstructDuringFirstStepOff();
  InternalImageSizeType r;
  r.Fill(1);
  SetRadius1(r);
  SetMaximumNumberOfLabels1(2);
  SetNumberOfNewLabels1(1);

  // Step 3: Remove the air (largest area).

  // Step 4: 2nd DecomposeAndReconstructImageFilter
  DecomposeAndReconstructDuringSecondStepOff();
  SetRadius2(r);
  SetMaximumNumberOfLabels2(2);
  SetNumberOfNewLabels2(1);
  
  // Step 5: Only keep label corresponding (Keep patient's labels)
  SetFirstKeep(1);
  SetLastKeep(1);
  
  // Step 4: OpenClose (option)
  FinalOpenCloseOn();
  AutoCropOff();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractPatientFilter<TInputImageType, TOutputImageType>::
SetInput(const TInputImageType * image) 
{
  this->SetNthInput(0, const_cast<TInputImageType *>(image));
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
template<class ArgsInfoType>
void 
clitk::ExtractPatientFilter<TInputImageType, TOutputImageType>::
SetArgsInfo(ArgsInfoType arg)
{
  SetVerboseOption_GGO(arg);
  SetVerboseStep_GGO(arg);
  SetWriteStep_GGO(arg);
  SetVerboseWarningOff_GGO(arg);

  SetUpperThreshold_GGO(arg);
  SetLowerThreshold_GGO(arg);

  SetDecomposeAndReconstructDuringFirstStep_GGO(arg);
  SetRadius1_GGO(arg);
  SetMaximumNumberOfLabels1_GGO(arg);
  SetNumberOfNewLabels1_GGO(arg);

  SetDecomposeAndReconstructDuringSecondStep_GGO(arg);
  SetRadius2_GGO(arg);
  SetMaximumNumberOfLabels2_GGO(arg);
  SetNumberOfNewLabels2_GGO(arg);
  
  SetFirstKeep_GGO(arg);
  SetLastKeep_GGO(arg);

  SetFinalOpenClose_GGO(arg);
  SetAutoCrop_GGO(arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractPatientFilter<TInputImageType, TOutputImageType>::
GenerateOutputInformation() { 

  Superclass::GenerateOutputInformation();
  input = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));

  // OutputImagePointer outputImage = this->GetOutput(0);
//   outputImage->SetRegions(input->GetLargestPossibleRegion());

  // Get input pointers
  static const unsigned int Dim = InputImageType::ImageDimension;
  //input = dynamic_cast<const TInputImageType*>(itk::ProcessObject::GetInput(0));
    
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Step 1: 
  StartNewStep("Find low densities areas");
  typedef itk::BinaryThresholdImageFilter<InputImageType, InternalImageType> BinarizeFilterType;  
  typename BinarizeFilterType::Pointer binarizeFilter=BinarizeFilterType::New();
  binarizeFilter->SetInput(input);
  if (m_UseLowerThreshold) binarizeFilter->SetLowerThreshold(GetLowerThreshold());
  binarizeFilter->SetUpperThreshold(GetUpperThreshold());
  binarizeFilter ->SetInsideValue(this->GetForegroundValue());
  binarizeFilter ->SetOutsideValue(this->GetBackgroundValue());

  // Connected component labeling
  typedef itk::ConnectedComponentImageFilter<InternalImageType, InternalImageType> ConnectFilterType;
  typename ConnectFilterType::Pointer connectFilter=ConnectFilterType::New();
  connectFilter->SetInput(binarizeFilter->GetOutput());
  connectFilter->SetBackgroundValue(this->GetBackgroundValue());
  connectFilter->SetFullyConnected(false);
  
  // Sort labels according to size
  typedef itk::RelabelComponentImageFilter<InternalImageType, InternalImageType> RelabelFilterType;
  typename RelabelFilterType::Pointer relabelFilter=RelabelFilterType::New();
  relabelFilter->InPlaceOn();
  relabelFilter->SetInput(connectFilter->GetOutput());
  relabelFilter->Update();
  working_image = relabelFilter->GetOutput();
  
  // End
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional] 
  if (GetDecomposeAndReconstructDuringFirstStep()) {
    StartNewStep("First Decompose & Reconstruct step");
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> FilterType;
    typename FilterType::Pointer f = FilterType::New();
    f->SetInput(working_image);
    // f->SetVerbose(m_Verbose);
    f->SetRadius(GetRadius1());
    f->SetMaximumNumberOfLabels(GetMaximumNumberOfLabels1());
    f->SetBackgroundValue(this->GetBackgroundValue());
    f->SetForegroundValue(this->GetForegroundValue());
    f->SetFullyConnected(true);
    f->SetNumberOfNewLabels(GetNumberOfNewLabels1());
    f->Update();
    working_image = f->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Remove the air (largest area)");
  typedef itk::BinaryThresholdImageFilter<InternalImageType, InternalImageType> iBinarizeFilterType;  
  typename iBinarizeFilterType::Pointer binarizeFilter2 = iBinarizeFilterType::New();
  binarizeFilter2->SetInput(working_image);
  binarizeFilter2->SetLowerThreshold(this->GetForegroundValue());
  binarizeFilter2->SetUpperThreshold(this->GetForegroundValue());
  binarizeFilter2 ->SetInsideValue(this->GetBackgroundValue());
  binarizeFilter2 ->SetOutsideValue(this->GetForegroundValue());
  //  binarizeFilter2 ->Update(); // NEEDED ?

  typename ConnectFilterType::Pointer connectFilter2 = ConnectFilterType::New();
  connectFilter2->SetInput(binarizeFilter2->GetOutput());
  connectFilter2->SetBackgroundValue(this->GetBackgroundValue());
  connectFilter2->SetFullyConnected(false);

  typename RelabelFilterType::Pointer relabelFilter2 = RelabelFilterType::New();
  relabelFilter2->SetInput(connectFilter2->GetOutput());
  relabelFilter2->Update();
  working_image = relabelFilter2->GetOutput();
  StopCurrentStep<InternalImageType>(working_image);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional] 
  if (GetDecomposeAndReconstructDuringSecondStep()) {
    StartNewStep("Second Decompose & Reconstruct step");
    typedef clitk::DecomposeAndReconstructImageFilter<InternalImageType,InternalImageType> FilterType;
    typename FilterType::Pointer f = FilterType::New();
    f->SetInput(working_image);
    // f->SetVerbose(m_Verbose);
    f->SetRadius(GetRadius2());
    f->SetMaximumNumberOfLabels(GetMaximumNumberOfLabels2());
    f->SetBackgroundValue(this->GetBackgroundValue());
    f->SetForegroundValue(this->GetForegroundValue());
    f->SetFullyConnected(true);
    f->SetNumberOfNewLabels(GetNumberOfNewLabels2());
    f->Update();
    working_image = f->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  StartNewStep("Keep patient's labels");
  typename iBinarizeFilterType::Pointer binarizeFilter3 = iBinarizeFilterType::New();
  binarizeFilter3->SetInput(working_image);
  binarizeFilter3->SetLowerThreshold(GetFirstKeep());
  binarizeFilter3->SetUpperThreshold(GetLastKeep());
  binarizeFilter3 ->SetInsideValue(this->GetForegroundValue());
  binarizeFilter3 ->SetOutsideValue(this->GetBackgroundValue());
  binarizeFilter3->Update();
  working_image = binarizeFilter3->GetOutput();
  StopCurrentStep<InternalImageType>(working_image);
  
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetFinalOpenClose()) {
    StartNewStep("Final OpenClose");
    // Open
    typedef itk::BinaryBallStructuringElement<InternalPixelType,Dim> KernelType;
    KernelType structuringElement;
    structuringElement.SetRadius(1);
    structuringElement.CreateStructuringElement();
    typedef itk::BinaryMorphologicalOpeningImageFilter<InternalImageType, InternalImageType , KernelType> OpenFilterType;
    typename OpenFilterType::Pointer openFilter = OpenFilterType::New();
    openFilter->SetInput(working_image);
    openFilter->SetBackgroundValue(this->GetBackgroundValue());
    openFilter->SetForegroundValue(this->GetForegroundValue());
    openFilter->SetKernel(structuringElement);  
    // Close
    typedef itk::BinaryMorphologicalClosingImageFilter<InternalImageType, InternalImageType , KernelType> CloseFilterType;
    typename CloseFilterType::Pointer closeFilter = CloseFilterType::New();
    closeFilter->SetInput(openFilter->GetOutput());
    closeFilter->SetSafeBorder(true);
    closeFilter->SetForegroundValue(this->GetForegroundValue());
    //  closeFilter->SetBackgroundValue(SetBackgroundValue());
    closeFilter->SetKernel(structuringElement);
    closeFilter->Update();  
    working_image = closeFilter->GetOutput();
    StopCurrentStep<InternalImageType>(working_image);
  }

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // Final Cast 
  typedef itk::CastImageFilter<InternalImageType, OutputImageType> CastImageFilterType;
  typename CastImageFilterType::Pointer caster= CastImageFilterType::New();
  caster->SetInput(working_image);
  caster->Update();
  output = caster->GetOutput();

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------
  // [Optional]
  if (GetAutoCrop()) {
  StartNewStep("AutoCrop");
    typedef clitk::AutoCropFilter<OutputImageType> CropFilterType;
    typename CropFilterType::Pointer cropFilter = CropFilterType::New();
    cropFilter->SetInput(output);
    cropFilter->SetBackgroundValue(GetBackgroundValue());
    cropFilter->Update();   
    output = cropFilter->GetOutput();
    StopCurrentStep<OutputImageType>(output);
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TInputImageType, class TOutputImageType>
void 
clitk::ExtractPatientFilter<TInputImageType, TOutputImageType>::
GenerateData() {
  //this->SetNthOutput(0, output); // -> no because redo filter otherwise
  this->GraftOutput(output);
}
//--------------------------------------------------------------------
  

#endif //#define CLITKBOOLEANOPERATORLABELIMAGEFILTER_TXX