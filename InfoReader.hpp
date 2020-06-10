#pragma once

#define MAXMODCOUNT 10
#define MAXKEYCOUNT 128
#define MAXEFACECOUNT 64
#define MAXKPSNUMBERS 10

struct MODE_INFO {
	char *BackgroundImageName;
	char *CatBackgroundImageName;

	bool HasModel;
	char *CatModelPath;

	char *KeysImagePath;
	int KeysCount;
	char *KeysImageName[MAXKEYCOUNT];
	char *KeyUse[MAXKEYCOUNT];

	bool ModelHasLeftHandModel;
	char *ModelLeftHandModelPath;

	int ModelLeftHandCount;
	char *LeftHandImagePath;
	char *LeftHandUpImageName;
	char *LeftHandImageName[MAXKEYCOUNT];

	bool ModelHasRightHandModel;
	char *ModelRightHandModelPath;

	int ModelRightHandCount;
	char *RightHandImagePath;
	char *RightHandUpImageName;
	char *RightHandImageName[MAXKEYCOUNT];
};

struct  FACE_INFO{
	char *HotKey[MAXEFACECOUNT];
	int Facecount;
	char *FaceImages[MAXEFACECOUNT];
};

struct KPS_INFO {
	int KpsCount;
	char *KPSImages[MAXKPSNUMBERS];
};

class InfoReader {
public:
	InfoReader();

	~InfoReader();

	bool InitFromConfig(const char* filePath);

	bool InitFaceFromConfig(const char *filePath);

	bool InitKpsFromConfig(const char *filePath);

	int ModeCount;
	char *ModePath[MAXMODCOUNT];

	MODE_INFO  *_modeInfo;
	FACE_INFO *_faceInfo;
	KPS_INFO *_kpsInfo;

private:

	bool ModeFromConfig(const char* filePath,int _modeid);

};
