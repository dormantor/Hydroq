#pragma once

#include "HydroqNetMsg.h"

void HydroqNetMsg::LoadFromStream(NetReader* reader) {
	this->paramStrHash = StringHash(reader->ReadDWord());
	this->paramVec2i = Vec2i(reader->ReadDWord(), reader->ReadDWord());
	this->paramVec2f = ofVec2f(reader->ReadFloat(), reader->ReadFloat());
	this->paramInt = reader->ReadDWord();
	this->paramStr = reader->ReadString();
}

void HydroqNetMsg::SaveToStream(NetWriter* writer) {
	writer->WriteDWord(paramStrHash);
	writer->WriteDWord(paramVec2i.x);
	writer->WriteDWord(paramVec2i.y);
	writer->WriteFloat(paramVec2f.x);
	writer->WriteFloat(paramVec2f.y);
	writer->WriteDWord(paramInt);
	writer->WriteString(paramStr);
}
