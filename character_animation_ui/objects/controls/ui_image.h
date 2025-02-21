#pragma once
#include "ui.h"

struct ui_image : public ui_control {
	ui_image();
	~ui_image();

	virtual bool init();
	virtual void clear();
	virtual bool update();

	bool setimage(const char * filename );

	virtual void msgproc(UINT msg, WPARAM wParam, LPARAM lParam);

	IDirect3DTexture9*     m_image_texture;

	bool genbackgroundbuffer();
	_string m_string;
};