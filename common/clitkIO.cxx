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

// std include
#include <iostream>
#include <fstream>

// itk include
#include "itkMacro.h"

// clitk include
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkDicomRTDoseIOFactory.h"
#include "clitkVoxImageIOFactory.h"
#include "clitkHisImageIOFactory.h"
#include "clitkVfImageIOFactory.h"
#include "clitkXdrImageIOFactory.h"
#include "rtkHndImageIOFactory.h"
#include "clitkGateAsciiImageIOFactory.h"
#include "clitkConfiguration.h"
#if CLITK_PRIVATE_FEATURES
  #include "clitkUsfImageIOFactory.h"
  #include "clitkUSVoxImageIOFactory.h"
  #include "clitkSvlImageIOFactory.h"
#endif

//--------------------------------------------------------------------
// Register factories
void clitk::RegisterClitkFactories()
{
#if CLITK_PRIVATE_FEATURES
  clitk::UsfImageIOFactory::RegisterOneFactory();
  clitk::USVoxImageIOFactory::RegisterOneFactory();
  clitk::SvlImageIOFactory::RegisterOneFactory();
#endif
  clitk::GateAsciiImageIOFactory::RegisterOneFactory();
  clitk::DicomRTDoseIOFactory::RegisterOneFactory();
#if ITK_VERSION_MAJOR <= 3
  itk::ImageIOFactory::RegisterBuiltInFactories();
#endif
  clitk::VoxImageIOFactory::RegisterOneFactory();
  clitk::VfImageIOFactory::RegisterOneFactory();
  clitk::HisImageIOFactory::RegisterOneFactory();
  clitk::XdrImageIOFactory::RegisterOneFactory();
  rtk::HndImageIOFactory::RegisterOneFactory();
} ////

