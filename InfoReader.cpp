#include "InfoReader.hpp"
#include "json.h"
#include"Pal.hpp"
#include "Define.hpp"
#include<string>
#include <vector>
#include"CubismSdk/Framework/src/Type/CubismBasicType.hpp"

using namespace std;
using namespace Json;
using namespace Live2D::Cubism::Framework;


namespace {
csmByte *CreateBuffer(const csmChar *path, csmSizeInt *size)
{
	return Pal::LoadFileAsBytes(path, size);
}

void DeleteBuffer(csmByte *buffer, const csmChar *path = "")
{
	Pal::ReleaseBytes(buffer);
}

char *CreateParam(string _param) {
	char * buf = new char[_param.size()+1];
	buf[_param.size()] = 0x00;
	memcpy(buf, _param.c_str(), _param.size());
	return buf;
}

}

InfoReader::InfoReader() : ModeCount(0)
{
	_modeInfo = new MODE_INFO[MAXMODCOUNT];
	_faceInfo = new FACE_INFO[1];
	_kpsInfo = new KPS_INFO[1];

}

InfoReader::~InfoReader() {

	delete _modeInfo;
	delete _faceInfo;
	delete _kpsInfo;

	for (int i = 0; i < ModeCount; i++) {
		delete ModePath[i];
	}

	ModeCount = 0;
}

bool InfoReader::InitFromConfig(const char *filePath)
{
	string _conf = string("config.json");
	string _filePath = string(filePath);
	string _fileConf = _filePath + _conf;

	csmSizeInt size;
	csmByte *buffer = CreateBuffer(_fileConf.c_str(), &size);

	JSONCPP_STRING err;
	Value root;
	CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse((char*)buffer, (char*)buffer + size, &root, &err)) {
		DeleteBuffer(buffer, _filePath.c_str());
		return false;
	}

	int index = 0;
	auto _modelPath = root["ModelPath"];
	for (auto entry = _modelPath.begin(); entry != _modelPath.end();entry++) {
		string _modeName = entry->asCString();
		string modePath = _filePath + _modeName;
		
		
		if (ModeFromConfig(modePath.c_str(), index)) {
			ModePath[index] = CreateParam(_modeName);
			index++;
		}		
	}
	ModeCount = index;

	DeleteBuffer(buffer, _filePath.c_str());
	return true;
}

