#include "StateMachine.h"
#include "StateMachineFactory.h"
#include "StateTransitionOperation.h"
#include "OperationEvent.h"
#include "UndoController.h"



//##ModelId=3E5B2DB301FD
mitk::StateMachine::StateMachine(std::string type)
: m_Type(type)
{
	StateMachineFactory* stateMF = new StateMachineFactory();
	m_CurrentState = stateMF->GetStartState(type);
	delete stateMF;
}

//##ModelId=3E5B2E660087
std::string mitk::StateMachine::GetType() const
{
	return m_Type;
}

//##ModelId=3E5B2DE30378
bool mitk::StateMachine::HandleEvent(StateEvent const* stateEvent)
{
	if (m_CurrentState == NULL)
		return false;//m_CurrentState needs to be set first!

	const Transition *tempTransition = m_CurrentState->GetTransition(stateEvent->GetId());
	State *tempState = tempTransition->GetNextState();
	if (tempState == NULL)
		return false;
	int tempSideEffectId = tempTransition->GetSideEffectId();

	//UNDO StateTransitionOperation
	StateTransitionOperation* undoSTO = new StateTransitionOperation(mitk::STATETRANSITION, m_CurrentState);
	StateTransitionOperation* redoSTO = new StateTransitionOperation(mitk::STATETRANSITION, tempState);//tempStateZeiger? m�gl. Fehlerquelle
	
	OperationEvent *operationEvent = new OperationEvent(((mitk::OperationActor*)(this)), undoSTO, redoSTO, 
		OperationEvent::GenerateObjectEventId(), OperationEvent::GenerateGroupEventId());

	UndoController *undoController = new UndoController(mitk::LIMITEDLINEARUNDO);//switch to LLU or add LLU
	undoController->SetOperationEvent(operationEvent);

	//first following StateChange, then operation(SideEffect)
	m_CurrentState = tempState;
	bool ok = ExecuteSideEffect(tempSideEffectId);//Undo in ExecuteSideEffect(...)
	return ok;
}

//##ModelId=3EAEEDC603D9
void mitk::StateMachine::SetOperation(Operation* operation)
{
//what to do?
}

