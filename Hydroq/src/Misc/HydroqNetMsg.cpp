#include "HydroqNetMsg.h"

void HydroqServerInitMsg::LoadFromStream(NetReader* reader) {
	if (reader != nullptr) {
		this->faction = (Faction)reader->ReadByte();
		this->map = reader->ReadString();
	}
}

void HydroqServerInitMsg::SaveToStream(NetWriter* writer) {
	writer->WriteByte((tBYTE)faction);
	writer->WriteString(map);
}

spt<NetOutputMessage> HydroqServerInitMsg::CreateMessage() {
	auto outputMsg = spt<NetOutputMessage>(new NetOutputMessage(0));
	outputMsg->SetAction(NET_MULTIPLAYER_INIT);
	outputMsg->SetData(this);
	return outputMsg;
}

void HydroqCommandMsg::LoadFromStream(NetReader* reader) {
	this->eventType = (SyncEventType)reader->ReadByte();
	this->entityType = (EntityType)reader->ReadByte();

	float x = reader->ReadFloat();
	float y = reader->ReadFloat();
	this->position = ofVec2f(x, y);
	this->faction = (Faction)reader->ReadByte();
	this->identifier = reader->ReadDWord();

	int px = reader->ReadDWord();
	int py = reader->ReadDWord();
	this->ownerPosition = Vec2i(px, py);
}

void HydroqCommandMsg::SaveToStream(NetWriter* writer) {
	writer->WriteByte((tBYTE)eventType);
	writer->WriteByte((tBYTE)entityType);
	writer->WriteFloat(position.x);
	writer->WriteFloat(position.y);
	writer->WriteByte((tBYTE)faction);
	writer->WriteDWord(identifier);
	writer->WriteDWord(ownerPosition.x);
	writer->WriteDWord(ownerPosition.y);
}

spt<NetOutputMessage> HydroqCommandMsg::CreateMessage() {
	auto outputMsg = spt<NetOutputMessage>(new NetOutputMessage(0));
	outputMsg->SetAction(NET_MULTIPLAYER_COMMAND);
	outputMsg->SetData(this);
	return outputMsg;
}