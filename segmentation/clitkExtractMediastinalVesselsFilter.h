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

#ifndef CLITKEXTRACTLYMPHSTATIONSFILTER_H
#define CLITKEXTRACTLYMPHSTATIONSFILTER_H

// clitk
#include "clitkFilterBase.h"
#include "clitkFilterWithAnatomicalFeatureDatabaseManagement.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  /*
    Try to extract the some Mediastinal Vessels in a thorax CT.
    Need a set of Anatomical Features (AFDB)
  */
  //--------------------------------------------------------------------
  
  template <class TImageType>
  class ITK_EXPORT ExtractMediastinalVesselsFilter: 
    public virtual clitk::FilterBase, 
    public clitk::FilterWithAnatomicalFeatureDatabaseManagement,
    public itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> >
  {

  public:
    /** Standard class typedefs. */
    typedef itk::ImageToImageFilter<TImageType, itk::Image<uchar, 3> > Superclass;
    typedef ExtractMediastinalVesselsFilter          Self;
    typedef itk::SmartPointer<Self>             Pointer;
    typedef itk::SmartPointer<const Self>       ConstPointer;
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(ExtractMediastinalVesselsFilter, ImageToImageFilter);

    /** Some convenient typedefs. */
    typedef TImageType                       ImageType;
    typedef typename ImageType::ConstPointer ImageConstPointer;
    typedef typename ImageType::Pointer      ImagePointer;
    typedef typename ImageType::RegionType   ImageRegionType; 
    typedef typename ImageType::PixelType    ImagePixelType; 
    typedef typename ImageType::SizeType     ImageSizeType; 
    typedef typename ImageType::IndexType    ImageIndexType; 
    typedef typename ImageType::PointType    ImagePointType; 
        
    typedef uchar MaskImagePixelType;
    typedef itk::Image<MaskImagePixelType, 3>    MaskImageType;  
    typedef typename MaskImageType::Pointer      MaskImagePointer;
    typedef typename MaskImageType::RegionType   MaskImageRegionType; 
    typedef typename MaskImageType::SizeType     MaskImageSizeType; 
    typedef typename MaskImageType::IndexType    MaskImageIndexType; 
    typedef typename MaskImageType::PointType    MaskImagePointType; 

    typedef itk::Image<MaskImagePixelType, 2>    MaskSliceType;
    typedef typename MaskSliceType::Pointer      MaskSlicePointer;
    typedef typename MaskSliceType::PointType    MaskSlicePointType;

    typedef long LabelType;
    /** ImageDimension constants */
    itkStaticConstMacro(ImageDimension, unsigned int, ImageType::ImageDimension);
    FILTERBASE_INIT;
   
    itkGetConstMacro(BackgroundValue, MaskImagePixelType);
    itkSetMacro(BackgroundValue, MaskImagePixelType);

    itkGetConstMacro(ForegroundValue, MaskImagePixelType);
    itkSetMacro(ForegroundValue, MaskImagePixelType);

    itkGetConstMacro(TemporaryForegroundValue, MaskImagePixelType);
    itkSetMacro(TemporaryForegroundValue, MaskImagePixelType);

    itkGetConstMacro(OutputFolder, std::string);
    itkSetMacro(OutputFolder, std::string);

    itkGetConstMacro(Threshold, ImagePixelType);
    itkSetMacro(Threshold, ImagePixelType);

  protected:
    ExtractMediastinalVesselsFilter();
    virtual ~ExtractMediastinalVesselsFilter() {}
    
    virtual void GenerateOutputInformation();
    virtual void GenerateInputRequestedRegion();
    virtual void GenerateData();
    
    std::string        m_OutputFolder;
    ImagePointer       m_Input;
    MaskImagePointer   m_Working_Support;
    MaskImagePointer   m_Mediastinum;
    MaskImagePointer   m_Mask;
    MaskImagePixelType m_BackgroundValue;
    MaskImagePixelType m_ForegroundValue;
    MaskImagePixelType m_TemporaryForegroundValue;
    ImagePixelType     m_Threshold;

    std::vector<MaskSlicePointer> m_slice_recon;
    
    void CropSupInf();
    //void SearchBrachioCephalicArtery(int & BCA_first_slice, LabelType & BCA_first_label);
    void TrackBifurcationFromPoint(MaskImagePointer & recon, 
                                   std::vector<MaskSlicePointer> & slices_recon, 
                                   MaskImagePointType BCA_p, 
                                   LabelType newLabel, 
                                   std::vector<MaskImagePointType> & bif);

  private:
    ExtractMediastinalVesselsFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
  }; // end class
  //--------------------------------------------------------------------

} // end namespace clitk
//--------------------------------------------------------------------

#ifndef ITK_MANUAL_INSTANTIATION
#include "clitkExtractMediastinalVesselsFilter.txx"
#endif

#endif