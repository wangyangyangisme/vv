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
#ifndef CLITKIMAGETOIMAGEGENERICFILTERBASE_H
#define CLITKIMAGETOIMAGEGENERICFILTERBASE_H
#include "clitkCommon.h"
#include "clitkImageCommon.h"
#include "clitkCommonGenericFilter.h"

// itk include
#include <itkImage.h>

// vv include
#include "vvImage.h"
#include "vvFromITK.h"
#include "vvToITK.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class ImageToImageGenericFilterBase: public itk::Object {
    
  public: 
    // constructor - destructor
    ImageToImageGenericFilterBase(std::string filterName);
    virtual ~ImageToImageGenericFilterBase();

    // Types
    typedef ImageToImageGenericFilterBase Self;
    typedef Object                        Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // Filters information
    const std::string & GetFilterName();
    void SetFilterName(std::string & n);

    // Generic IO
    /// Returns the dimension and pixel type of the *first* input
    void GetInputImageDimensionAndPixelType(unsigned int& dim, std::string& pixeltype,
                                            unsigned int & components);

    // File IO
    void SetInputFilename(const std::string & filename);
    void AddInputFilename(const std::string & filename);
    void SetInputFilenames(const std::vector<std::string> & filenames);
    void EnableReadOnDisk(bool b);
    void SetOutputFilename(const std::string & filename);
    void AddOutputFilename(const std::string & filename);
    void SetOutputFilenames(const std::vector<std::string> & filenames);
    std::string GetOutputFilename();
    void SetIOVerbose(bool b);

    // VVImage IO
    void SetInputVVImage (vvImage::Pointer input);
    void SetInputVVImages (std::vector<vvImage::Pointer> input);
    void AddInputVVImage (vvImage::Pointer input);
    vvImage::Pointer GetOutputVVImage ();
    std::vector<vvImage::Pointer> GetOutputVVImages ();

    // Information on available image types
    void PrintAvailableImageTypes();
    virtual std::string GetAvailableImageTypes() = 0;

    virtual bool CheckImageType(unsigned int dim,unsigned int ncomp, std::string pixeltype) = 0;
    virtual bool CheckImageType() = 0;

    // Main function to call for using the filter. 
    virtual bool Update() = 0;

  protected:  
    bool mReadOnDisk;
    /// Call this function to dispatch an output towards the correct sink
    template<class ImageType> 
    void SetNextOutput(typename ImageType::Pointer output);

    /// Call this function to get the nth itk input image, regardless of input source
    template<class ImageType> 
    typename ImageType::Pointer GetInput(unsigned int n);

    std::vector<std::string> mInputFilenames;
    std::list<std::string> mOutputFilenames;

    bool mIOVerbose;
    unsigned int mDim;
    std::string mPixelTypeName;
    unsigned int mNbOfComponents;
    std::string mFilterName;

    std::vector<vvImage::Pointer> mInputVVImages;
    std::vector<vvImage::Pointer> mOutputVVImages;

    void ImageTypeError();
    void SetImageTypeError();
    bool mFailOnImageTypeError;

  }; // end class clitk::ImageToImageGenericFilter

#define ADD_VEC_IMAGE_TYPE(DIM, COMP, PT) this->mImageTypesManager.template AddNewDimensionAndPixelType<DIM,COMP, PT>();
#define ADD_IMAGE_TYPE(DIM, PT) this->mImageTypesManager.template AddNewDimensionAndPixelType<DIM, PT>();

  //#include "clitkImageToImageGenericFilterBase.txx"

} // end namespace

#endif /* end #define CLITKIMAGETOIMAGEGENERICFILTERBASE_H */
