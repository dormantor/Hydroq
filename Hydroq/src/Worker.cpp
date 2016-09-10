#include "Worker.h"
#include "HydroqGameModel.h"
#include "SteeringBehavior.h"
#include "MsgEvents.h"

// ======================= WORKER IDLE STATE ==============================


bool WorkerIdleState::FindTaskToDo() {
	// each 10 frame check if this is something to do...
	
	// position the worker stays
	auto start = owner->GetTransform().localPos;

	auto faction = owner->GetAttr<Faction>(ATTR_FACTION);
	auto allTasks = gameModel->GetGameTasksByFaction(faction);

	// order tasks by distance
	sort(allTasks.begin(), allTasks.end(),
		[start](const spt<GameTask>  a, const spt<GameTask> b) -> bool
	{
		return a->taskNode->GetTransform().localPos.distance(start) < b->taskNode->GetTransform().localPos.distance(start);
	});


	auto map = gameModel->GetMap();

	// get the nearest task
	for (auto& task : allTasks) {
		if (task->isReserved && task->IsNodeReserved(owner->GetId())) {
			// position of the place the bridge will stay
			auto position = task->taskNode->GetTransform().localPos;
			// node at position the bridge will stay
			auto mapNode = map->GetNode((int)position.x, (int)position.y);
			// position the worker stays
			auto start = owner->GetTransform().localPos;

			if (task->type == GameTaskType::BRIDGE_BUILD || task->type == GameTaskType::BRIDGE_DESTROY) {

				HydMapNode* nodeToWorkFrom;

				if (task->type == GameTaskType::BRIDGE_BUILD) {
					// find first safe platform the worker can stay on
					nodeToWorkFrom = mapNode->FindWalkableNeighbor(Vec2i(start.x, start.y));
				}
				else {
					// find platform the worker can return to base from
					auto nearestBase = gameModel->FindNearestRigByFaction(gameModel->GetFaction(), start);
					ofVec2f preferredPosition = (nearestBase != nullptr) ? nearestBase->GetTransform().localPos : start;
					nodeToWorkFrom = mapNode->FindWalkableNeighbor(Vec2i(preferredPosition.x, preferredPosition.y));
					if (nodeToWorkFrom == nullptr) nodeToWorkFrom = mapNode->FindNeighborByType(MapNodeType::RIG_PLATFORM, Vec2i(preferredPosition.x, preferredPosition.y));
				}


				if (nodeToWorkFrom != nullptr) {

					COGLOGDEBUG("Hydroq", "Got task for bridge building at position [%d,%d]", position.x, position.y);

					// change state from IDLE to BRIDGE_BUILD
					auto stateToChange = GetParent()->FindLocalState(StrId(STATE_WORKER_BUILD));
					auto buildState = static_cast<WorkerBridgeBuildState*>(stateToChange);
					buildState->SetGameTask(task);
					buildState->SetNodeToBuildFrom(nodeToWorkFrom);
					this->GetParent()->ChangeState(StrId(STATE_WORKER_BUILD));

					return true;
				}
			}
			else if (task->type == GameTaskType::ATTRACT) {
				float cardinality = gameModel->CalcAttractorAbsCardinality(faction, task->taskNode->GetId());
				int neededDistance = cardinality * 4;
				vector<HydMapNode*> nearestNodes;
				mapNode->FindWalkableNeighbor(neededDistance, nearestNodes);

				if (!nearestNodes.empty()) {
					auto randomNodeToFollow = nearestNodes[ofRandom(0, 1)*nearestNodes.size()];

					COGLOGDEBUG("Hydroq", "Got task for attractor following at position [%d,%d]", randomNodeToFollow->pos.x, randomNodeToFollow->pos.y);

					// change state from IDLE to BRIDGE_BUILD
					auto stateToChange = GetParent()->FindLocalState(StrId(STATE_WORKER_ATTRACTOR_FOLLOW));
					auto buildState = static_cast<WorkerAttractorFollowState*>(stateToChange);
					buildState->SetGameTask(task);
					buildState->SetNodeToFollow(randomNodeToFollow);
					this->GetParent()->ChangeState(StrId(STATE_WORKER_ATTRACTOR_FOLLOW));

					return true;
				}
			}
		}
	}

	return false;
}

void WorkerIdleState::MoveAround() {
	// move around...
	if (movingAround == nullptr) {
		movingAround = new ArriveBehavior(10, 10, 0.25f);
		owner->AddBehavior(movingAround);
		movingAround->SetListenerState(ListenerState::DISABLED);
	}
	else {
		if (movingAround->IsFinished() || movingAround->GetListenerState() == ListenerState::DISABLED) {
			// find some point that can be used
			float x = 0;
			float y = 0;

			if (ofRandom(0, 1) > 0.5f) {
				// try to go up or down
				y = ofRandom(-2.0f, 2.0f);
			}
			else {
				// try to go left or right
				x = ofRandom(-2.0f, 2.0f);
			}

			auto startPrec = owner->GetTransform().localPos;
			auto start = Vec2i(startPrec);
			auto endPrec = startPrec + ofVec2f(x, y);
			auto end = Vec2i(endPrec);
			
			// check if we can go at selected location
			vector<Vec2i> map = gameModel->GetMap()->FindPath(start, end, false, 5);

			if (map.size() > 0 && map.size() <= 2) {
				// go there 
				movingAround->Start();
				movingAround->SetListenerState(ListenerState::ACTIVE_ALL);
				// set destination
				owner->AddAttr(ATTR_STEERING_BEH_SEEK_DEST, ofVec2f(endPrec));
			}
		}
	}
}

