/*=========================================================================

  Program:   Medical Imaging & Interaction Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/ for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mitkPointSetInteractor.h"
#include <mitkPointOperation.h>
#include <mitkPositionEvent.h>
#include "mitkPointSet.h"
#include "mitkStatusBar.h"
#include <mitkDataTreeNode.h>
#include <mitkInteractionConst.h>
#include "mitkAction.h"

//how precise must the user pick the point
//default value
const int PRECISION = 5;

//##ModelId=3F017B3200F5
mitk::PointSetInteractor::PointSetInteractor(const char * type, DataTreeNode* dataTreeNode, int n)
	:Interactor(type, dataTreeNode), m_N(n), m_Precision(PRECISION)
{
  m_LastPoint.Fill(0);
  m_SumVec.Fill(0);
}

//##ModelId=3F017B320103
mitk::PointSetInteractor::~PointSetInteractor()
{
}

void mitk::PointSetInteractor::SetPrecision(unsigned int precision)
{
  m_Precision = precision;
}


//##ModelId=3F017B320121
void mitk::PointSetInteractor::UnselectAll()
{
	mitk::PointSet* pointSet = dynamic_cast<mitk::PointSet*>(m_DataTreeNode->GetData());
	if (pointSet == NULL)
		return;

  mitk::PointSet::DataType *itkPointSet = pointSet->GetPointSet();
  mitk::PointSet::PointsContainer::Iterator it, end;
  end = itkPointSet->GetPoints()->End();
  for (it = itkPointSet->GetPoints()->Begin(); it != end; it++)
	{
    int position = it->Index();
    PointSet::PointDataType pointData;
    itkPointSet->GetPointData(position, &pointData);
    if ( pointData.selected )//then declare an operation which unselects this point; UndoOperation as well!
		{
			mitk::Point3D noPoint;
			noPoint.Fill(0);
      mitk::PointOperation* doOp = new mitk::PointOperation(OpDESELECTPOINT, noPoint, position);
			if (m_UndoEnabled)
			{
				mitk::PointOperation* undoOp = new mitk::PointOperation(OpSELECTPOINT, noPoint, position);
				OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
				m_UndoController->SetOperationEvent(operationEvent);
			}
			pointSet->ExecuteOperation(doOp);
		}
	}
}

void mitk::PointSetInteractor::SelectPoint( int position )
{
  mitk::PointSet* pointSet = dynamic_cast<mitk::PointSet*>(m_DataTreeNode->GetData());
	if (pointSet == NULL)
		return;
  if (pointSet->GetSize()<=0)//if List is empty, then no select of a point can be done!
    return;

  mitk::Point3D noPoint;//dummyPoint... not needed anyway
  noPoint.Fill(0);
  mitk::PointOperation* doOp = new mitk::PointOperation(OpSELECTPOINT, noPoint, position);
	if (m_UndoEnabled)
	{
		mitk::PointOperation* undoOp = new mitk::PointOperation(OpDESELECTPOINT, noPoint, position);
		OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
		m_UndoController->SetOperationEvent(operationEvent);
	}
	pointSet->ExecuteOperation(doOp);
}

//##ModelId=3F017B320105
bool mitk::PointSetInteractor::ExecuteAction( Action* action, mitk::StateEvent const* stateEvent )
{
	bool ok = false;//for return type bool

	//checking corresponding Data; has to be a PointSet or a subclass
	mitk::PointSet* pointSet = dynamic_cast<mitk::PointSet*>(m_DataTreeNode->GetData());
	if (pointSet == NULL)
		return false;

  //for reading on the points, Id's etc
  mitk::PointSet::DataType *itkpointSet = pointSet->GetPointSet();
  mitk::PointSet::PointsContainer *points = itkpointSet->GetPoints();

  /*Each case must watch the type of the event!*/
  switch (action->GetActionId())
	{
  case AcDONOTHING:
    ok = true;
	  break;
  case AcCHECKOPERATION:
    //to check if the given Event is a PositionEvent. Other Events (DisplayEvent without WorldPosition) can't be yet used to add points due to a missing axsis.
    {
	    mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
		  if (posEvent != NULL) 
      {
        mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDYES, stateEvent->GetEvent());
        this->HandleEvent( newStateEvent );
      }
      else 
      {
        mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDNO, stateEvent->GetEvent());
        this->HandleEvent( newStateEvent );
      }
    }
    ok = true;
    break;
	case AcADDPOINT:
		/*
		declare two operations: one for the selected state: deselect the last one selected and select the new one
		the other operation is the add operation: There the first empty place have to be found and the new point inserted into that space
    */
	{
	  mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
		if (posEvent == NULL) 
      //check if it is a DisplayEvent thrown in a 3D window.
      //Then the z-information is missing.
      //returning false might end in the state full, but the last point couldn't be added, so the set wouldn't be full.
      //so a extra Action that checks the operationtype has been added.
      return false;

		mitk::Point3D itkPoint;
		itkPoint = posEvent->GetWorldPosition();

    //undo-supported deselect of all points in the DataList; if List is empty, then nothing will be unselected
		this->UnselectAll();
	  
    //find the position, the point is to be added to: first entry with empty index.
    //if the Set is empty, then start with 0. if not empty, then take the first index not occupied
    int lastPosition = 0;
    if (!points->empty())
    {
      mitk::PointSet::PointsIterator it, end;
      it = points->Begin();
      end = points->End();
      while( it != end )
      {
        if (!points->IndexExists(lastPosition))
          break;
        ++it;
        ++lastPosition;
      }
    }

		PointOperation* doOp = new mitk::PointOperation(OpINSERT, itkPoint, lastPosition);
		if (m_UndoEnabled)
		{
		  //difference between OpDELETE and OpREMOVE is, that OpDELETE deletes a point at the end, and OpREMOVE deletes it from the given position
      //remove is better, cause we need the position to add or remove the point anyway. We can get the last position from size()
			PointOperation* undoOp = new mitk::PointOperation(OpREMOVE, itkPoint, lastPosition);
			OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
			m_UndoController->SetOperationEvent(operationEvent);
		}
		//execute the Operation
		pointSet->ExecuteOperation(doOp);

    //outquoted, cause change in PointSet: the point is added and directly selected.
		////now select the new point
		////lastposition was the size, then the list got incremended, now lastposition is the index of the last element in list
		//PointOperation* doSelOp = new mitk::PointOperation(OpSELECTPOINT, itkPoint, lastPosition);
		//if (m_UndoEnabled)
		//{
		//	PointOperation* undoSelOp = new mitk::PointOperation(OpDESELECTPOINT, itkPoint, lastPosition);
		//	OperationEvent *selOperationEvent = new OperationEvent(pointSet, doSelOp, undoSelOp);
		//	m_UndoController->SetOperationEvent(selOperationEvent);
		//}
		//pointSet->ExecuteOperation(doSelOp);

		ok = true;
    break;
	}
  case AcINITMOVEMENT:
  {
    mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
		if (posEvent == NULL)
      return false;

    //start of the Movement is stored to calculate the undoKoordinate in FinishMovement
    m_LastPoint = posEvent->GetWorldPosition();
    
    //initialize a value to calculate the movement through all MouseMoveEvents from MouseClick to MouseRelease
    m_SumVec.Fill(0);
    
    ok = true;
    break;
  }
	case AcMOVESELECTED://moves all selected Elements
	{
    mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
		if (posEvent == NULL)
      return false;

    mitk::Point3D newPoint, resultPoint;
		newPoint = posEvent->GetWorldPosition();
		//search the elements in the list, that are selected
    //then calculate the vector, because only with the vector we can move several elements in the same direction
    //newPoint - lastPoint = vector
    //then move all selected and set the lastPoint = newPoint.
    //then add all vectors to a summeryVector (to be able to calculate the startpoint for undoOperation)
    mitk::Vector3D dirVector = newPoint - m_LastPoint;

    //summ up all Movement for Undo in FinishMovement
    m_SumVec = m_SumVec + dirVector;

    mitk::PointSet::PointsIterator it, end;
    it = points->Begin();
    end = points->End();
    while( it != end )
    {
      int position = it->Index();
      if ( pointSet->GetSelectInfo(position) )//if selected
      {
        PointSet::PointType pt = pointSet->GetPoint(position);
        mitk::Point3D sumVec;
        sumVec[0] = pt[0];
        sumVec[1] = pt[1];
        sumVec[2] = pt[2];
        resultPoint = sumVec + dirVector;
        PointOperation* doOp = new mitk::PointOperation(OpMOVE, resultPoint, position);

        //execute the Operation
        //here no undo is stored, because the movement-steps aren't interesting. only the start and the end is interisting to store for undo.
			  pointSet->ExecuteOperation(doOp);
      }
      ++it;
    }
    m_LastPoint = newPoint;//for calculation of the direction vector
    ok = true;
    break;
	}
	case AcREMOVEPOINT://remove the given Point from the list
	{
    //if the point to be removed is given by the positionEvent:
	  mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
		if (posEvent != NULL)
		{
			mitk::Point3D itkPoint;
			itkPoint = posEvent->GetWorldPosition();

			//search the point in the list
			int position = pointSet->SearchPoint(itkPoint, 0);
			//distance set to 0, cause we already got the exact point from last State checkpoint
			//but we also need the position in the list to remove it
			if (position>=0)//found a point
			{
        PointSet::PointType pt = pointSet->GetPoint(position);
				itkPoint[0] = pt[0];
        itkPoint[1] = pt[1];
        itkPoint[2] = pt[2];

        //Undo
        PointOperation* doOp = new mitk::PointOperation(OpREMOVE, itkPoint, position);
				if (m_UndoEnabled)	//write to UndoMechanism
        {
					PointOperation* undoOp = new mitk::PointOperation(OpINSERT, itkPoint, position);
		  		OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
				  m_UndoController->SetOperationEvent(operationEvent);
				}
				//execute the Operation
				pointSet->ExecuteOperation(doOp);

          /*now select the point "position-1", 
            and if it is the first in list, 
            then contine at the last in list*/
          if (pointSet->GetSize()>0)//only then a select of a point is possible!
          {
            if (position>0)//not the first in list
            {
              SelectPoint( position-1 );
            }
            else//it was the first point in list, that was removed, so select the last in list
            {
              position = pointSet->GetSize()-1;//last in list
              SelectPoint( position );
            }//else
          }//if

    		ok = true;
			}
    }
    else //no position is given so remove all selected elements
		{
		  //delete all selected points
      //search for the selected one and then declare the operations!
      mitk::PointSet::PointsContainer::Iterator it, end;
      it = points->Begin();
      end = points->End();
      int position = 0;
      while (it != end)
	    {
        if (  pointSet->GetSelectInfo(it->Index()) )//if point is selected
        {
          //get the coordinates of that point to be undoable
          PointSet::PointType selectedPoint = it->Value();
          mitk::Point3D itkPoint;
          itkPoint[0] = selectedPoint[0];
          itkPoint[1] = selectedPoint[1];
          itkPoint[2] = selectedPoint[2];
          
          position = it->Index();
          PointOperation* doOp = new mitk::PointOperation(OpREMOVE, itkPoint, position);
	        //Undo
				  if (m_UndoEnabled)	//write to UndoMechanism
				  {
					  PointOperation* undoOp = new mitk::PointOperation(OpINSERT, itkPoint, position);
					  OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
					  m_UndoController->SetOperationEvent(operationEvent);
				  }
          pointSet->ExecuteOperation(doOp);
          
          //after delete the iterator is undefined, so start again and count to the entry before!
          //if there ist only one point, then rather start at the begining, cause that is the point then.
          it = points->Begin();
          if (points->Size()>1)
          {
            for (int counter = 0; counter < position-1; counter++)
              it++;
          }

          
        }//if
        it++;
      }//while
      /*now select the point before the point/points that was/were deleted*/
      if (pointSet->GetSize()>0)//only then a select of a point is possible!
      {
        int aPosition = position;//to not loose the position of the deleted point
        bool found = false;
        
        --aPosition;//begin before the one deleted
        for (;aPosition >= 0; --aPosition)//search backwards
        {
          if (points->IndexExists(aPosition))
          {
            found = true;
            break;
          }
        }
        
        if (!found)//no element before position! so look afterwards!
        {
          mitk::PointSet::PointsContainer::Iterator it, end;
          it = points->Begin();
          end = points->End();
          //go to the position in front of the point, that was deleted
          for (int counter = 0; counter < position-1; ++counter)
    	    {
            ++it;
          }
          aPosition = it->Index();
        }
        SelectPoint( aPosition );
      }//if
      ok = true;
    }//else
  }
	break;
	//case AcDELETEPOINT://delete last element in list
	//	{
	//		mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
	//		if (posEvent == NULL) return false;

	//		mitk::Point3D itkPoint;
	//		itkPoint = posEvent->GetWorldPosition();

	//		int position = pointSet->GetSize()-1;//not needed for the doOp, but for UndoOp
	//		PointOperation* doOp = new mitk::PointOperation(OpDELETE, itkPoint, position);
	//		//Undo
	//		if (m_UndoEnabled)	//write to UndoMechanism
	//		{
	//			PointOperation* undoOp = new mitk::PointOperation(OpADD, itkPoint, position);
	//			OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
	//			m_UndoController->SetOperationEvent(operationEvent);
	//		}
	//		//execute the Operation
	//		pointSet->ExecuteOperation(doOp);
	//		ok = true;
	//	}
	//	break;

	case AcCHECKELEMENT:
		/*checking if the Point transmitted is close enough to one point. Then generate a new event with the point and let this statemaschine handle the event.*/
		{
      mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
			if (posEvent != NULL)
      {
        mitk::Point3D worldPoint = posEvent->GetWorldPosition();

			  int position = pointSet->SearchPoint(worldPoint, m_Precision);
			  if (position>=0)//found a point near enough to the given point
			  {
          PointSet::PointType pt = pointSet->GetPoint(position);//get that point, the one meant by the user!
				  mitk::Point2D displPoint;
          displPoint[0] = worldPoint[0]; displPoint[1] = worldPoint[1];
          //new Event with information YES and with the correct point
  			  mitk::PositionEvent const* newPosEvent = new mitk::PositionEvent(posEvent->GetSender(), Type_None, BS_NoButton, BS_NoButton, Key_none, displPoint, pt);
          mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDYES, newPosEvent);
          //call HandleEvent to leave the guard-state
          this->HandleEvent( newStateEvent );
				  ok = true;
			  }
			  else
			  {
				  //new Event with information NO
          mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDNO, posEvent);
          this->HandleEvent(newStateEvent );
				  ok = true;
			  }
		  }
      else
      {
        mitk::DisplayPositionEvent const  *disPosEvent = dynamic_cast <const mitk::DisplayPositionEvent *> (stateEvent->GetEvent());
			  if (disPosEvent != NULL)
        {//2d Koordinates for 3D Interaction; return false to redo the last statechange
          mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDNO, posEvent);
          this->HandleEvent(newStateEvent );
				  ok = true;
        }
      }
      break;
    }
    case AcCHECKSELECTED:
        /*check, if the given point is selected:
          if no, then send EIDNO
          if yes, then send EIDYES*/

        //check, if: because of the need to look up the point again, it is possible, that we grab the wrong point in case there are two same points
        //so maybe we do have to set a global index for further computation, as long, as the mouse is moved...
        {
          int position = -1;
          mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
			    if (posEvent == NULL)
            return false;
          mitk::Point3D worldPoint = posEvent->GetWorldPosition();

          position = pointSet->SearchPoint(worldPoint, m_Precision);

			    if (position>=0)
			    {
				    mitk::PositionEvent const  *newPosEvent = new mitk::PositionEvent(posEvent->GetSender(),
					  posEvent->GetType(), posEvent->GetButton(), posEvent->GetButtonState(), posEvent->GetKey(),
					  posEvent->GetDisplayPosition(), posEvent->GetWorldPosition());

				    if (pointSet->GetSelectInfo(position))//if selected on true, then call Event EIDYES
            {
              mitk::StateEvent* newStateEvent = new mitk::StateEvent( EIDYES, newPosEvent );
              this->HandleEvent( newStateEvent );
				      ok = true;

              //saving the spot for calculating the direction vector in moving
					    m_LastPoint = posEvent->GetWorldPosition();
            }
            else //not selected then call event EIDNO
            {
              //new Event with information NO
              mitk::StateEvent* newStateEvent = new mitk::StateEvent( EIDNO, newPosEvent );
              this->HandleEvent( newStateEvent );
				      ok = true;
            }
          }
          else //the position wasn't set properly. If necessary: search the given point in list and set var position
          {
          mitk::StatusBar::DisplayText("Message from mitkPointSetInteractor: Error in Actions! Check Config XML-file", 10000);
            ok = false;
          }
        }
        break;
    case AcCHECKNMINUS1://generate Events if the set will be full after the addition of the point or not.
        {
          if (m_N<0)//number of points not limited->pass on "Amount of points in Set is smaller then N-1"
          {
            mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDSTSMALERNMINUS1, stateEvent->GetEvent());
            this->HandleEvent( newStateEvent );
				    ok = true;
          }
          else 
          {
            if (pointSet->GetSize()<(m_N-1))
            //pointset after addition won't be full
            {
              mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDSTSMALERNMINUS1, stateEvent->GetEvent());
              this->HandleEvent( newStateEvent );
				      ok = true;
            }
            else //(pointSet->GetSize()>=(m_N-1))
            //after the addition of a point, the container will be full
            {
              mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDSTLARGERNMINUS1, stateEvent->GetEvent());
              this->HandleEvent(newStateEvent );
				      ok = true;
            }//else
          }//else
        }
        break;
    case AcCHECKEQUALS1:
        {
            if (pointSet->GetSize()<=1)//the number of points in the list is 1 (or smaler)
            {
                mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDYES, stateEvent->GetEvent());
                this->HandleEvent( newStateEvent );
				        ok = true;
            }
            else //more than 1 points in list, so stay in the state!
            {
                mitk::StateEvent* newStateEvent = new mitk::StateEvent(EIDNO, stateEvent->GetEvent());
                this->HandleEvent(newStateEvent );
				        ok = true;
            }
        }
        break;
	case AcSELECTPICKEDOBJECT://and deselect others
		{
      mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
			if (posEvent == NULL) return false;

			mitk::Point3D itkPoint;
			itkPoint = posEvent->GetWorldPosition();

			//search the point in the list
			int position = pointSet->SearchPoint(itkPoint, 0);
			//distance set to 0, cause we already got the exact point from last State checkpoint
			//but we also need the position in the list to move it
			if (position>=0)//found a point
			{
        //first deselect the other points
	      this->UnselectAll();//undoable deselect of all points in the DataList

        PointOperation* doOp = new mitk::PointOperation(OpSELECTPOINT, itkPoint, position);

        //Undo
				if (m_UndoEnabled)	//write to UndoMechanism
        {
					PointOperation* undoOp = new mitk::PointOperation(OpDESELECTPOINT, itkPoint, position);
					OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
  				m_UndoController->SetOperationEvent(operationEvent);
				}

				//execute the Operation
				pointSet->ExecuteOperation(doOp);
				ok = true;
			}
		}
		break;
	case AcDESELECTOBJECT:
		{
      mitk::PositionEvent const  *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
			if (posEvent == NULL) 
        return false;

			mitk::Point3D itkPoint;
			itkPoint = posEvent->GetWorldPosition();

			//search the point in the list
			int position = pointSet->SearchPoint(itkPoint, 0);
			//distance set to 0, cause we already got the exact point from last State checkpoint
			//but we also need the position in the list to move it
			if (position>=0)//found a point
			{
        //Undo
        PointOperation* doOp = new mitk::PointOperation(OpDESELECTPOINT, itkPoint, position);
				if (m_UndoEnabled)	//write to UndoMechanism
        {
					PointOperation* undoOp = new mitk::PointOperation(OpSELECTPOINT, itkPoint, position);
					OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
					m_UndoController->SetOperationEvent(operationEvent);
				}
				//execute the Operation
				pointSet->ExecuteOperation(doOp);
				ok = true;
			}
		}
		break;
    case AcDESELECTALL:
		{
            this->UnselectAll();//undo-supported able deselect of all points in the DataList
            ok = true;
        }
	    break;
	case AcFINISHMOVEMENT:
		{
      mitk::PositionEvent const *posEvent = dynamic_cast <const mitk::PositionEvent *> (stateEvent->GetEvent());
			if (posEvent == NULL)
        return false;

      //finish the movement:
      //the final point is m_LastPoint
      //m_SumVec stores the movement in a vector
      //the operation would not be necessary, but we need it for the undo Operation.
      //m_LastPoint is for the Operation
      //the point for undoOperation calculates from all selected elements (point) - m_SumVec

      //search all selected elements and move them with undo-functionality.
      mitk::PointSet::PointsIterator it, end;
      it = points->Begin();
      end = points->End();
      while( it != end )
      {
        int position = it->Index();
        if ( pointSet->GetSelectInfo(position) )//if selected
        {
          PointSet::PointType pt = pointSet->GetPoint(position);
          Point3D itkPoint;
          itkPoint[0] = pt[0];
          itkPoint[1] = pt[1];
          itkPoint[2] = pt[2];
          PointOperation* doOp = new mitk::PointOperation(OpMOVE, itkPoint, position);
	        if ( m_UndoEnabled )//&& (posEvent->GetType() == mitk::Type_MouseButtonRelease)
          {
            //set the undo-operation, so the final position is undo-able
            //calculate the old Position from the already moved position - m_SumVec
            mitk::Point3D undoPoint = ( itkPoint - m_SumVec );
            PointOperation* undoOp = new mitk::PointOperation(OpMOVE, undoPoint, position);
				    OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
			      m_UndoController->SetOperationEvent(operationEvent);
			    }
			    //execute the Operation
			    pointSet->ExecuteOperation(doOp);
        }
        ++it;
      }

      //set every variable for movement calculation to sero
			m_LastPoint.Fill(0);
      m_SumVec.Fill(0);
      //increase the GroupEventId, so that the Undo goes to here
			this->IncCurrGroupEventId();
			ok = true;
		}
		break;
	case AcCLEAR:
		{
			this->Clear();
		}
	default:
    mitk::StatusBar::DisplayText("Message from mitkPointSetInteractor: I do not understand the Action!", 10000);
	  ok = false;
    //a false here causes the statemachine to undo its last statechange.
    //otherwise it will end up in a different state, but without done Action.
    //if a transition really has no Action, than call donothing
	}
  
  return ok;
}

void mitk::PointSetInteractor::Clear()
{
	mitk::Point3D point;
  point.Fill(0);

	mitk::PointSet* pointSet = dynamic_cast<mitk::PointSet*>(m_DataTreeNode->GetData());
	if (pointSet == NULL)
		return;
	
  //for reading on the points, Id's etc
  mitk::PointSet::PointsContainer *points = pointSet->GetPointSet()->GetPoints();

  mitk::PointSet::PointsIterator it, end;
  it = points->Begin();
  end = points->End();
  while( it != end )
  {
    point = pointSet->GetPoint(it->Index());
    PointOperation* doOp = new mitk::PointOperation(OpDELETE, point, it->Index());
		if (m_UndoEnabled)	//write to UndoMechanism/ Can probably be removed!
		{
			PointOperation* undoOp = new mitk::PointOperation(OpADD, point, it->Index());
			OperationEvent *operationEvent = new OperationEvent(pointSet, doOp, undoOp);
			m_UndoController->SetOperationEvent(operationEvent);
	  }
		//execute the Operation
		pointSet->ExecuteOperation(doOp);
    ++it;
  }
}

