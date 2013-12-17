/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
/*=========================================================================
*
*  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
*
*  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
*
*  For complete copyright, license and disclaimer of warranty information
*  please refer to the NOTICE file at the top of the ITK source tree.
*
*=========================================================================*/
#ifndef __itkMultiGaussianImageSource_hxx
#define __itkMultiGaussianImageSource_hxx
#include <iostream>
#include <fstream>
#include <time.h>
#include "itkMultiGaussianImageSource.h"
#include "itkImageRegionIterator.h"
#include "itkObjectFactory.h"
#include "itkProgressReporter.h"
#include "stdlib.h"

namespace itk
{
  /**
  *
  */
  template< class TOutputImage >
  MultiGaussianImageSource< TOutputImage >
    ::MultiGaussianImageSource()
  {
    //Initial image is 100 wide in each direction.
    for ( unsigned int i = 0; i < TOutputImage::GetImageDimension(); i++ )
    {
      m_Size[i] = 100;
      m_Spacing[i] = 1.0;
      m_Origin[i] = 0.0;
      m_SphereMidpoint[i] = 0;
    }

    m_NumberOfGaussians = 0;
    m_Radius = 1;
    m_RadiusStepNumber = 5;
    m_MeanValue = 0;

    m_Min = NumericTraits< OutputImagePixelType >::NonpositiveMin();
    m_Max = NumericTraits< OutputImagePixelType >::max();
  }