void WorkerIdleState::OnStart() {
	owner->SetState(StrId(STATE_WORKER_IDLE));
}

void WorkerIdleState::OnFinish() {
	owner->ResetState(StrId(STATE_WORKER_IDLE));
}

void WorkerIdleState::Update(const uint64 delta, const uint64 absolute) {
	
	bool foundTask = false;
	// each nth frame find a task to do...
	if (CogGetFrameCounter() % 30 == 0) {
		foundTask = this->FindTaskToDo();
	}

	if (!foundTask) {
		MoveAround();
	}
	else {
		// remove moving around behavior
		if (movingAround != nullptr) {
			// stop movement
			movingAround->Finish();
			owner->GetAttr<Movement>(ATTR_MOVEMENT).Stop();
			owner->RemoveBehavior(movingAround, true);
			movingAround = nullptr;
		}
	}
}

// ========================== WORKER BRIDGE BUILD STATE ==============================

void WorkerBridgeBuildState::OnMessage(Msg& msg) {
	if (msg.GetAction() == StrId(ACT_TASK_ABORTED)) {
		TaskAbortEvent* msgTask = msg.GetData<TaskAbortEvent>();
		if (msgTask->taskToAbort == task) {
			if (buildGoal != nullptr) {
				COGLOGDEBUG("Hydroq", "BridgeBuildState: aborting process");
				buildGoal->Abort();
			}
		}
	}
}

void WorkerBridgeBuildState::OnStart() {

	owner->SetState(StrId(STATE_WORKER_BUILD));

	task->handlerNode = owner;
	task->isProcessing = true;

	// worker position
	auto workerPos = owner->GetTransform().localPos;
	// position where the bridge will stay
	auto position = task->taskNode->GetTransform().localPos;
	// safe position the worker can stay during the building
	auto targetSafePos = nodeToBuildfrom->pos;
	// precise position will be little bit close to the edge of the existing platform
	ofVec2f precisePosition = ofVec2f(targetSafePos.x + (position.x - targetSafePos.x) / 2.0f + 0.5f, 
		targetSafePos.y + (position.y - targetSafePos.y) / 2.0f + 0.5f);

	COGLOGDEBUG("Hydroq", "Going from [%.2f, %.2f] to [%.2f, %.2f]", workerPos.x, workerPos.y, precisePosition.x, precisePosition.y);

	auto composite = new GoalComposite(StrId(), false);
	composite->AddSubgoal(new GotoPositionGoal(gameModel, task, Vec2i(workerPos.x, workerPos.y), targetSafePos,workerPos, precisePosition));
	
	if (task->type == GameTaskType::BRIDGE_BUILD) {
		composite->AddSubgoal(new BuildBridgeGoal(gameModel, task));
	}
	else {
		composite->AddSubgoal(new DestroyBridgeGoal(gameModel, task));
	}
	this->buildGoal = composite;
	owner->AddBehavior(composite);
}

void WorkerBridgeBuildState::OnFinish() {
	owner->ResetState(StrId(STATE_WORKER_IDLE));
}


void WorkerBridgeBuildState::Update(const uint64 delta, const uint64 absolute) {
	if (buildGoal != nullptr && (buildGoal->GoalEnded())) {

		// change back to idle state
		auto stateToChange = GetParent()->FindLocalState(StrId(STATE_WORKER_IDLE));
		GetParent()->ChangeState(stateToChange);

		gameModel->RemoveGameTask(task);
	}
}



// ========================== WORKER ATTRACTOR FOLLOW STATE ==============================

void WorkerAttractorFollowState::OnMessage(Msg& msg) {
	if (msg.GetAction() == StrId(ACT_TASK_ABORTED)) {
		TaskAbortEvent* msgTask = msg.GetData<TaskAbortEvent>();
		if (msgTask->taskToAbort == task) {
			if (followGoal != nullptr) {
				COGLOGDEBUG("Hydroq", "WorkerAttractorFollowState: aborting process");
				followGoal->Abort();
			}
		}
	}
}

void WorkerAttractorFollowState::OnStart() {

	owner->SetState(StrId(STATE_WORKER_ATTRACTOR_FOLLOW));

	//task->handlerNode = owner;
	//task->isProcessing = true;

	// worker position
	auto workerPos = owner->GetTransform().localPos;
	// position where the bridge will stay
	auto position = this->nodeToFollow->pos;
	auto precisePosition = ofVec2f(position.x + ofRandom(0,1), position.y+ofRandom(0,1));

	COGLOGDEBUG("Hydroq", "Going from [%.2f, %.2f] to [%.2f, %.2f]", workerPos.x, workerPos.y, precisePosition.x, precisePosition.y);

	this->followGoal = new GotoPositionGoal(gameModel, task, Vec2i(workerPos.x, workerPos.y), precisePosition, workerPos, precisePosition);
	owner->AddBehavior(followGoal);
}

void WorkerAttractorFollowState::OnFinish() {
	owner->ResetState(StrId(STATE_WORKER_IDLE));
}


void WorkerAttractorFollowState::Update(const uint64 delta, const uint64 absolute) {
	if (followGoal != nullptr && (followGoal->GoalEnded())) {

		// change back to idle state
		auto stateToChange = GetParent()->FindLocalState(StrId(STATE_WORKER_IDLE));
		GetParent()->ChangeState(stateToChange);

		// this will be interesting
		task->RemoveReserverNode(owner->GetId());

		//gameModel->RemoveGameTask(task);
	}
}


