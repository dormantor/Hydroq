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
	auto nodeToBuildfrom = mapNode->FindNeighborByType(MapNodeType::GROUND);

	if (nodeToBuildfrom != nullptr) {
		CogLogDebug("Goal", "Going to point [%d,%d]", nodeToBuildfrom->pos.x, nodeToBuildfrom->pos.y);

		auto start = owner->GetTransform().localPos;
		auto targetSafePos = nodeToBuildfrom->pos;

		vector<Vec2i> map = model->GetMap()->FindPath(Vec2i(start.x, start.y), Vec2i(targetSafePos.x, targetSafePos.y));

		ofVec2f precisePosition = ofVec2f(targetSafePos.x + (targetPosition.x - targetSafePos.x)/2.0f, targetSafePos.y + (targetPosition.y - targetSafePos.y)/2.0f);

		// the last point will be at the very border
		Path* pth = new Path(start, precisePosition);

		for (auto point : map) {
			pth->AddSegment(point);
		}

		// add the last point
		pth->AddSegment(precisePosition);

		auto followBehavior = new FollowBehavior(pth, 60, 0.25f, 0.1f);
		owner->AddBehavior(followBehavior);
	}
	else {
		this->SetGoalState(GoalState::COMPLETED);
	}
}