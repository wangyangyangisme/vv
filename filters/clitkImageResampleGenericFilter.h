#ifndef CLITKIMAGERESAMPLEGENERICFILTER_H
#define CLITKIMAGERESAMPLEGENERICFILTER_H

/**
 -------------------------------------------------------------------
 * @file   clitkImageResampleGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008 08:37:53

 * @brief  
 -------------------------------------------------------------------*/

// clitk include
#include "clitkImageToImageGenericFilter.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageResampleGenericFilter: 
    public ImageToImageGenericFilter<ImageResampleGenericFilter> {
    
  public: 
    // constructor
    ImageResampleGenericFilter();

    // Types
    typedef ImageResampleGenericFilter    Self;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    void SetOutputSize(const std::vector<int> & size);
    void SetOutputSpacing(const std::vector<double> & spacing);
    void SetGaussianSigma(const std::vector<double> & sigma);
    void SetInterpolationName(const std::string & inter);
    void SetDefaultPixelValue(double dpv) { mDefaultPixelValue = dpv;}
    void SetBSplineOrder(int o) { mBSplineOrder = o; }
    void SetBLUTSampling(int b) { mSamplingFactors.resize(1); mSamplingFactors[0] = b; }

    //--------------------------------------------------------------------
    template<class InputImageType> void UpdateWithInputImageType();

  protected:
    //--------------------------------------------------------------------
    std::string mInterpolatorName;
    std::vector<int> mOutputSize;
    std::vector<double> mOutputSpacing;
    std::vector<double> mOutputOrigin;
    double mDefaultPixelValue;
    bool mApplyGaussianFilterBefore;
    std::vector<double> mSigma;
    int mBSplineOrder;
    std::vector<int> mSamplingFactors;

    //--------------------------------------------------------------------
    template<unsigned int Dim> void InitializeImageTypeWithDim();
     
  }; // end class ImageResampleGenericFilter
  //--------------------------------------------------------------------
    
  //#include "clitkImageResampleGenericFilter.txx"

} // end namespace
//--------------------------------------------------------------------
    
#endif /* end #define CLITKIMAGERESAMPLEGENERICFILTER_H */

