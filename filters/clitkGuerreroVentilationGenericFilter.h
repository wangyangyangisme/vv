/*-------------------------------------------------------------------------

  Copyright (c) CREATIS (Centre de Recherche et d'Applications en Traitement de
  l'Image). All rights reserved. See Doc/License.txt or
  http://www.creatis.insa-lyon.fr/Public/Gdcm/License.html for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
                                                                             
-------------------------------------------------------------------------*/

#ifndef clitkGuerreroVentilationGenericFilter_H
#define clitkGuerreroVentilationGenericFilter_H

/**
 -------------------------------------------------------------------
 * @file   clitkGuerreroVentilationGenericFilter.h
 * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
 * @date   23 Feb 2008
 -------------------------------------------------------------------*/

// clitk include
#include "clitkImageToImageGenericFilter.h"

// itk include
#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace clitk {
  
  //--------------------------------------------------------------------
  class GuerreroVentilationGenericFilter : 
  public ImageToImageGenericFilter<GuerreroVentilationGenericFilter> {
  public:
	
    // Constructor 
    GuerreroVentilationGenericFilter();
    virtual ~GuerreroVentilationGenericFilter() {}

    // Types
    typedef GuerreroVentilationGenericFilter  Self;
    typedef ImageToImageGenericFilterBase     Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    // New
    itkNewMacro(Self);
    
    // Set methods
    void SetBloodCorrectionFactor(double f) {blood_mass_factor=f;}
    void SetUseCorrectFormula(bool u) {use_correct_formula=u;}

    //--------------------------------------------------------------------
    // Main function called each time the filter is updated
    template<class InputImageType>  
    void UpdateWithInputImageType();

  protected:  
    template<unsigned int Dim> void InitializeImageType();
    //Parameters
    double blood_mass_factor;
    bool use_correct_formula;

  }; // end class GuerreroVentilationGenericFilter
//--------------------------------------------------------------------

} // end namespace
//--------------------------------------------------------------------

#endif //#define clitkGuerreroVentilationGenericFilter_H

