#pragma once

#include "HydroqGoals.h"
#include "HydroqGameModel.h"
#include "HydMap.h"
#include "SteeringBehavior.h"

using namespace Cog;

void GotoPositionGoal::Init() {
	auto model = GETCOMPONENT(HydroqGameModel);
	// find nearest neighbor that can be used
	auto map = model->GetMap();
	auto mapNode = map->GetNode((int)targetPosition.x, (int)targetPosition.y);
	auto start = owner->GetTransform().localPos;
	auto nodeToBuildfrom = mapNode->FindNeighborByType(MapNodeType::GROUND,Vec2i(start.x, start.y));

	if (nodeToBuildfrom != nullptr) {
		auto targetSafePos = nodeToBuildfrom->pos;

		MLOGDEBUG("Hydroq", "Going from [%d %d] to [%d %d]", (int)start.x, (int)start.y, (int)nodeToBuildfrom->pos.x, (int)nodeToBuildfrom->pos.y);

		vector<Vec2i> map = model->GetMap()->FindPath(Vec2i(start.x, start.y), Vec2i(targetSafePos.x, targetSafePos.y));

		ofVec2f precisePosition = ofVec2f(targetSafePos.x + (targetPosition.x - targetSafePos.x)/2.0f + 0.5f, targetSafePos.y + (targetPosition.y - targetSafePos.y)/2.0f + 0.5f);

		// the last point will be at the very border
		// skip first point, because we are already there
		Vec2i firstPoint = map.size() >= 2 ? map[1] : Vec2i(start.x, start.y);
		Path* pth = new Path(start, ofVec2f(firstPoint.x + 0.5f, firstPoint.y + 0.5f));
		
		for (int i = 2; i < map.size(); i++) {
			auto point = map[i];
			MLOGDEBUG("Hydroq", "  Map:: [%d, %d]", point.x, point.y);
			pth->AddSegment(ofVec2f(point.x + 0.5f, point.y + 0.5f));

		}

		// add the last point
		pth->AddSegment(precisePosition);

		innerBehavior = new FollowBehavior(pth, 60, 0.25f, 0.1f);
		owner->AddBehavior(innerBehavior);
	}
	else {
		this->SetGoalState(GoalState::COMPLETED);
	}
}

void BuildBridgeGoal::Update(const uint64 delta, const uint64 absolute) {
	if (goalStarted == 0) {
		goalStarted = absolute;
	}
	else {
		if ((absolute - goalStarted) > 3500) {

			CogLogDebug("Hydroq", "Building bridge");
			
			// build bridge each 3.5s
			auto model = GETCOMPONENT(HydroqGameModel);
			auto position = Vec2i(task->taskNode->GetTransform().localPos);
			model->BuildPlatform(position);
			task->isEnded = true;
			this->Complete();
			Finish();
		}
	}
}