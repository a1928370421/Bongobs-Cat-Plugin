/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#pragma once

class VtuberFrameWork {
public:
	static void InitVtuber(int id);

	static void ReanderVtuber(int targetid, char *data, int bufferWidth,
				  int bufferheight);

	static void UinitVtuber(int id);

	static void UpData(int id,double _x, double _y, int width, int height,
			   double scale,double delayTime, bool randomMotion,
			   bool breath,bool eyeBlink, const char *ModelName,
			   bool track, const char *modelPath, bool _live2d,
			   bool relative_mouse, bool _isMouseHorizontalFlip,
			   bool _isMouseVerticalFlip, bool _isUsemask);

	static const char **GetModeDefine(int &_size);

	static int GetWidth(int id);

	static int GetHeight(int id);
};


