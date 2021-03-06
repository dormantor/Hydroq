#include "TopPanel.h"
#include "TransformMath.h"


void TopPanel::OnInit() {
	blueBar = owner->GetScene()->FindNodeByTag("scorebar_blue");
	redBar = owner->GetScene()->FindNodeByTag("scorebar_red");

	gridWidth = owner->GetScene()->GetSettings().GetSettingValInt("transform", "grid_width");
	gridHeight = owner->GetScene()->GetSettings().GetSettingValInt("transform", "grid_height");

	auto gameBoard = owner->GetScene()->FindNodeByTag("gameboard");
	gameModel = gameBoard->GetBehavior<GameModel>();
}

void TopPanel::Update(const uint64 delta, const uint64 absolute) {

	// update each 10th frame
	if (CogGetFrameCounter() % 10 == 0) {

		// calculate number of red and blue units
		int blue = 0;
		int red = 0;

		for (auto& moving : gameModel->GetMovingObjects()) {
			auto faction = moving->GetAttr<Faction>(ATTR_FACTION);
			if (faction == Faction::RED) red++;
			else blue++;
		}

		int total = red + blue;
		float redPerc = red / ((float)total);
		float bluePerc = blue / ((float)total);

		if (redPerc == 0) redPerc = 0.01f; // prevent from scale compensation
		if (bluePerc == 0) bluePerc = 0.01f;

		// display the balance
		TransformMath math = TransformMath();
		TransformEnt blueEnt = TransformEnt(ofVec2f(43, 0.6f), 10, CalcType::GRID, ofVec2f(0, 0), ofVec2f(SCORE_WIDTH*bluePerc, 1.3f), CalcType::GRID);
		TransformEnt redEnt = TransformEnt(ofVec2f(57, 0.6f), 10, CalcType::GRID, ofVec2f(1, 0), ofVec2f(SCORE_WIDTH*redPerc, 1.3f), CalcType::GRID);

		math.SetTransform(blueBar, blueBar->GetParent(), blueEnt, gridWidth, gridHeight);
		math.SetTransform(redBar, redBar->GetParent(), redEnt, gridWidth, gridHeight);
	}
}