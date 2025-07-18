#pragma once

#include "application_header.h"


struct ui;
struct pascal_object;
struct ui_static;
struct ui_button;

struct application : public application_flags {

    application();

	bool init();

	void run();
    
	void clear();
    
	bool update();

    void onlostdevice();
    void onresetdevice();

	ui * m_ui;

    pascal_object* m_pascal;

	/* member control handles ******/
	ui_static * m_light_control;
	ui_static * m_rotation_control;
	ui_static * m_animation_control;
	ui_button * m_vsync_button;
	ui_button * m_fullscreen_button;
	/*******************************/

	/*camera***************/
    _mat4      m_view;
    _mat4      m_projection;
	/**********************/

	/*cursor***************/
	float      m_x_cursor_pos;
    float      m_y_cursor_pos;
	/**********************/

	/* ui control ids****************/
	uint32_t m_animation_increment_id;
	uint32_t m_animation_decrement_id;
	uint32_t m_rotation_increment_id;
	uint32_t m_rotation_decrement_id;
	uint32_t m_light_increment_id;
	uint32_t m_light_decrement_id;
	uint32_t m_reset_id;
	uint32_t m_vsync_id;
	uint32_t m_fullscreen_id;
	/*******************************/

	/* animation keyframe updated in window class (winproc)*/
    int        m_keyframe;
	/**********************/

	float m_light_value;
	float m_rotation_value;

	/*timer***************/
	int        m_framespersecond;
	float      m_last_frame_seconds;
	float      m_last_frame_milliseconds;
	/**********************/

	/*application global static variables *********************/
    static _vec3           _up_axis;
    static application *  _instance;
    static HINSTANCE      _win32_instance;
	/**********************************************************/

	/* global object array */
	static _array<application_object*> object_array;
	/**********************************************************/

	/***generates a _mesh from a ._mesh resource file **************/
	static bool loadmeshfile(const LPVOID data,_mesh* submeshes,bool bones=true);
	/**********************************************************/
	
	static LPVOID  s_lpdata;
	static HGLOBAL s_hglobal;
	static HRSRC   s_hresource;
	static LPVOID  getresourcedata(int id);
	static void    freeresourcedata();

};