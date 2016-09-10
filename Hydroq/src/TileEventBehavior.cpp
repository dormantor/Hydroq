#include "TileEventBehavior.h"
#include "ComponentStorage.h"
#include "ResourceCache.h"

void TileEventBehavior::OnInit() {
	SubscribeForMessages(ACT_OBJECT_HIT_STARTED, ACT_OBJECT_HIT_ENDED);
	cache = GETCOMPONENT(ResourceCache);
	gameView = owner->GetBehavior<GameView>();
}


void TileEventBehavior::OnMessage(Msg& msg) {
	if (msg.GetContextNode()->GetId() == owner->GetId() 
		|| msg.GetContextNode()->GetId() == owner->GetParent()->GetId()) {

		auto touch = msg.GetData<InputEvent>();

		if (msg.HasAction(ACT_OBJECT_HIT_STARTED)) {
			hitPos = touch->input->position;
		}
		else if (msg.HasAction(ACT_OBJECT_HIT_ENDED)) {

			// user clicked on game tile -> transform mouse position to the game model coordinate
			if (mapWidth == 0 || mapHeight == 0) {
				auto gameModel = owner->GetBehavior<GameModel>();
				mapWidth = gameModel->GetMap()->GetWidth();
				mapHeight = gameModel->GetMap()->GetHeight();
			}

			ofVec2f endPos = touch->input->position;

			if (touch->input->IsProcessed()) return;

			bool isPointerOver = isPointerOver = endPos.distance(hitPos) < CogGetScreenWidth() / TOUCHMOVE_TOLERANCE;

			if (isPointerOver) {

				auto mesh = owner->GetMesh();

				touch->input->SetIsProcessed(true);

				// get pressed tile
				float shapeWidth = mesh->GetWidth()*owner->GetTransform().absScale.x;
				float shapeHeight = mesh->GetHeight()*owner->GetTransform().absScale.y;
				float absPosX = owner->GetTransform().absPos.x;
				float absPosY = owner->GetTransform().absPos.y;

				float distX = (endPos.x - absPosX);
				float distY = (endPos.y - absPosY);

				// get tile indices
				int brickX = (int)((distX / shapeWidth)*mapWidth);
				int brickY = (int)((distY / shapeHeight)*mapHeight);

				// suppose that the collection follows positional ordering
				auto pressedTileSprite = gameView->GetStaticSprites()->GetSprites()[brickY*mapWidth + brickX];

				// send message that player has clicked on a tile
				SendMessage(ACT_BRICK_CLICKED, spt<SpriteClickEvent>(new SpriteClickEvent(brickX, brickY, pressedTileSprite)));
			}
		}
	}
}