  template< class TOutputImage >
  MultiGaussianImageSource< TOutputImage >
    ::~MultiGaussianImageSource()
  {}

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetSize(SizeValueArrayType sizeArray)
  {
    const unsigned int count = TOutputImage::ImageDimension;
    unsigned int       i;

    for ( i = 0; i < count; i++ )
    {
      if ( sizeArray[i] != this->m_Size[i] )
      {
        break;
      }
    }
    if ( i < count )
    {
      this->Modified();
      for ( i = 0; i < count; i++ )
      {
        this->m_Size[i] = sizeArray[i];
      }
    }
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::SizeValueType *
    MultiGaussianImageSource< TOutputImage >
    ::GetSize() const
  {
    return this->m_Size.GetSize();
  }

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetSpacing(SpacingValueArrayType spacingArray)
  {
    const unsigned int count = TOutputImage::ImageDimension;
    unsigned int       i;

    for ( i = 0; i < count; i++ )
    {
      if ( spacingArray[i] != this->m_Spacing[i] )
      {
        break;
      }
    }
    if ( i < count )
    {
      this->Modified();
      for ( i = 0; i < count; i++ )
      {
        this->m_Spacing[i] = spacingArray[i];
      }
    }
  }

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetOrigin(PointValueArrayType originArray)
  {
    const unsigned int count = TOutputImage::ImageDimension;
    unsigned int       i;

    for ( i = 0; i < count; i++ )
    {
      if ( originArray[i] != this->m_Origin[i] )
      {
        break;
      }
    }
    if ( i < count )
    {
      this->Modified();
      for ( i = 0; i < count; i++ )
      {
        this->m_Origin[i] = originArray[i];
      }
    }
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::PointValueType *
    MultiGaussianImageSource< TOutputImage >
    ::GetOrigin() const
  {
    for ( unsigned int i = 0; i < TOutputImage::ImageDimension; i++ )
    {
      this->m_OriginArray[i] = this->m_Origin[i];
    }
    return this->m_OriginArray;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::SpacingValueType *
    MultiGaussianImageSource< TOutputImage >
    ::GetSpacing() const
  {
    for ( unsigned int i = 0; i < TOutputImage::ImageDimension; i++ )
    {
      this->m_SpacingArray[i] = this->m_Spacing[i];
    }
    return this->m_SpacingArray;
  }

  /**
  *
  */
  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::PrintSelf(std::ostream & os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Max: "
      << static_cast< typename NumericTraits< OutputImagePixelType >::PrintType >( m_Max )
      << std::endl;
    os << indent << "Min: "
      << static_cast< typename NumericTraits< OutputImagePixelType >::PrintType >( m_Min )
      << std::endl;

    os << indent << "Origin: [";
    unsigned int ii = 0;
    while( ii < TOutputImage::ImageDimension - 1 )
    {
      os << m_Origin[ii] << ", ";
      ++ii;
    }
    os << m_Origin[ii] << "]" << std::endl;

    os << indent << "Spacing: [";
    ii = 0;
    while( ii < TOutputImage::ImageDimension - 1 )
    {
      os << m_Spacing[ii] << ", ";
      ++ii;
    }
    os << m_Spacing[ii] << "]" << std::endl;

    os << indent << "Size: [";
    ii = 0;
    while( ii < TOutputImage::ImageDimension - 1 )
    {
      os << m_Size[ii] << ", ";
      ++ii;
    }
    os << m_Size[ii] << "]" << std::endl;
  }

  template< class TOutputImage >
  unsigned int
    MultiGaussianImageSource< TOutputImage >
    ::GetNumberOfGaussians() const
  {
    return this->m_NumberOfGaussians;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::RadiusType
    MultiGaussianImageSource< TOutputImage >
    ::GetRadius() const
  {
    return this->m_Radius;
  }

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetRadius( RadiusType  radius )
  {
    this->m_Radius = radius;
  }

  template< class TOutputImage >
  const int
    MultiGaussianImageSource< TOutputImage >
    ::GetRadiusStepNumber() const
  {
    return this->m_RadiusStepNumber;
  }

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetRadiusStepNumber( unsigned int stepNumber )
  {
    this->m_RadiusStepNumber = stepNumber;
  }

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetNumberOfGausssians( unsigned int n )
  {
    this->m_NumberOfGaussians = n;
  }


  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetRegionOfInterest( ItkVectorType roiMin, ItkVectorType roiMax )
  {
    m_RegionOfInterestMax.operator=(roiMax);
    m_RegionOfInterestMin.operator=(roiMin);
  }


  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::OutputImagePixelType
    MultiGaussianImageSource< TOutputImage >
    ::GetMaxMeanValue() const
  {
    return m_MeanValue;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::OutputImagePixelType
    MultiGaussianImageSource< TOutputImage >
    ::GetMaxValueInSphere() const
  {
    return m_MaxValueInSphere;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::IndexType
    MultiGaussianImageSource< TOutputImage >
    ::GetMaxValueIndexInSphere() const
  {
    return  m_MaxValueIndexInSphere;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::OutputImagePixelType
    MultiGaussianImageSource< TOutputImage >
    ::GetMinValueInSphere() const
  {
    return m_MinValueInSphere;
  }

  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::IndexType
    MultiGaussianImageSource< TOutputImage >
    ::GetMinValueIndexInSphere() const
  {
    return  m_MinValueIndexInSphere;
  }
  //----------------------------------------------------------------------------
  template< class TOutputImage >
  const typename MultiGaussianImageSource< TOutputImage >::IndexType
    MultiGaussianImageSource< TOutputImage >
    ::GetSphereMidpoint() const
  {
    return m_SphereMidpoint;
  }

  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  double
    MultiGaussianImageSource< TOutputImage >
    ::MultiGaussianFunctionValueAtCuboid(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax)
  {
    double mean = 0;
    double summand0, summand1, summand2, value, factor;

    for(unsigned int n =0; n < m_NumberOfGaussians; ++n)
    {
      summand0 = FunctionPhi( (xMax - m_CenterX[n]) / m_SigmaX[n] ) + FunctionPhi( (xMin - m_CenterX[n]) / m_SigmaX[n] );
      summand1 = FunctionPhi( (yMax - m_CenterY[n]) / m_SigmaY[n] ) + FunctionPhi( (yMin - m_CenterY[n]) / m_SigmaY[n] );
      summand2 = FunctionPhi( (zMax - m_CenterZ[n]) / m_SigmaZ[n] ) + FunctionPhi( (zMin - m_CenterZ[n]) / m_SigmaZ[n] );

      value = summand0 * summand1 * summand2;
      factor = (m_SigmaX[n] * m_SigmaY[n] * m_SigmaZ[n] * pow(2.0 * itk::Math::pi, 1.5 )); // ((xMax - xMin) * (yMax - yMin) * (zMax - zMin));
      mean = mean + m_Altitude[n] * factor * value; //  * m_Altitude[n] ????

    }
    return mean;
  }
  //---------------------------------------------------------------------------------------------------------------------


  template< class TOutputImage >
  double
    MultiGaussianImageSource< TOutputImage >
    ::FunctionPhi(double value)
  {
    double phiAtValue;

    // TODO qubische interpolation
    int indexValue = static_cast<int>( 100 * value);
    if (value < 0.0)
    {
      phiAtValue = 1.0 - m_NormalDistValues[- indexValue + 1];
    }
    if(indexValue > 410)
    {
      phiAtValue = 1;
    }
    phiAtValue = m_NormalDistValues[indexValue];
    return  phiAtValue;
  }
  //----------------------------------------------------------------------------------------------------------------------
  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::InsertPoints( PointType globalCoordinateMidpointCuboid, double cuboidRadius)
  {
    //TODO letzter Punkt entfernen.....!

    PointType edgePoint;
    MapContainerPoints::ElementIdentifier identifier;
    VectorType edgesIds;
    MapContainerPoints::ElementIdentifier id = m_Edges.Size() + 1;
    MapContainerPoints::ElementIdentifier id1 = m_Midpoints.Size() + 1;
    for(int i = -1; i < 2; i+=2)
    {
      for(int k = -1; k < 2; k+=2)
      {
        for(int j = -1; j < 2; j+=2)
        {
          edgePoint[0] = globalCoordinateMidpointCuboid[0] + i * cuboidRadius;
          edgePoint[1] = globalCoordinateMidpointCuboid[1] + k * cuboidRadius;
          edgePoint[2] = globalCoordinateMidpointCuboid[2] + j * cuboidRadius;

          identifier = m_EdgePoints.Size() + 1;
          m_EdgePoints.InsertElement(identifier, globalCoordinateMidpointCuboid);
          edgesIds.push_back(identifier);
        }
      }
    }

    m_Midpoints.InsertElement(id1, globalCoordinateMidpointCuboid);
    m_RadiusCuboid.InsertElement(id1, cuboidRadius);
    m_Edges.InsertElement(id, edgesIds);

  }

  //----------------------------------------------------------------------------------------------------------------------
  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::CalculateEdgesInSphere( PointType globalCoordinateMidpointCuboid, PointType globalCoordinateMidpointSphere, double cuboidRadius)
  {
    double cuboidRadiusRecursion = cuboidRadius;
    double minCuboidRadius = m_Spacing[0] / 5.0;
    bool exit = 0;
    while(cuboidRadiusRecursion > minCuboidRadius && !exit)
    {
      int intersect = IntesectTheSphere( globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadiusRecursion);

      if( intersect == 1)
      {
        cuboidRadiusRecursion = cuboidRadiusRecursion / 2.0;
        for(int i = -1; i < 2; i+=2)
        {
          for(int k = -1; k < 2; k+=2)
          {
            for(int j = -1; j < 2; j+=2)
            {
              // cuboid intersect the sphere
              PointType newMidpoint;

              newMidpoint[0] = globalCoordinateMidpointCuboid[0] + static_cast<double>(i) * cuboidRadiusRecursion;
              newMidpoint[1] = globalCoordinateMidpointCuboid[1] + static_cast<double>(k) * cuboidRadiusRecursion;
              newMidpoint[2] = globalCoordinateMidpointCuboid[2] + static_cast<double>(j) * cuboidRadiusRecursion;

              CalculateEdgesInSphere( newMidpoint, globalCoordinateMidpointSphere, cuboidRadiusRecursion);
            }
          }
        }
      }
      else if(intersect == 2)
      {
        // cuboid in the sphere
        // insert the edge points of the cuboid
        InsertPoints(globalCoordinateMidpointCuboid, cuboidRadiusRecursion);
        exit = 1;

        //    WriteXMLToTestTheCuboidInsideTheSphere();

      }
      else if (intersect == 0)
      {
        // cuboid not in the sphere
        exit = 1;
      }

    }

  }


  //-----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::GenerateCuboidSegmentationInSphere()
  {

    double cuboidRadius;


    PointType globalCoordinateMidpointCuboid;
    PointType globalCoordinateMidpointSphere;
    IndexType index;
    OutputImageRegionType  regionOfInterest;
    IndexType indexR;
    indexR.SetElement(0, m_RegionOfInterestMin[0]);
    indexR.SetElement(1, m_RegionOfInterestMin[1]);
    indexR.SetElement(2, m_RegionOfInterestMin[2]);
    regionOfInterest.SetIndex(indexR);
    SizeType sizeROI;
    sizeROI.SetElement(0, m_RegionOfInterestMax[0] - m_RegionOfInterestMin[0] +1);
    sizeROI.SetElement(1, m_RegionOfInterestMax[1] - m_RegionOfInterestMin[1] +1);
    sizeROI.SetElement(2, m_RegionOfInterestMax[2] - m_RegionOfInterestMin[2] +1);
    regionOfInterest.SetSize(sizeROI);
    typename TOutputImage::Pointer image = this->GetOutput(0);
    IteratorType regionOfInterestIterator(image, regionOfInterest);

    for(regionOfInterestIterator.GoToBegin(); !regionOfInterestIterator.IsAtEnd(); ++regionOfInterestIterator)
    {
      cuboidRadius = m_Radius / 2.0;
      index = regionOfInterestIterator.GetIndex();
      image->TransformIndexToPhysicalPoint(index, globalCoordinateMidpointSphere);
      for(int i = -1; i < 2; i++)
      {
        for(int k = -1; k < 2; k++)
        {
          for(int j = -1; j < 2; j++)
          {
            PointType newMidpoint;
            newMidpoint[0] = globalCoordinateMidpointSphere[0] + static_cast<double>(i) * cuboidRadius;
            newMidpoint[1] = globalCoordinateMidpointSphere[1] + static_cast<double>(k) * cuboidRadius;
            newMidpoint[2] = globalCoordinateMidpointSphere[2] + static_cast<double>(j) * cuboidRadius;
            CalculateEdgesInSphere( newMidpoint, globalCoordinateMidpointSphere, cuboidRadius);
          }
        }
      }




      WriteXMLToTestTheCuboidInsideTheSphere();
      //to test only one step!!! TODO
      regionOfInterestIterator.GoToReverseBegin();
    }

  }


  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::CalculateTheMidPointMeanValueInCuboid()
  {

    m_MeanValue = 0.0;
    double meanValueTemp;
    PointType midpoint;
    MapContainerPoints::ElementIdentifier cuboidNumber = m_Midpoints.Size();
    SetNormalDistributionValues();
    double radius;
    double xMin, xMax, yMin, yMax, zMin, zMax;


    PointType globalCoordinateMidpointSphere;
    IndexType index;
    OutputImageRegionType  regionOfInterest;
    IndexType indexR;
    indexR.SetElement(0, m_RegionOfInterestMin[0]);
    indexR.SetElement(1, m_RegionOfInterestMin[1]);
    indexR.SetElement(2, m_RegionOfInterestMin[2]);
    regionOfInterest.SetIndex(indexR);
    SizeType sizeROI;
    sizeROI.SetElement(0, m_RegionOfInterestMax[0] - m_RegionOfInterestMin[0] +1);
    sizeROI.SetElement(1, m_RegionOfInterestMax[1] - m_RegionOfInterestMin[1] +1);
    sizeROI.SetElement(2, m_RegionOfInterestMax[2] - m_RegionOfInterestMin[2] +1);
    regionOfInterest.SetSize(sizeROI);
    typename TOutputImage::Pointer image = this->GetOutput(0);
    IteratorType regionOfInterestIterator(image, regionOfInterest);

    for(regionOfInterestIterator.GoToBegin(); !regionOfInterestIterator.IsAtEnd(); ++regionOfInterestIterator)
    {

      index = regionOfInterestIterator.GetIndex();
      image->TransformIndexToPhysicalPoint(index, globalCoordinateMidpointSphere);

      meanValueTemp = 0.0;
      for(unsigned int i = 0; i < cuboidNumber; i++ )
      {

        midpoint = m_Midpoints.ElementAt(i);
        midpoint[0] = midpoint[0] + globalCoordinateMidpointSphere[0];
        midpoint[2] = midpoint[2] + globalCoordinateMidpointSphere[2];
        midpoint[1] = midpoint[1] + globalCoordinateMidpointSphere[1];
        radius= m_RadiusCuboid.ElementAt(i);
        xMin =   midpoint[0] - radius;
        xMax =   midpoint[0] + radius;
        yMin =   midpoint[1] - radius;
        yMax =   midpoint[1] + radius;
        zMin =   midpoint[2] - radius;
        zMax =   midpoint[2] + radius;
        meanValueTemp  = meanValueTemp + MultiGaussianFunctionValueAtCuboid( xMin, xMax, yMin, yMax, zMin, zMax );
      }

      meanValueTemp = meanValueTemp / (m_Radius * itk::Math::pi * itk::Math::pi);
      if(meanValueTemp > m_MeanValue)
      {
        m_GlobalCoordinate = globalCoordinateMidpointSphere;
        m_MeanValue = meanValueTemp;
        m_SphereMidpoint = index;
      }

    }

  }

  //-----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::CalculateMidpoint()
  {
    double valueMean  = 0;
    m_MeanValue = 0.0;
    double sideLength = m_Radius;
    double cuboidRadius;
    double sideLengthNew = m_Radius;
    double meanValueTemp;
    bool intersect;
    PointType globalCoordinateMidpointCuboid;
    PointType globalCoordinateMidpointSphere;
    IndexType index;
    OutputImageRegionType  regionOfInterest;
    IndexType indexR;
    indexR.SetElement(0, m_RegionOfInterestMin[0]);
    indexR.SetElement(1, m_RegionOfInterestMin[1]);
    indexR.SetElement(2, m_RegionOfInterestMin[2]);
    regionOfInterest.SetIndex(indexR);
    SizeType sizeROI;
    sizeROI.SetElement(0, m_RegionOfInterestMax[0] - m_RegionOfInterestMin[0] +1);
    sizeROI.SetElement(1, m_RegionOfInterestMax[1] - m_RegionOfInterestMin[1] +1);
    sizeROI.SetElement(2, m_RegionOfInterestMax[2] - m_RegionOfInterestMin[2] +1);
    regionOfInterest.SetSize(sizeROI);
    typename TOutputImage::Pointer image = this->GetOutput(0);
    IteratorType regionOfInterestIterator(image, regionOfInterest);
    SetNormalDistributionValues();

    for(regionOfInterestIterator.GoToBegin(); !regionOfInterestIterator.IsAtEnd(); ++regionOfInterestIterator)
    {
      cuboidRadius = sideLength / 2.0;
      meanValueTemp = 0.0;
      index = regionOfInterestIterator.GetIndex();
      image->TransformIndexToPhysicalPoint(index, globalCoordinateMidpointSphere);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] - cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] - cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] - cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] + cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] - cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] - cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] + cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] + cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] - cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] + cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] + cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] + cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] + cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] - cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] + cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere,
        cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] - cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] + cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] + cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] - cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] + cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] - cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

      globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointSphere[0] - cuboidRadius;
      globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointSphere[1] - cuboidRadius;
      globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointSphere[2] + cuboidRadius;
      meanValueTemp = meanValueTemp + Quadtrees(globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere,
        cuboidRadius, meanValueTemp);



      meanValueTemp = meanValueTemp / (m_Radius * itk::Math::pi * itk::Math::pi);
      if(meanValueTemp > m_MeanValue)
      {
        m_GlobalCoordinate = globalCoordinateMidpointSphere;
        m_MeanValue = meanValueTemp;
        m_SphereMidpoint = index;
      }


      //to test only one step!!! TODO
      regionOfInterestIterator.GoToReverseBegin();

    }
  }
  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  double
    MultiGaussianImageSource< TOutputImage >
    ::Quadtrees( PointType globalCoordinateMidpointCuboid, PointType globalCoordinateMidpointSphere,  double cuboidRadius, double meanValueTemp)
  {
    double xMin, xMax, yMin, yMax, zMin, zMax;
    double minDistance = m_Spacing[0] / 2.0;
    unsigned int intersect;

    while(cuboidRadius > minDistance)
    {
      intersect = IntesectTheSphere( globalCoordinateMidpointCuboid, globalCoordinateMidpointSphere, cuboidRadius);
      if( intersect == 1 )
      {
        cuboidRadius = cuboidRadius / 2.0;
        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;
        meanValueTemp = meanValueTemp +  Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);

        globalCoordinateMidpointCuboid[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
        globalCoordinateMidpointCuboid[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
        globalCoordinateMidpointCuboid[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;
        meanValueTemp =  meanValueTemp + Quadtrees(  globalCoordinateMidpointCuboid,  globalCoordinateMidpointSphere, cuboidRadius, meanValueTemp);


      }
      else if( intersect == 2 )
      {

        xMin = globalCoordinateMidpointCuboid[0] - cuboidRadius;
        xMax = globalCoordinateMidpointCuboid[0] + cuboidRadius;
        yMin = globalCoordinateMidpointCuboid[1] - cuboidRadius;
        yMax = globalCoordinateMidpointCuboid[1] + cuboidRadius;
        zMin = globalCoordinateMidpointCuboid[2] - cuboidRadius;
        zMax = globalCoordinateMidpointCuboid[2] + cuboidRadius;

        meanValueTemp = meanValueTemp + MultiGaussianFunctionValueAtCuboid( xMin,  xMax,  yMin,  yMax,  zMin, zMax);
        cuboidRadius = minDistance;
      }
      else if( intersect == 0 )
      {
        cuboidRadius = minDistance;
      }
    }
    return meanValueTemp;
  }
  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  unsigned int
    MultiGaussianImageSource< TOutputImage >
    ::IntesectTheSphere( PointType globalCoordinateMidpointCuboid, PointType globalCoordinateMidpointSphere,  double cuboidRadius)
  {

    unsigned int intersect = 1;
    PointType cuboidEdge1, cuboidEdge2, cuboidEdge3, cuboidEdge4, cuboidEdge5, cuboidEdge6, cuboidEdge7, cuboidEdge8;
    int count = 0, index = 0;

    cuboidEdge1[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
    cuboidEdge1[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
    cuboidEdge1[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;

    cuboidEdge2[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
    cuboidEdge2[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
    cuboidEdge2[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;

    cuboidEdge3[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
    cuboidEdge3[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
    cuboidEdge3[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;

    cuboidEdge4[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
    cuboidEdge4[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
    cuboidEdge4[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;

    cuboidEdge5[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
    cuboidEdge5[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
    cuboidEdge5[2] = globalCoordinateMidpointCuboid[2] - cuboidRadius;

    cuboidEdge6[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
    cuboidEdge6[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
    cuboidEdge6[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;

    cuboidEdge7[0] = globalCoordinateMidpointCuboid[0] - cuboidRadius;
    cuboidEdge7[1] = globalCoordinateMidpointCuboid[1] - cuboidRadius;
    cuboidEdge7[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;

    cuboidEdge8[0] = globalCoordinateMidpointCuboid[0] + cuboidRadius;
    cuboidEdge8[1] = globalCoordinateMidpointCuboid[1] + cuboidRadius;
    cuboidEdge8[2] = globalCoordinateMidpointCuboid[2] + cuboidRadius;



    count =  (globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge1) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge2) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge3) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge4) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge5) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge6) < m_Radius)  +
      ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge7) < m_Radius)  + ( globalCoordinateMidpointSphere.EuclideanDistanceTo(cuboidEdge8) < m_Radius);
    if ( count == 0 )
    {
      intersect = 0;
    }
    if (count == 8 )
    {
      intersect = 2;
    }

    //if(count != 0 && count != 8)
    //{
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge1[0]);
    //  m_YCoordToTest.push_back(cuboidEdge1[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge1[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge2[0]);
    //  m_YCoordToTest.push_back(cuboidEdge2[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge2[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge3[0]);
    //  m_YCoordToTest.push_back(cuboidEdge3[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge3[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge4[0]);
    //  m_YCoordToTest.push_back(cuboidEdge4[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge4[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge5[0]);
    //  m_YCoordToTest.push_back(cuboidEdge5[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge5[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge6[0]);
    //  m_YCoordToTest.push_back(cuboidEdge6[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge6[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge7[0]);
    //  m_YCoordToTest.push_back(cuboidEdge7[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge7[2]);
    //  //to write a point set
    //  m_XCoordToTest.push_back(cuboidEdge8[0]);
    //  m_YCoordToTest.push_back(cuboidEdge8[1]);
    //  m_ZCoordToTest.push_back(cuboidEdge8[2]);
    //  WriteXMLToTestTheCuboid();

    //}

    return intersect;
  }

  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  const double
    MultiGaussianImageSource< TOutputImage >
    ::MultiGaussianFunctionValueAtPoint(double x, double y, double z)
  {
    double summand0, summand1, summand2, power, value = 0;
    // the for-loop represent  the sum of the gaussian function
    for(unsigned int n =0; n < m_NumberOfGaussians; ++n)
    {
      summand0 = (x - m_CenterX[n]) / m_SigmaX[n];
      summand1 = (y - m_CenterY[n]) / m_SigmaY[n];
      summand2 = (z - m_CenterZ[n]) / m_SigmaZ[n];

      power = summand0 * summand0 + summand1 * summand1 + summand2 * summand2;
      value = value + m_Altitude[n] * pow(itk::Math::e, -0.5 * power);
    }
    return value;
  }

  //----------------------------------------------------------------------------------------------------------------------

  template< class TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::AddGaussian( VectorType x, VectorType y, VectorType z, VectorType sx, VectorType sy, VectorType sz, VectorType altitude)
  {
    for(unsigned int i = 0; i < x.size();  ++i)
    {
      m_CenterX.push_back(x[i]);
      m_CenterY.push_back(y[i]);
      m_CenterZ.push_back(z[i]);
      m_SigmaX.push_back(sx[i]);
      m_SigmaY.push_back(sy[i]);
      m_SigmaZ.push_back(sz[i]);
      m_Altitude.push_back(altitude[i]);
    }
  }

  //----------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::GenerateOutputInformation()
  {
    TOutputImage *output;
    IndexType     index;
    index.Fill(0);
    output = this->GetOutput(0);
    typename TOutputImage::RegionType largestPossibleRegion;
    largestPossibleRegion.SetSize(this->m_Size);
    largestPossibleRegion.SetIndex(index);
    output->SetLargestPossibleRegion(largestPossibleRegion);
    output->SetSpacing(m_Spacing);
    output->SetOrigin(m_Origin);
  }

  //----------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::GenerateData()
  {
    itkDebugMacro(<< "Generating a  image of scalars ");
    double valueReal;
    IndexType index;
    typedef typename TOutputImage::PixelType scalarType;
    typename TOutputImage::Pointer image = this->GetOutput(0);
    image = this->GetOutput(0);
    image->SetBufferedRegion( image->GetRequestedRegion() );
    image->Allocate();
    IteratorType imageIt(image, image->GetLargestPossibleRegion());
    PointType globalCoordinate;
    for(imageIt.GoToBegin(); !imageIt.IsAtEnd(); ++imageIt)
    {
      valueReal = 0.0;
      index = imageIt.GetIndex();
      image->TransformIndexToPhysicalPoint(imageIt.GetIndex(), globalCoordinate);
      valueReal = MultiGaussianFunctionValueAtPoint(globalCoordinate[0], globalCoordinate[1] ,globalCoordinate[2]);
      imageIt.Set(valueReal);
    }
  }

  //----------------------------------------------------------------------------
  /*
  This class allows by the method CalculateMidpointAndMeanValue() to find a sphere with a specified radius that has a maximal mean value over all      sphere with that radius with midpoint inside or at the boundary of the image. The parameter RadiusStepNumber controls the accuracy of that           calculation (the higher the value the higher the exactness).
  The algorithm works as follows:
  1. the first three for-loops traverse the region of interest and assume the current point to be the wanted sphere midpoint
  2. calculate the mean value for that sphere (use sphere coordinates):
  2.1. traverse the radius of the sphere with step size Radius divided by RadiusStepNumber (the for-loop with index i)
  2.2. define a variable dist, which gives a approximately distance between the points at the sphere surface
  (here we take such a distance, that on the smallest sphere are located 8 points)
  2.3. calculate the angles so that the points are equally spaced on the surface (for-loops with indexes j and k)
  2.3. for all radius length add the values at the points on the sphere and divide by the number of points added
  (the values at each point is calculate with the method MultiGaussianFunctionValueAtPoint())
  3. Compare with the until-now-found-maximum and take the bigger one
  */
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::CalculateMidpointAndMeanValue()
  {
    itkDebugMacro(<< "Generating a  image of scalars ");

    double numberSummand = 0.0;
    int fiStepNumber, psiStepNumber;
    double x, y, z, temp, abst;
    MultiGaussianImageSource< TOutputImage >::OutputImagePixelType value, meanValueTemp, valueAdd;
    PointType globalCoordinate;
    IndexType index;
    double riStep, fijStep, psikStep, ri, fij, psik, rNew;
    double dist = itk::Math::pi * m_Radius / static_cast< double >(2 * m_RadiusStepNumber);
    m_MeanValue = 0.0;
    riStep = m_Radius / static_cast< double >( m_RadiusStepNumber );
    OutputImageRegionType  regionOfInterest;
    IndexType indexR;
    indexR.SetElement(0, m_RegionOfInterestMin[0]);
    indexR.SetElement(1, m_RegionOfInterestMin[1]);
    indexR.SetElement(2, m_RegionOfInterestMin[2]);
    regionOfInterest.SetIndex(indexR);
    SizeType sizeROI;
    sizeROI.SetElement(0, m_RegionOfInterestMax[0] - m_RegionOfInterestMin[0] +1);
    sizeROI.SetElement(1, m_RegionOfInterestMax[1] - m_RegionOfInterestMin[1] +1);
    sizeROI.SetElement(2, m_RegionOfInterestMax[2] - m_RegionOfInterestMin[2] +1);
    regionOfInterest.SetSize(sizeROI);
    typename TOutputImage::Pointer image = this->GetOutput(0);
    IteratorType regionOfInterestIterator(image, regionOfInterest);

    for(regionOfInterestIterator.GoToBegin(); !regionOfInterestIterator.IsAtEnd(); ++regionOfInterestIterator)
    {
      numberSummand = 1.0;
      value = regionOfInterestIterator.Get();
      m_ValueAtMidpoint =  value;
      index = regionOfInterestIterator.GetIndex();


      image->TransformIndexToPhysicalPoint(index, globalCoordinate);
      ri = riStep;

      //for(unsigned int i = 0; i < m_RadiusStepNumber; ++i)
      //{
      //  angleStepNumberOverTwo = static_cast<int>( itk::Math::pi * ri / dist);
      //  //fij = 0.0;
      //  fijStep = itk::Math::pi / static_cast< double >( angleStepNumberOverTwo );
      //  fij = fijStep;
      //  std::cout << "i =         " << i << "    r = " << ri << "\n" << std::endl;
      //  for(unsigned int j = 0; j < angleStepNumberOverTwo -1 ; ++j) // from 0 to pi    j<= angleStep...
      //  {
      //    z = ri * cos(fij);
      //    psikStep = 2.0 * itk::Math::pi / (2.0 * static_cast< double >( angleStepNumberOverTwo ) );
      //    psik =  -itk::Math::pi + psikStep;
      //    temp =  ri * sin(fij);
      //    std::cout << "    j =     " << j << "    fi = " << fij << "\n" << std::endl;
      //    for(unsigned int k = 0; k < 2 * angleStepNumberOverTwo; ++k) // from -pi to pi
      //    {
      //      std::cout << "       k =  " << k << "    psi = " << psik << "\n" << std::endl;
      //      x = temp * cos(psik);
      //      y = temp * sin(psik);
      //      numberSummand += 1.0;
      //  //    std::cout << "x, y, z:    " << x <<"    " << y << "     " << z << "\n" <<std::endl;
      //   //   std::cout << "psi, fi, r: "<< psik <<"    " << fij << "     " << ri << "\n" <<std::endl;
      //      valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], y + globalCoordinate[1], z + globalCoordinate[2]);
      //      value = value + valueAdd;
      //      psik = psik + psikStep;
      //      //double tempX = x + globalCoordinate[0];
      //      //double tempY = y + globalCoordinate[1];
      //      //double tempZ = z + globalCoordinate[2];
      //      //if ( tempX < xMin ) xMin = tempX;
      //      //if ( tempY < yMin ) yMin = tempY;
      //      //if ( tempZ < zMin ) zMin = tempZ;
      //      //if ( tempX > xMax ) xMax = tempX;
      //      //if ( tempY > yMax ) yMax = tempY;
      //      //if ( tempZ > zMax ) zMax = tempZ;
      //
      //    }
      //    fij = fij  + fijStep;
      //  }
      //  ri = ri + riStep;
      //}
      // second implementation ... corrected!
      //  for(unsigned int i = 0; i < m_RadiusStepNumber; ++i)
      //{
      //  angleStepNumberOverTwo = static_cast<int>( itk::Math::pi * ri / dist);
      //  //fij = 0.0;
      //  fijStep = itk::Math::pi / static_cast< double >( angleStepNumberOverTwo );
      //  fij = fijStep;
      //  //        std::cout << "i =         " << i << "    r = " << ri << "\n" << std::endl;
      //  for(unsigned int j = 0; j < angleStepNumberOverTwo -1 ; ++j) // from 0 to pi    j<= angleStep...
      //  {
      //    z = ri * cos(fij);
      //    /* double l = 2.0 * ri /  static_cast< double >( angleStepNumberOverTwo - 1);
      //    double tempVar = (j + 1) * l;
      //    abst =  (tempVar < ri ) ? tempVar : (2.0 * ri - tempVar);
      //    double testVar = abst * (ri - abst);
      //    rNew = std::sqrt(abst * (ri - abst)); */
      //    if(fij < itk::Math::pi / 2.0)
      //    {
      //      rNew = sin(fij) * ri;
      //    }
      //    else
      //    {
      //      rNew = sin(itk::Math::pi - fij) * ri;
      //    }
      //    psiStepNumber = static_cast<int>( 2.0 * itk::Math::pi * rNew / dist);
      //    psikStep = 2.0 * itk::Math::pi / psiStepNumber  ;
      //    psik =  -itk::Math::pi + psikStep;
      //    temp =  ri * sin(fij);
      //    //         std::cout << "    j =     " << j << "    fi = " << fij << "\n" << std::endl;
      //    for(unsigned int k = 0; k < psiStepNumber; ++k) // from -pi to pi
      //    {
      //      //           std::cout << "       k =  " << k << "    psi = " << psik << "\n" << std::endl;
      //      x = temp * cos(psik);
      //      y = temp * sin(psik);
      //      numberSummand += 1.0;
      //      valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], y + globalCoordinate[1], z + globalCoordinate[2]);
      //      value = value + valueAdd;
      //      psik = psik + psikStep;
      //    }
      //    fij = fij  + fijStep;
      //  }
      //  ri = ri + riStep;
      //}


      for(unsigned int i = 0; i < m_RadiusStepNumber; ++i)
      {
        fiStepNumber = static_cast<int>( (itk::Math::pi * ri / dist));
        //fij = 0.0;
        fijStep = itk::Math::pi / (static_cast< double >( fiStepNumber ) * 2.0);
        fij = fijStep;
        //       std::cout << "i =         " << i << "    r = " << ri << "\n" << std::endl;
        for(unsigned int j = 0; j < fiStepNumber -2 ; ++j) // from 0 to pi/2    j<= angleStep...
        {
          z = ri * cos(fij);
          if(fij < itk::Math::pi / 2.0)
          {
            rNew = sin(fij) * ri;
          }
          else
          {
            rNew = sin(itk::Math::pi - fij) * ri;
          }

          psiStepNumber = static_cast<int>(( 2.0 * itk::Math::pi * rNew / dist) / 4.0);
          psikStep = itk::Math::pi / (static_cast< double >( psiStepNumber ) * 2.0);
          psik = psikStep;
          temp =  ri * sin(fij);
          //  std::cout << "   j =      " << j << "    fi = " << fij << "\n" << std::endl;
          if(psiStepNumber > 2){
            for(unsigned int k = 0; k < psiStepNumber -2; ++k) // from 0 to pi/2
            {
              //   std::cout << "       k =  " << k << "    psi = " << psik << "\n" << std::endl;
              x = temp * cos(psik);
              y = temp * sin(psik);
              numberSummand += 8.0;


              valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], y + globalCoordinate[1], z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint( - x + globalCoordinate[0], y + globalCoordinate[1], z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint( - x + globalCoordinate[0], - y + globalCoordinate[1], - z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], - y + globalCoordinate[1], z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], - y + globalCoordinate[1], - z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint(x + globalCoordinate[0], y + globalCoordinate[1], - z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint(- x + globalCoordinate[0], y + globalCoordinate[1], - z + globalCoordinate[2]);
              value = value + valueAdd;
              valueAdd = MultiGaussianFunctionValueAtPoint(- x + globalCoordinate[0], - y + globalCoordinate[1], z + globalCoordinate[2]);
              value = value + valueAdd;




              psik = psik + psikStep;

            }
          }
          fij = fij  + fijStep;
        }
        ri = ri + riStep;
      }


      meanValueTemp = value / numberSummand;
      if(meanValueTemp > m_MeanValue)
      {
        m_GlobalCoordinate = globalCoordinate;
        m_MeanValue = meanValueTemp;
        m_SphereMidpoint = index;
      }
    }
  }

  //-------------------------------------------------------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::WriteXMLToTestTheCuboidInsideTheSphere()
  {
    std::stringstream                                                    ss;


    int numberSummand = 1.0;

    //write an  .mps test file
    itk::DOMNodeXMLWriter::Pointer                                       xmlWriter;
    typedef itk::DOMNode::Pointer                                        DOMNodeType;
    DOMNodeType                                                          domXML, domPointSetFile, domFileVersion, domPointSet, domPoint, domId, domX, domY, domZ;

    xmlWriter = itk::DOMNodeXMLWriter::New();
    domXML =  itk::DOMNode::New();
    domXML->SetName("?xml");
    domPointSetFile =  itk::DOMNode::New();
    domPointSetFile->SetName("point_set_file");
    //domFileVersion =  itk::DOMNode::New();
    //domFileVersion->SetName("file_version");
    domPointSet =  itk::DOMNode::New();
    domPointSet->SetName("point_set");

    ss.str("");
    ss << 1.0;
    domXML->SetAttribute("version", ss.str());
    domXML->AddChildAtBegin(domPointSetFile);
    //domPointSetFile -> AddChildAtBegin(domFileVersion);
    domPointSetFile -> AddChildAtBegin(domPointSet);


    int cap = m_Midpoints.Size();
    for(unsigned int  iter = 0 ; iter < cap; ++iter)
    {
      numberSummand += 1.0;

      domPoint =  itk::DOMNode::New();
      domPoint->SetName("point");
      domX =  itk::DOMNode::New();
      domX->SetName("x");
      domY =  itk::DOMNode::New();
      domY->SetName("y");
      domZ =  itk::DOMNode::New();
      domZ->SetName("z");
      domId =  itk::DOMNode::New();
      domId->SetName("id");


      ss.str("");
      ss << numberSummand;
      domId->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domId);

      double scaleFactor = 10.0;
      PointType point = m_Midpoints.GetElement(numberSummand);
      ss.str("");
      ss <<  point[0] * scaleFactor;
      domX->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domX);
      ss.str("");
      ss << point[1] * scaleFactor;
      domY->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domY);
      ss.str("");
      ss << point[2] * scaleFactor;
      domZ->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domZ);
      domPointSet -> AddChildAtEnd(domPoint);

    }


    // .mps (Data)
    ss.str("");
    ss << "C:/temp/CuboidsInTheSphere.mps";
    std::string name = ss.str();
    char * fileNamePointer = (char*) name.c_str();
    xmlWriter->SetFileName( fileNamePointer);
    xmlWriter->SetInput( domXML );
    xmlWriter->Update();




  }

  //---------------------------------------------------------------------------------------------------------------------

  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::WriteXMLToTestTheCuboid()
  {
    // to create a point set


    std::stringstream                                                    ss;
    int fiStepNumber, psiStepNumber;
    double x, y, z, temp;
    MultiGaussianImageSource< TOutputImage >::OutputImagePixelType  meanValueTemp;
    PointType globalCoordinate;
    double riStep, fijStep, psikStep, ri, fij, psik, rNew;


    int angleStepNumber = 4;
    double dist = 2.0 * itk::Math::pi * m_Radius / static_cast< double >(angleStepNumber * m_RadiusStepNumber);
    m_MeanValue = 0.0;
    riStep = m_Radius / static_cast< double >( m_RadiusStepNumber);
    int numberSummand = 1.0;

    //write an  .mps test file
    itk::DOMNodeXMLWriter::Pointer                                       xmlWriter;
    typedef itk::DOMNode::Pointer                                        DOMNodeType;
    DOMNodeType                                                          domXML, domPointSetFile, domFileVersion, domPointSet, domPoint, domId, domX, domY, domZ;

    xmlWriter = itk::DOMNodeXMLWriter::New();
    domXML =  itk::DOMNode::New();
    domXML->SetName("?xml");
    domPointSetFile =  itk::DOMNode::New();
    domPointSetFile->SetName("point_set_file");
    //domFileVersion =  itk::DOMNode::New();
    //domFileVersion->SetName("file_version");
    domPointSet =  itk::DOMNode::New();
    domPointSet->SetName("point_set");

    ss.str("");
    ss << 1.0;
    domXML->SetAttribute("version", ss.str());
    domXML->AddChildAtBegin(domPointSetFile);
    //domPointSetFile -> AddChildAtBegin(domFileVersion);
    domPointSetFile -> AddChildAtBegin(domPointSet);


    int cap = m_XCoordToTest.size();
    for(unsigned int  iter = 0 ; iter < cap; ++iter)
    {
      numberSummand += 1.0;

      domPoint =  itk::DOMNode::New();
      domPoint->SetName("point");
      domX =  itk::DOMNode::New();
      domX->SetName("x");
      domY =  itk::DOMNode::New();
      domY->SetName("y");
      domZ =  itk::DOMNode::New();
      domZ->SetName("z");
      domId =  itk::DOMNode::New();
      domId->SetName("id");


      ss.str("");
      ss << numberSummand;
      domId->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domId);

      double scaleFactor = 10.0;

      ss.str("");
      ss << m_XCoordToTest[iter] * scaleFactor;
      domX->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domX);
      ss.str("");
      ss << m_YCoordToTest[iter] * scaleFactor;
      domY->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domY);
      ss.str("");
      ss << m_ZCoordToTest[iter] * scaleFactor;
      domZ->AddTextChildAtBegin(ss.str());
      domPoint -> AddChildAtEnd(domZ);
      domPointSet -> AddChildAtEnd(domPoint);



    }


    // .mps (Data)
    ss.str("");
    ss << "C:/temp/CuboidMidpointsSet.mps";
    std::string name = ss.str();
    char * fileNamePointer = (char*) name.c_str();
    xmlWriter->SetFileName( fileNamePointer);
    xmlWriter->SetInput( domXML );
    xmlWriter->Update();


  }
  //------------------------------------------------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::WriteXMLToTest()
  {
    // to create a point set

    double numberSummand = 0.0;
    std::stringstream                                                    ss;
    int fiStepNumber, psiStepNumber;
    double x, y, z, temp;
    MultiGaussianImageSource< TOutputImage >::OutputImagePixelType  meanValueTemp;
    PointType globalCoordinate;
    double riStep, fijStep, psikStep, ri, fij, psik, rNew;


    int angleStepNumber = 4;
    double dist = 2.0 * itk::Math::pi * m_Radius / static_cast< double >(angleStepNumber * m_RadiusStepNumber);
    m_MeanValue = 0.0;
    riStep = m_Radius / static_cast< double >( m_RadiusStepNumber);
    numberSummand = 1.0;

    //write an  .mps test file
    itk::DOMNodeXMLWriter::Pointer                                       xmlWriter;
    typedef itk::DOMNode::Pointer                                        DOMNodeType;
    DOMNodeType                                                          domXML, domPointSetFile, domFileVersion, domPointSet, domPoint, domId, domX, domY, domZ;

    xmlWriter = itk::DOMNodeXMLWriter::New();
    domXML =  itk::DOMNode::New();
    domXML->SetName("?xml");
    domPointSetFile =  itk::DOMNode::New();
    domPointSetFile->SetName("point_set_file");
    //domFileVersion =  itk::DOMNode::New();
    //domFileVersion->SetName("file_version");
    domPointSet =  itk::DOMNode::New();
    domPointSet->SetName("point_set");

    ss.str("");
    ss << 1.0;
    domXML->SetAttribute("version", ss.str());
    domXML->AddChildAtBegin(domPointSetFile);
    //domPointSetFile -> AddChildAtBegin(domFileVersion);
    domPointSetFile -> AddChildAtBegin(domPointSet);


    typename TOutputImage::Pointer image = this->GetOutput(0);
    image->TransformIndexToPhysicalPoint(m_SphereMidpoint, globalCoordinate);
    ri = riStep;
    for(unsigned int i = 0; i < m_RadiusStepNumber; ++i)
    {
      fiStepNumber = static_cast<int>( (itk::Math::pi * ri / dist) / 2.0);
      //fij = 0.0;
      fijStep = itk::Math::pi / (static_cast< double >( fiStepNumber ) * 2.0);
      fij = fijStep;
      //        std::cout << "i =         " << i << "    r = " << ri << "\n" << std::endl;
      for(unsigned int j = 0; j < fiStepNumber - 2 ; ++j) // from 0 to pi/2    j<= angleStep...
      {
        z = ri * cos(fij);
        if(fij < itk::Math::pi / 2.0)
        {
          rNew = sin(fij) * ri;
        }
        else
        {
          rNew = sin(itk::Math::pi - fij) * ri;
        }

        psiStepNumber = static_cast<int>(( 2.0 * itk::Math::pi * rNew / dist) / 4.0);
        psikStep = itk::Math::pi / (static_cast< double >( psiStepNumber ) * 2.0);
        psik = psikStep;
        temp =  ri * sin(fij);

        for(unsigned int k = 0; k < psiStepNumber - 2; ++k) // from 0 to pi/2
        {
          x = temp * cos(psik);
          y = temp * sin(psik);
          numberSummand += 1.0;

          domPoint =  itk::DOMNode::New();
          domPoint->SetName("point");
          domX =  itk::DOMNode::New();
          domX->SetName("x");
          domY =  itk::DOMNode::New();
          domY->SetName("y");
          domZ =  itk::DOMNode::New();
          domZ->SetName("z");
          domId =  itk::DOMNode::New();
          domId->SetName("id");


          ss.str("");
          ss << numberSummand;
          domId->AddTextChildAtBegin(ss.str());
          domPoint -> AddChildAtEnd(domId);

          double scaleFactor = 40.0;

          ss.str("");
          ss << (x + globalCoordinate[0]) * scaleFactor;
          domX->AddTextChildAtBegin(ss.str());
          domPoint -> AddChildAtEnd(domX);
          ss.str("");
          ss << (y + globalCoordinate[1]) * scaleFactor;
          domY->AddTextChildAtBegin(ss.str());
          domPoint -> AddChildAtEnd(domY);
          ss.str("");
          ss << (z + globalCoordinate[2]) * scaleFactor;
          domZ->AddTextChildAtBegin(ss.str());
          domPoint -> AddChildAtEnd(domZ);
          domPointSet -> AddChildAtEnd(domPoint);


          psik = psik + psikStep;

        }
        fij = fij  + fijStep;
      }
      ri = ri + riStep;
    }


    // .mps (Data)
    ss.str("");
    ss << "C:/temp/SpherePointSet.mps";
    std::string name = ss.str();
    char * fileNamePointer = (char*) name.c_str();
    xmlWriter->SetFileName( fileNamePointer);
    xmlWriter->SetInput( domXML );
    xmlWriter->Update();


  }

  //-------------------------------------------------------------------------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::OptimizeMeanValue()
  {
    int radiusStepNumberOptimized = m_RadiusStepNumber * 4;
    int numberSummand = 1, angleStepNumberOverTwo;
    double x, y, z, temp;
    double riStep, fijStep, psikStep, ri, fij, psik;
    double dist = itk::Math::pi * m_Radius / (2 * radiusStepNumberOptimized);
    MultiGaussianImageSource< TOutputImage >::OutputImagePixelType valueAdd, value,
      m_MeanValue = 0;
    riStep = m_Radius / radiusStepNumberOptimized;
    ri = riStep;
    value =  m_ValueAtMidpoint;
    for(unsigned int i = 0; i < radiusStepNumberOptimized; ++i)
    {
      angleStepNumberOverTwo = static_cast<int>( itk::Math::pi * ri / dist);
      fij = 0.0;
      fijStep = itk::Math::pi / angleStepNumberOverTwo;
      for(unsigned int j = 0; j <= angleStepNumberOverTwo; ++j) // from 0 to pi
      {
        z = ri * cos(fij);
        psikStep = 2.0 * itk::Math::pi / (2.0 * angleStepNumberOverTwo);
        psik =  -itk::Math::pi + psikStep;
        temp =  ri * sin(fij);
        for(unsigned int k = 0; k < 2 * angleStepNumberOverTwo; ++k) // from -pi to pi
        {
          x = temp * cos(psik);
          y = temp * sin(psik);
          numberSummand++;
          valueAdd = MultiGaussianFunctionValueAtPoint(x + m_GlobalCoordinate[0] , y + m_GlobalCoordinate[1], z + m_GlobalCoordinate[2]);
          value = value + valueAdd;
          psik = psik + psikStep;
        }
        fij = fij  + fijStep;
      }
      ri = ri + riStep;
    }
    m_MeanValue = value / numberSummand;
  }


  //-------------------------------------------------------------------------------------------------------------------------------------------
  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::CalculateMaxAndMinInSphere()
  {
    IndexType index;
    MultiGaussianImageSource< TOutputImage >::OutputImagePixelType value;
    m_MaxValueInSphere = std::numeric_limits<OutputImagePixelType>::min();
    m_MinValueInSphere = std::numeric_limits<OutputImagePixelType>::max();
    int radInt;
    OutputImageRegionType  cuboidRegion;
    IndexType indexR;
    SizeType sizeR;
    int indexRegion, sizeRegion;
    for( unsigned int i = 0; i < 3; ++i )
    {
      radInt = static_cast<int>(m_Radius/m_Spacing[i]);
      indexRegion =  m_SphereMidpoint[i] - radInt;
      if( m_Origin[i] > indexRegion )
      {
        indexR.SetElement(i, m_Origin[i] );
      }
      else
      {
        indexR.SetElement(i, indexRegion );
      }
      sizeRegion = 2 *radInt + 1;

      if(indexR[i] + sizeRegion > m_Size[i])
      {
        sizeR.SetElement(i,  m_Size[i] );
      }
      else
      {
        sizeR.SetElement(i,  sizeRegion );
      }

    }
    cuboidRegion.SetIndex(indexR);
    cuboidRegion.SetSize(sizeR);
    typename TOutputImage::Pointer image = this->GetOutput(0);
    IteratorType cuboidRegionOfInterestIterator(image, cuboidRegion);
    PointType globalCoordinate;
    for(cuboidRegionOfInterestIterator.GoToBegin(); !cuboidRegionOfInterestIterator.IsAtEnd(); ++cuboidRegionOfInterestIterator)
    {
      index = cuboidRegionOfInterestIterator.GetIndex();
      if(IsInImage(index))
      {
        image->TransformIndexToPhysicalPoint(cuboidRegionOfInterestIterator.GetIndex(), globalCoordinate);
        if( m_GlobalCoordinate.EuclideanDistanceTo(globalCoordinate) <= m_Radius )
        {
          value = cuboidRegionOfInterestIterator.Get();
          if(m_MaxValueInSphere < value)
          {
            m_MaxValueInSphere = value;
            m_MaxValueIndexInSphere = index;
          }
          if(m_MinValueInSphere > value)
          {
            m_MinValueInSphere = value;
            m_MinValueIndexInSphere = index;
          }
        }
      }
    }
  }
  //--------------------------------------------------------------------------------------------------------------------------

  template< typename TOutputImage >
  bool
    MultiGaussianImageSource< TOutputImage >
    ::IsInImage(IndexType index)
  {
    bool isInImage = 0;
    if(index[0] >= m_Origin[0] && index[0] <= m_Size[0] && index[1] >= m_Origin[1] && index[1] <= m_Size[1] && index[2] >= m_Origin[2] && index[2] <= m_Size[2])
    {
      isInImage = 1;
    }
    return isInImage;
  }


  //-----------------------------------------------------------------------------------------------------------------------


  template< typename TOutputImage >
  void
    MultiGaussianImageSource< TOutputImage >
    ::SetNormalDistributionValues()
  {

    double temp[410] = { 0.5 ,       0.50399 ,       0.50798,       0.51197,       0.51595,       0.51994,       0.52392,       0.5279,       0.53188,       0.53586,       0.53983,       0.5438,       0.54776,       0.55172,       0.55567,       0.55962,       0.56356,       0.56749,       0.57142,       0.57535,       0.57926,       0.58317,       0.58706,       0.59095,       0.59483 ,       0.59871,       0.60257,       0.60642,       0.61026,       0.61409,       0.61791,       0.62172,       0.62552,       0.6293,       0.63307,       0.63683,       0.64058,       0.64431,       0.64803,       0.65173,       0.65542,       0.6591,       0.66276,       0.6664,       0.67003,       0.67364,       0.67724,       0.68082,       0.68439,       0.68793,       0.69146,       0.69497,       0.69847,       0.70194,       0.7054,       0.70884,       0.71226,       0.71566,       0.71904,       0.7224,       0.72575,       0.72907,       0.73237,       0.73565,       0.73891,       0.74215,       0.74537,       0.74857,       0.75175,       0.7549,       0.75804,       0.76115,       0.76424,       0.7673,       0.77035,       0.77337,       0.77637,       0.77935,       0.7823,       0.78524,       0.78814,       0.79103,       0.79389,       0.79673,       0.79955,       0.80234,       0.80511,       0.80785,       0.81057,       0.81327,       0.81594,       0.81859,       0.82121,       0.82381,       0.82639,       0.82894,       0.83147,       0.83398,       0.83646,       0.83891,       0.84134,       0.84375,       0.84614,       0.84849,       0.85083,       0.85314,       0.85543,       0.85769,       0.85993,       0.86214,       0.86433,       0.8665,       0.86864,       0.87076,       0.87286,       0.87493,       0.87698,       0.879,       0.881,       0.88298,       0.88493,       0.88686,       0.88877,       0.89065,       0.89251,       0.89435,       0.89617,       0.89796,       0.89973,       0.90147,       0.9032,       0.9049,       0.90658,       0.90824,       0.90988,       0.91149,       0.91309,       0.91466,       0.91621,       0.91774,       0.91924,       0.92073,       0.9222,       0.92364,       0.92507,       0.92647,       0.92785,       0.92922,       0.93056,       0.93189,       0.93319,       0.93448,       0.93574,       0.93699,       0.93822,       0.93943,       0.94062,       0.94179,       0.94295,       0.94408,       0.9452,       0.9463,       0.94738,       0.94845,       0.9495,       0.95053,       0.95154,       0.95254,       0.95352,       0.95449,       0.95543,       0.95637,       0.95728,       0.95818,       0.95907,       0.95994,       0.9608,       0.96164,       0.96246,       0.96327,       0.96407,       0.96485,       0.96562,       0.96638,       0.96712,       0.96784,       0.96856,       0.96926,       0.96995,       0.97062,       0.97128,       0.97193,       0.97257,       0.9732,       0.97381,       0.97441,       0.975,       0.97558,       0.97615,       0.9767,       0.97725,       0.97778,       0.97831,       0.97882,       0.97932,       0.97982,       0.9803,       0.98077,       0.98124,       0.98169,       0.98214,       0.98257,       0.983,       0.98341,       0.98382,       0.98422,       0.98461,       0.985,       0.98537,       0.98574,       0.9861,       0.98645,       0.98679,       0.98713,       0.98745,       0.98778,       0.98809,       0.9884,       0.9887,       0.98899,       0.98928,       0.98956,       0.98983,       0.9901,       0.99036,       0.99061,       0.99086,       0.99111,       0.99134,       0.99158,       0.9918,       0.99202,       0.99224,       0.99245,       0.99266,       0.99286,       0.99305,       0.99324,       0.99343,       0.99361,       0.99379,       0.99396,       0.99413,       0.9943,       0.99446,       0.99461,       0.99477,       0.99492,       0.99506,       0.9952,       0.99534,       0.99547,       0.9956,       0.99573,       0.99585,       0.99598,       0.99609,       0.99621,       0.99632,       0.99643,       0.99653,       0.99664,       0.99674,       0.99683,       0.99693,       0.99702,       0.99711,       0.9972,       0.99728,       0.99736,       0.99744,       0.99752,       0.9976,       0.99767,       0.99774,       0.99781,       0.99788,       0.99795,       0.99801,       0.99807,       0.99813,       0.99819,       0.99825,       0.99831,       0.99836,       0.99841,       0.99846,       0.99851,       0.99856,       0.99861,       0.99865,       0.99869,       0.99874,       0.99878,       0.99882,       0.99886,       0.99889,       0.99893,       0.99896,       0.999,       0.99903,       0.99906,       0.9991,       0.99913,       0.99916,       0.99918,       0.99921,       0.99924,       0.99926,       0.99929,       0.99931,       0.99934,       0.99936,       0.99938,       0.9994,       0.99942,       0.99944,       0.99946,       0.99948,       0.9995,       0.99952,       0.99953,       0.99955,       0.99957,       0.99958,       0.9996,       0.99961,       0.99962,       0.99964,       0.99965,       0.99966,       0.99968,       0.99969,       0.9997,       0.99971,       0.99972,       0.99973,       0.99974,       0.99975,       0.99976,       0.99977,       0.99978,       0.99978,       0.99979,       0.9998,       0.99981,       0.99981,       0.99982,       0.99983,       0.99983,       0.99984,       0.99985,       0.99985,       0.99986,       0.99986,       0.99987,       0.99987,       0.99988,       0.99988,       0.99989,       0.99989,       0.9999,       0.9999,       0.9999,       0.99991,       0.99991,       0.99992,       0.99992,       0.99992,       0.99992,       0.99993,       0.99993,       0.99993,       0.99994,       0.99994,       0.99994,       0.99994,       0.99995,       0.99995,       0.99995,       0.99995,       0.99995,       0.99996,       0.99996,       0.99996,       0.99996,       0.99996,       0.99996,       0.99997,       0.99997,       0.99997,       0.99997,       0.99997,       0.99997,       0.99997,       0.99997,       0.99998,       0.99998,       0.99998,       0.99998     };

    for(int i=0; i < 410; i++)
    {
      m_NormalDistValues[i] = temp[i];
    }

  }


} // end namespace itk

#endif


