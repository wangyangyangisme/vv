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

#include "vvImageContour.h"
#include "vvImage.h"
#include <vtkImageActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMarchingSquares.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//------------------------------------------------------------------------------
vvImageContour::vvImageContour() {
  mTSlice = -1;
  mSlice = 0;
  mHiddenImageIsUsed = false;
  mDisplayModeIsPreserveMemory = true;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageContour::~vvImageContour() {
  for (unsigned int i = 0; i < mSlicer->GetImage()->GetVTKImages().size(); i++) {
    mSlicer->GetRenderer()->RemoveActor(mSquaresActorList[i]);
  }
  mSquaresActorList.clear();
  mSquaresList.clear();
  mClipperList.clear();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setSlicer(vvSlicer * slicer) {
  mSlicer = slicer;  
  // Create an actor for each time slice
  for (unsigned int numImage = 0; numImage < mSlicer->GetImage()->GetVTKImages().size(); numImage++) {
    vtkImageClip * mClipper = vtkImageClip::New();
    vtkMarchingSquares * mSquares = vtkMarchingSquares::New();
    vtkActor * mSquaresActor = vtkActor::New();
    createNewActor(&mSquaresActor, &mSquares, &mClipper, numImage);
    mSquaresActorList.push_back(mSquaresActor);
    mSquaresList.push_back(mSquares);
    mClipperList.push_back(mClipper);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setImage(vvImage::Pointer image) {
  for (unsigned int numImage = 0; numImage < image->GetVTKImages().size(); numImage++) {
    mClipperList[numImage]->SetInput(image->GetVTKImages()[numImage]);
  }
  mHiddenImageIsUsed = true;
  mHiddenImage = image;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setPreserveMemoryModeEnabled(bool b) {
  // FastCache mode work only if threshold is always the same
  if (mDisplayModeIsPreserveMemory == b) return;
  mDisplayModeIsPreserveMemory = b;
  if (!b) {
    hideActors();
    initializeCacheMode();
  }
  else {
    for(unsigned int d=0; d<mListOfCachedContourActors.size(); d++)
      mListOfCachedContourActors[d].clear();
    mListOfCachedContourActors.clear();
    showActors();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::setColor(double r, double g, double b) {
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetColor(r,g,b);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::SetLineWidth(double w) {
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->GetProperty()->SetLineWidth(w);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::hideActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
    mSquaresActorList[i]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvImageContour::showActors() {
  if (!mSlicer) return;
  mSlice = mSlicer->GetSlice();
  mTSlice = mSlicer->GetTSlice();
  //  for(unsigned int i=0; i<mSquaresActorList.size(); i++) {
  mSquaresActorList[mTSlice]->VisibilityOn();
  update(mValue);
  //}
}
//------------------------------------------------------------------------------

  
//------------------------------------------------------------------------------
void vvImageContour::update(double value) {
  //  DD(value);
  if (!mSlicer) return;
  if (mPreviousValue == value) {
    if (mPreviousSlice == mSlicer->GetSlice()) {
      if (mPreviousTSlice == mSlicer->GetTSlice()) {
	// DD("vvImageContour::update nothing");
        return; // Nothing to do
      }
    }
  }

  // Get current threshold value
  mValue = value;

  // Get current slice
  mSlice = mSlicer->GetSlice();
  //  DD(mDisplayModeIsPreserveMemory);
  if (mDisplayModeIsPreserveMemory) {
    updateWithPreserveMemoryMode();
  }
  else {
    updateWithFastCacheMode();
  }


  mSlicer->Render(); //DS ---> REMOVE ??


  mPreviousTSlice = mSlicer->GetTSlice();
  mPreviousSlice  = mSlicer->GetSlice();
  mPreviousValue  = value;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateWithPreserveMemoryMode() {
  // Only change actor visibility if tslice change
  //DD(mTSlice);
  //DD(mSlice);
  int mPreviousTslice = mTSlice;
  //    ;
  //   if (mTSlice != mSlicer->GetTSlice()) {
  //     if (mTSlice != -1) mTsliceToSetOff = mTSlice;
  //       mSquaresActorList[mTSlice]->VisibilityOff();
  mTSlice = mSlicer->GetTSlice();
  //   }
  //  else return;
  //  DD(mTSlice);
  vtkMarchingSquares * mSquares = mSquaresList[mTSlice];
  vtkImageClip * mClipper = mClipperList[mTSlice];
  vtkActor * mSquaresActor = mSquaresActorList[mTSlice];
  int orientation = computeCurrentOrientation();
  //  DD(orientation);
  //DD(mValue);
  //DD(mSlice);
  //DD(mPreviousTslice);
  updateActor(mSquaresActor, mSquares, mClipper, mValue, orientation, mSlice);
  mSquaresActorList[mTSlice]->VisibilityOn();
  if (mPreviousTslice != mTSlice) {
    if (mPreviousTslice != -1) mSquaresActorList[mPreviousTslice]->VisibilityOff();
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::initializeCacheMode() {
  mPreviousSlice = mPreviousOrientation = 0;
  int dim = mSlicer->GetImage()->GetNumberOfDimensions();

  mListOfCachedContourActors.resize(dim);
  for(int d=0; d<dim; d++) {
    int size = mSlicer->GetImage()->GetSize()[d];
    //DD(size);
    mListOfCachedContourActors[d].resize(size);
    for(int j=0; j<size; j++) {
      mListOfCachedContourActors[d][j] = NULL;
    }
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
int vvImageContour::computeCurrentOrientation() {
  // Get extent of image in the slicer
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();
  
  // Compute orientation
  int orientation;
  for (orientation = 0; orientation < 6;orientation = orientation+2) {
    if (extent[orientation] == extent[orientation+1]) {
      break;
    }
  }
  orientation = orientation/2;
  return orientation;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateWithFastCacheMode() {
  // Compute orientation
  int orientation = computeCurrentOrientation();

  if ((mPreviousSlice == mSlice) && (mPreviousOrientation == orientation)) return;

  vtkActor * actor = mListOfCachedContourActors[orientation][mSlice];
  if (actor != NULL) {
    mListOfCachedContourActors[orientation][mSlice]->VisibilityOn();
  }
  else {
    vtkImageClip * mClipper;
    vtkMarchingSquares * mSquares;
    vtkActor * mSquaresActor;
    createNewActor(&mSquaresActor, &mSquares, &mClipper, 0);
    updateActor(mSquaresActor, mSquares, mClipper, mValue, orientation, mSlice);
    mListOfCachedContourActors[orientation][mSlice] = mSquaresActor;
    mSquaresActor->VisibilityOn();
  }

  if (mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice] != NULL)
    mListOfCachedContourActors[mPreviousOrientation][mPreviousSlice]->VisibilityOff();
  mPreviousSlice = mSlice;
  mPreviousOrientation = orientation;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::createNewActor(vtkActor ** actor, 
				    vtkMarchingSquares ** squares, 
				    vtkImageClip ** clipper, 
                                    int numImage) {
  vtkActor * mSquaresActor = (*actor = vtkActor::New());
  vtkImageClip * mClipper = (*clipper = vtkImageClip::New());
  vtkMarchingSquares * mSquares = (*squares = vtkMarchingSquares::New());
  vtkPolyDataMapper * mSquaresMapper = vtkPolyDataMapper::New();
  
  if (mHiddenImageIsUsed) 
    mClipper->SetInput(mHiddenImage->GetVTKImages()[0]);
  else 
    mClipper->SetInput(mSlicer->GetImage()->GetVTKImages()[numImage]);
  mSquares->SetInput(mClipper->GetOutput());
  mSquaresMapper->SetInput(mSquares->GetOutput());
  mSquaresMapper->ScalarVisibilityOff();
  mSquaresActor->SetMapper(mSquaresMapper);
  mSquaresActor->GetProperty()->SetColor(1.0,0,0);
  mSquaresActor->SetPickable(0);
  mSquaresActor->VisibilityOff();
  mSlicer->GetRenderer()->AddActor(mSquaresActor);  
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageContour::updateActor(vtkActor * actor, 
				 vtkMarchingSquares * squares, 
				 vtkImageClip * clipper, 
				 double threshold, int orientation, int slice) {
  
  // Set parameter for the MarchigSquare
  squares->SetValue(0, threshold);

  // Get image extent
  int* extent = mSlicer->GetImageActor()->GetDisplayExtent();

  // Change extent if needed
  int* extent2;
  if (mHiddenImageIsUsed) {
    extent2 = new int[6];
    int * extent3;
    extent3 = mHiddenImage->GetFirstVTKImageData()->GetExtent();
    for(int i=0; i<6; i++) extent2[i] = extent3[i];
    
    double s = (double)extent[orientation*2]*(double)mSlicer->GetImage()->GetSpacing()[orientation]; // in mm
    s = s+mSlicer->GetImage()->GetOrigin()[orientation]; // from origin
    s = s-mHiddenImage->GetFirstVTKImageData()->GetOrigin()[orientation]; // from corner second image
    s = s/mHiddenImage->GetFirstVTKImageData()->GetSpacing()[orientation]; // in voxel
    
    if (s == floor(s)) { 
      extent2[orientation*2] = extent2[orientation*2+1] = (int)floor(s);
    }
    else {
      extent2[orientation*2] = (int)floor(s);
      extent2[orientation*2+1] = extent2[orientation*2];
    }
  }
  else {
    extent2 = extent;
  }
  clipper->SetOutputWholeExtent(extent2[0],extent2[1],extent2[2],
                                extent2[3],extent2[4],extent2[5]);

  if (mHiddenImage) delete extent2;

  // Move the actor to be visible
  // DD(orientation);
//   DD(slice);

  //TO SIMPLiFY :!!!!!!!!! == ???????
  // actor->SetPosition(-1,-1,-1);
 
  switch (orientation)  {
  case 0: 
    // DD(mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0]);
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[0] > slice) {
      actor->SetPosition(1,0,0);
    }
    else {
      actor->SetPosition(-1,0,0);
    }
    break;
  case 1: 
    // DD(mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1]);
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[1] > slice) {
      actor->SetPosition(0,1,0);
    }
    else {
      actor->SetPosition(0,-1,0);
    }
    break;
  case 2: 
    // DD(mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2]);
    if (mSlicer->GetRenderer()->GetActiveCamera()->GetPosition()[2] > slice) {
      // DD("1");
      actor->SetPosition(0,0,1);
    }
    else {
      // DD("-1");
      actor->SetPosition(0,0,-1);
    }
    break;
  }

  squares->Update();
}
//------------------------------------------------------------------------------


