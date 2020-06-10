/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "EventManager.hpp"
#include <math.h>
#include <Windows.h>

EventManager::EventManager()
    : _startY(0.0f)
    , _startX(0.0f)
    , _lastX(0.0f)
    , _lastY(0.0f)
    , _lastX1(0.0f)
    , _lastY1(0.0f)
    , _lastX2(0.0f)
    , _lastY2(0.0f)
    , _deltaX(0.0f)
    , _deltaY(0.0f)
    , _scale(1.0f)
    , _relativemouseX(0)
    , _relativemouseY(0)
    ,_leftButton(false)
    ,_rightButton(false)
{
	for (int i = 0; i < KEYAMOUT; i++)
		_keyEvent[i].KeyBoardSignal = false;
	
}

EventManager::~EventManager() {

}

void EventManager::MouseEventBegan(float deviceX, float deviceY)
{
    _lastX = deviceX;
    _lastY = deviceY;
    _startX = deviceX;
    _startY = deviceY;
}

void EventManager::MouseEventMoved(float deviceX, float deviceY)
{
    _lastX = deviceX;
    _lastY = deviceY;
}

void EventManager::MouseEventMoved(float deviceX1, float deviceY1,
				   float deviceX2, float deviceY2)
{
    float distance = CalculateDistance(deviceX1, deviceY1, deviceX2, deviceY2);
    float centerX = (deviceX1 + deviceX2) * 0.5f;
    float centerY = (deviceY1 + deviceY2) * 0.5f;

    _lastX = centerX;
    _lastY = centerY;
    _lastX1 = deviceX1;
    _lastY1 = deviceY1;
    _lastX2 = deviceX2;
    _lastY2 = deviceY2;

}

void EventManager::MouseEventMoved(int width, int height, float deviceX,
				   float deviceY)
{
	if (deviceX - _startX > 0) {
		if (deviceX - _startX > width / 2) {
			_startX += deviceX - _startX - width / 2;	
		} 
		_lastX = deviceX;
	} else {
		if (abs(deviceX - _startX) > width / 2) {
			_startX -= abs(deviceX - _startX) - width / 2;
		}	
		_lastX = deviceX;
	}

	if (deviceY - _startY > 0) {
		if (deviceY - _startY > height / 2) {
			_startY += deviceY - _startY - height / 2;
		}
		_lastY = deviceY;
	} else {
		if (abs(deviceY - _startY) > height / 2) {
			_startY -= abs(deviceY - _startY) - height / 2;
		}
		_lastY = deviceY;
	}
}

void EventManager::KeyEventDown(int key)
{
	_keyEvent[key].KeyBoardSignal = true;
}

void EventManager::KeyEventUp(int key) {
	_keyEvent[key].KeyBoardSignal = false;
}

void EventManager::LeftButtonDown() {
	_leftButton = true;
}

void EventManager::LeftButtonUp() {
	_leftButton = false;
}

void EventManager::RightButtonDown() {
	_rightButton = true;
}

void EventManager::RightButtonUp() {
	_rightButton = false;
}

void EventManager::SetRelativeMouse(int _rx, int _ry) {
	_relativemouseX += _rx;
	_relativemouseY += _ry;
}

void EventManager::GetRelativeMouse(int &_rx, int &_ry) {
	_rx = _relativemouseX;
	_ry = _relativemouseY;
}

void EventManager::GetCurrentMousePosition(int &_x, int &_y) {
	POINT p;
	GetCursorPos(&p);
	_x = p.x;
	_y = p.y;
}

float EventManager::GetFlickDistance() const
{
    return CalculateDistance(_startX, _startY, _lastX, _lastY);
}

float EventManager::CalculateDistance(float x1, float y1, float x2, float y2) const
{
    return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float EventManager::CalculateMovingAmount(float v1, float v2)
{
    if ((v1 > 0.0f) != (v2 > 0.0f))
    {
        return 0.0f;
    }

    float sign = v1 > 0.0f ? 1.0f : -1.0f;
    float absoluteValue1 = fabsf(v1);
    float absoluteValue2 = fabsf(v2);
    return sign * ((absoluteValue1 < absoluteValue2) ? absoluteValue1 : absoluteValue2);
}
