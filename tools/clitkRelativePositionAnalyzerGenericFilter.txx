/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

//--------------------------------------------------------------------
template<class ArgsInfoType>
clitk::RelativePositionAnalyzerGenericFilter<ArgsInfoType>::
RelativePositionAnalyzerGenericFilter():
  ImageToImageGenericFilter<Self>("RelativePositionAnalyzer")
{
  // Default values
  cmdline_parser_clitkRelativePositionAnalyzer_init(&mArgsInfo);
  InitializeImageType<3>();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ArgsInfoType>
template<unsigned int Dim>
void clitk::RelativePositionAnalyzerGenericFilter<ArgsInfoType>::
InitializeImageType() 
{  
  ADD_IMAGE_TYPE(Dim, uchar);
}
//--------------------------------------------------------------------
  

//--------------------------------------------------------------------
template<class ArgsInfoType>
void clitk::RelativePositionAnalyzerGenericFilter<ArgsInfoType>::
SetArgsInfo(const ArgsInfoType & a) 
{
  mArgsInfo=a;
  SetIOVerbose(mArgsInfo.verbose_flag);
  if (mArgsInfo.imagetypes_flag) this->PrintAvailableImageTypes();
  if (mArgsInfo.support_given) AddInputFilename(mArgsInfo.support_arg);
  if (mArgsInfo.object_given) AddInputFilename(mArgsInfo.object_arg);
  if (mArgsInfo.target_given) AddInputFilename(mArgsInfo.target_arg);
  if (mArgsInfo.output_given) AddOutputFilename(mArgsInfo.output_arg);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class FilterType>
void clitk::RelativePositionAnalyzerGenericFilter<ArgsInfoType>::
SetOptionsFromArgsInfoToFilter(FilterType * f) 
{
  
}

//--------------------------------------------------------------------
// Update with the number of dimensions and the pixeltype
//--------------------------------------------------------------------
template<class ArgsInfoType>
template<class ImageType>
void clitk::RelativePositionAnalyzerGenericFilter<ArgsInfoType>::
UpdateWithInputImageType() 
{ 
  // Reading input
  typename ImageType::Pointer support = this->template GetInput<ImageType>(0);
  typename ImageType::Pointer object = this->template GetInput<ImageType>(1);
  typename ImageType::Pointer target = this->template GetInput<ImageType>(2);

  // Create filter
  typedef clitk::RelativePositionAnalyzerFilter<ImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  
  // Set global Options 
  filter->SetInputSupport(support);
  filter->SetInputObject(object);
  filter->SetInputTarget(target);
  SetOptionsFromArgsInfoToFilter<FilterType>(filter);

  // Go !
  filter->Update();
  
  // Write/Save results
  //  typename ImageType::Pointer output = filter->GetOutput();
  //this->template SetNextOutput<ImageType>(output); 

}
//--------------------------------------------------------------------