bool InfoReader::ModeFromConfig(const char *filePath, int _modeid)
{

	string _conf = string("/config.json");
	string _filePath = string(filePath) + _conf;

	csmSizeInt size;
	csmByte *buffer = CreateBuffer(_filePath.c_str(), &size);

	JSONCPP_STRING err;
	Value root;
	CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse((char *)buffer, (char *)buffer + size, &root,
			   &err)) {
		DeleteBuffer(buffer, _filePath.c_str());
		return false;
	}

	string _BackImageName = root["BackgroundImageName"].asCString();
	string _CatBackgroundImageName = root["CatBackgroundImageName"].asCString();
	_modeInfo[_modeid].BackgroundImageName = CreateParam(_BackImageName);
	_modeInfo[_modeid].CatBackgroundImageName = CreateParam(_CatBackgroundImageName);

	bool _HasModel = root["HasModel"].asBool();
	string _CatModelPath = root["CatModelPath"].asCString();
	_modeInfo[_modeid].HasModel = _HasModel;
	_modeInfo[_modeid].CatModelPath = CreateParam(_CatModelPath);

	string _KeysImagePath = root["KeysImagePath"].asCString();
	_modeInfo[_modeid].KeysImagePath = CreateParam(_KeysImagePath);
	

	int index = 0;
	auto _keysImageName = root["KeysImageName"];
	for (auto entry = _keysImageName.begin(); entry != _keysImageName.end();entry++) {

		string _keys_Name = entry->asCString();		
		_modeInfo[_modeid].KeysImageName[index] =CreateParam(_keys_Name);
		index++;
	}
	
	int kindex = 0;
	auto _keyUse = root["KeyUse"];
	for (auto entry = _keyUse.begin(); entry != _keyUse.end();entry++) {
		string _key_Use = entry->asCString();
		_modeInfo[_modeid].KeyUse[kindex] = CreateParam(_key_Use);	
		kindex++;
	}
	if (index>kindex) {
		_modeInfo[_modeid].KeysCount = kindex;

	} else {
		_modeInfo[_modeid].KeysCount = index;
	}
	
	
	bool _ModelHasLeftHandModel = root["ModelHasLeftHandModel"].asBool();
	string _ModelLeftHandModelPath =root["ModelLeftHandModelPath"].asCString();
	_modeInfo[_modeid].ModelHasLeftHandModel = _ModelHasLeftHandModel;
	_modeInfo[_modeid].ModelLeftHandModelPath = CreateParam(_ModelLeftHandModelPath);

	
	string _LeftHandImagePath = root["LeftHandImagePath"].asCString();
	string _LeftHandUpImageName = root["LeftHandUpImageName"].asCString();
	
	_modeInfo[_modeid].LeftHandImagePath = CreateParam(_LeftHandImagePath);
	_modeInfo[_modeid].LeftHandUpImageName = CreateParam(_LeftHandUpImageName);

	index = 0;
	auto _leftHandImageName = root["LeftHandImageName"];
	for (auto entry = _leftHandImageName.begin();  entry != _leftHandImageName.end();entry++) {
		string _imageName = entry->asCString();
		_modeInfo[_modeid].LeftHandImageName[index] =CreateParam(_imageName);
		index++;
	}
	_modeInfo[_modeid].ModelLeftHandCount = index;

	bool _ModelHasRightHandModel = root["ModelHasRightHandModel"].asBool();
	string _ModelRightHandModelPath =root["ModelRightHandModelPath"].asCString();
	_modeInfo[_modeid].ModelHasRightHandModel = _ModelHasRightHandModel;
	_modeInfo[_modeid].ModelRightHandModelPath =CreateParam(_ModelRightHandModelPath);

	string _RightHandImagePath = root["RightHandImagePath"].asCString();
	string _RightHandUpImageName = root["RightHandUpImageName"].asCString();
	
	_modeInfo[_modeid].RightHandImagePath =CreateParam(_RightHandImagePath);
	_modeInfo[_modeid].RightHandUpImageName =CreateParam(_RightHandUpImageName);

	index = 0;
	auto _rightHandImageName = root["RightHandImageName"];
	for (auto entry = _rightHandImageName.begin(); entry != _rightHandImageName.end(); entry++) {
		string _imageName = entry->asCString();
		_modeInfo[_modeid].RightHandImageName[index] =CreateParam(_imageName);
		index++;
	}
	_modeInfo[_modeid].ModelRightHandCount = index;

	DeleteBuffer(buffer, _filePath.c_str());
	return true;
}

bool InfoReader::InitFaceFromConfig(const char *filePath)
{
	string _conf = string("config.json");
	string _filePath = string(filePath) + _conf;

	csmSizeInt size;
	csmByte *buffer = CreateBuffer(_filePath.c_str(), &size);

	JSONCPP_STRING err;
	Value root;
	CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse((char *)buffer, (char *)buffer + size, &root, &err)) {
		DeleteBuffer(buffer, _filePath.c_str());
		return false;
	}

	int findex = 0;
	auto _hotKey = root["HotKey"];
	for (auto entry = _hotKey.begin(); entry != _hotKey.end(); entry++) {
		_faceInfo[0].HotKey[findex] =CreateParam(entry->asCString());
		findex++;
	}

	int index = 0;
	auto _FaceImageName = root["FaceImageName"];
	for (auto entry = _FaceImageName.begin(); entry != _FaceImageName.end();entry++) {
		_faceInfo[0].FaceImages[index] =CreateParam(entry->asCString());
		index++;
	}
	if(findex>index)
		_faceInfo[0].Facecount = index;
	else 
		_faceInfo[0].Facecount = findex;

	DeleteBuffer(buffer, _filePath.c_str());
	return true;
}

bool InfoReader::InitKpsFromConfig(const char *filePath)
{
	string _conf = string("config.json");
	string _filePath = string(filePath) + _conf;

	csmSizeInt size;
	csmByte *buffer = CreateBuffer(_filePath.c_str(), &size);

	JSONCPP_STRING err;
	Value root;
	CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse((char *)buffer, (char *)buffer + size, &root,
			   &err)) {
		DeleteBuffer(buffer, _filePath.c_str());
		return false;
	}


	int index = 0;
	auto _kpsImageName = root["Numbers"];
	for (auto entry = _kpsImageName.begin(); entry != _kpsImageName.end();
	     entry++) {
		_kpsInfo[0].KPSImages[index] =CreateParam(entry->asCString());
		index++;
	}
	_kpsInfo[0].KpsCount = index;

	DeleteBuffer(buffer, _filePath.c_str());
	return true;
}
