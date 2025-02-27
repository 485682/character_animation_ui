
#include "application.h"

#include "d3d_window.h"
#include "d3d_manager.h"

#include "ui.h"

#include "ui_static.h"
#include "ui_line.h"
#include "ui_text.h"
#include "ui_button.h"
#include "ui_image.h"

#include "pascal.h"

_vec3         application::_up_axis        = _vec3(0.0f,1.0f,0.0f);
application*  application::_instance       = NULL;
HINSTANCE     application::_win32_instance = NULL;

_array< application_object *> application::object_array;

ui_static * fps_control  = NULL;
ui_static * mspf_control = NULL;

application::application(){


	m_ui = NULL;
	m_pascal = NULL;

	m_animation_control = NULL;
	m_vsync_button      = NULL;
	m_fullscreen_button = NULL;
	m_light_control     = NULL;
	m_rotation_control  = NULL;
	m_animation_control = NULL;
	m_vsync_button      = NULL;
	m_fullscreen_button = NULL;


	m_light_value    = 1.0f;
	m_rotation_value = 0.0f;

	m_keyframe = 0;
	m_framespersecond = 0;
	m_last_frame_seconds = 0;
	m_last_frame_milliseconds = 0;

}

bool application::init(){

	/*win32 HINSTANCE */
	_win32_instance = GetModuleHandle(NULL);

	_window         = new d3d_window();
	_api_manager    = new d3d_manager();

	if(!_window->init())      { return false;}
	if(!_api_manager->init()) { return false; }

	/* when this flag is removed, the application exits */
	addflags(application_running); 

	/*skeletan animation object (character)*/
	m_pascal = new pascal_object();
	if(!m_pascal->init()) { return false; }
	m_pascal->setbindpose();
	/***************************************/


	
	m_ui = new ui();

	/* frames per second display *****/
	fps_control = new ui_static();
	fps_control->m_x      = 25;
	fps_control->m_y      = 5;
	fps_control->m_width  = 225;
	fps_control->m_height = 20;
	fps_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	fps_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(fps_control);
	/*********************************/

	/* millisecond per frame display */
	mspf_control = new ui_static();
	mspf_control->m_x      = 25;
	mspf_control->m_y      = 25;
	mspf_control->m_width  = 225;
	mspf_control->m_height = 20;
	mspf_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	mspf_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(mspf_control);
	/*********************************/

	/* displays current animation ****/
	m_animation_control = new ui_static();
	m_animation_control->m_x      = 25;
	m_animation_control->m_y      = 45;
	m_animation_control->m_width  = 75;
	m_animation_control->m_height = 20;
	m_animation_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	m_animation_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(m_animation_control);
	_application->m_animation_control->settext("walk");
	/*********************************/

	/* image as button, animation switch ****/
	ui_image * animation_left_arrow = new ui_image();
	animation_left_arrow->m_x      = 100;
	animation_left_arrow->m_y      = 45;
	animation_left_arrow->m_width  = 20;
	animation_left_arrow->m_height = 20;
	animation_left_arrow->setimage("left_arrow.bmp");
	m_ui->addcontrol(animation_left_arrow);
	m_animation_decrement_id = animation_left_arrow->m_id;
	/****************************************/

	/* image as button, animation switch ****/
	ui_image * animation_right_arrow = new ui_image();
	animation_right_arrow->m_x      = 125;
	animation_right_arrow->m_y      = 45;
	animation_right_arrow->m_width  = 20;
	animation_right_arrow->m_height = 20;
	animation_right_arrow->setimage("right_arrow.bmp");
	m_ui->addcontrol(animation_right_arrow);
	m_animation_increment_id = animation_right_arrow->m_id;
	/****************************************/


	/* animation label****************** ****/
	ui_static * animation_label = new ui_static();
	animation_label->m_x      = 145;
	animation_label->m_y      = 45;
	animation_label->m_width  = 100;
	animation_label->m_height = 20;
	animation_label->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	animation_label->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(animation_label);
	animation_label->settext("animation");
	/****************************************/

	/* displays degrees of rotation ********/
	m_rotation_control = new ui_static();
	m_rotation_control->m_x      = 25;
	m_rotation_control->m_y      = 65;
	m_rotation_control->m_width  = 75;
	m_rotation_control->m_height = 20;
	m_rotation_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	m_rotation_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(m_rotation_control);
	_application->m_rotation_control->settext("0");
	/****************************************/

	/* image as button, degrees switch ****/
	ui_image * rotate_left_arrow = new ui_image();
	rotate_left_arrow->m_x      = 100;
	rotate_left_arrow->m_y      = 65;
	rotate_left_arrow->m_width  = 20;
	rotate_left_arrow->m_height = 20;
	rotate_left_arrow->setimage("left_arrow.bmp");
	m_ui->addcontrol(rotate_left_arrow);
	m_rotation_decrement_id = rotate_left_arrow->m_id;
	/****************************************/

	/* image as button, degrees switch ****/
	ui_image * rotate_right_arrow = new ui_image();
	rotate_right_arrow->m_x      = 125;
	rotate_right_arrow->m_y      = 65;
	rotate_right_arrow->m_width  = 20;
	rotate_right_arrow->m_height = 20;
	rotate_right_arrow->setimage("right_arrow.bmp");
	m_ui->addcontrol(rotate_right_arrow);
	m_rotation_increment_id = rotate_right_arrow->m_id;
	/****************************************/

	/* rotate label *************************/
	ui_static * rotate_label = new ui_static();
	rotate_label->m_x      = 145;
	rotate_label->m_y      = 65;
	rotate_label->m_width  = 100;
	rotate_label->m_height = 20;
	rotate_label->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	rotate_label->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(rotate_label);
	rotate_label->settext("rotate");
	/****************************************/

	/* displays light intensity *************/
	m_light_control = new ui_static();
	m_light_control->m_x      = 25;
	m_light_control->m_y      = 85;
	m_light_control->m_width  = 75;
	m_light_control->m_height = 20;
	m_light_control->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	m_light_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(m_light_control);
	_application->m_light_control->settext("100");
	/****************************************/

	/* image as button, light intensity switch */
	ui_image * light_left_arrow = new ui_image();
	light_left_arrow->m_x      = 100;
	light_left_arrow->m_y      = 85;
	light_left_arrow->m_width  = 20;
	light_left_arrow->m_height = 20;
	light_left_arrow->setimage("left_arrow.bmp");
	m_ui->addcontrol(light_left_arrow);
	m_light_decrement_id = light_left_arrow->m_id;
	/*******************************************/

	/* image as button, light intensity switch */
	ui_image * light_right_arrow = new ui_image();
	light_right_arrow->m_x      = 125;
	light_right_arrow->m_y      = 85;
	light_right_arrow->m_width  = 20;
	light_right_arrow->m_height = 20;
	light_right_arrow->setimage("right_arrow.bmp");
	m_ui->addcontrol(light_right_arrow);
	m_light_increment_id = light_right_arrow->m_id;
	/*******************************************/

	/* light intensity label*** *************/
	ui_static * light_label = new ui_static();
	light_label->m_x      = 145;
	light_label->m_y      = 85;
	light_label->m_width  = 120;
	light_label->m_height = 20;
	light_label->m_background_color = _vec4(0.0f,0.0f,0.0f,0.0f);
	light_label->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(light_label);
	light_label->settext("light intensity");
	/****************************************/



	/** line edit, no events attached ******************************/
	ui_line * line_control = new ui_line();
	line_control->m_x      = 25;
	line_control->m_y      = 110;
	line_control->m_width  = 250;
	line_control->m_height = 20;
	line_control->m_background_color = _vec4(0.0f,0.0f,1.0f,0.04f);
	line_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(line_control);
	line_control->settext("line edit");
	/**************************************************************/

	/** text edit, no events attached ******************************/
	ui_text * text_control = new ui_text();
	text_control->m_x      = 25;
	text_control->m_y      = 135;
	text_control->m_width  = 250;
	text_control->m_height = 120;
	text_control->m_background_color = _vec4(0.0f,0.0f,1.0f,0.04f);
	text_control->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_ui->addcontrol(text_control);		
	text_control->settext("text edit");
	/**************************************************************/

	/*reset*******************************************************/
	ui_button * reset_button = new ui_button();
	reset_button->m_x      = 25;
	reset_button->m_y      = 260;
	reset_button->m_width  = 145;
	reset_button->m_height = 40;
	reset_button->m_background_color = _vec4(0.0f,0.0f,0.0f,0.01f);
	reset_button->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	reset_button->m_alt_color = _vec4(0.0f,0.0f,0.4f,0.2f);
	m_ui->addcontrol(reset_button);
	reset_button->settext("reset");
	m_reset_id = reset_button->m_id;
	/**************************************************************/

	/*vsync*******************************************************/
	m_vsync_button = new ui_button();
	m_vsync_button->m_x      = 25;
	m_vsync_button->m_y      = 300;
	m_vsync_button->m_width  = 145;
	m_vsync_button->m_height = 40;
	m_vsync_button->m_background_color = _vec4(0.0f,0.0f,0.0f,0.01f);
	m_vsync_button->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_vsync_button->m_alt_color = _vec4(0.0f,0.0f,0.4f,0.2f);
	m_ui->addcontrol(m_vsync_button);
	m_vsync_button->settext("vsync");
	m_vsync_id = m_vsync_button->m_id;
	/**************************************************************/

	/* fullscreen *************************************************/
	m_fullscreen_button = new ui_button();
	m_fullscreen_button->m_x      = 25;
	m_fullscreen_button->m_y      = 340;
	m_fullscreen_button->m_width  = 145;
	m_fullscreen_button->m_height = 40;
	m_fullscreen_button->m_background_color = _vec4(0.0f,0.0f,0.0f,0.01f);
	m_fullscreen_button->m_foreground_color = _vec4(0.0f,0.0f,1.0f,1.0f);
	m_fullscreen_button->m_alt_color = _vec4(0.0f,0.0f,0.4f,0.2f);
	m_ui->addcontrol(m_fullscreen_button);
	m_fullscreen_button->settext("fullscreen");
	m_fullscreen_id = m_fullscreen_button->m_id;
	/**************************************************************/

	if(!m_ui->init()){ return false; }

	return true;
}

void application::run(){

	onresetdevice();

	m_keyframe =1; // walk animation
	_application->m_pascal->keyframe(0,5);
	_application->m_pascal->m_animation_length =0.14f;

	_window->update();


	/* initialize  timer */
	int64_t tickspersecond = 0;
	int64_t previous_timestamp = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&tickspersecond);
	float secsPerCnt = 1.0f / (float)tickspersecond;
	QueryPerformanceCounter((LARGE_INTEGER*)&previous_timestamp);
	/********************************/

	int frames = 0;
	float second =0.0f,coursor_second =0.0f;
	while( testflags(application_running) ){

		/**d3d device test.  error exits application**/
		removeflags(application_lostdev);
		HRESULT hr = _api_manager->m_d3ddevice->TestCooperativeLevel();

		if( hr == D3DERR_DEVICELOST ) {  addflags( application_lostdev );  }
		else if( hr == D3DERR_DRIVERINTERNALERROR ) {
			addflags( application_deverror );
			application_error("d3d device error");
		}else if( hr == D3DERR_DEVICENOTRESET ){
			_api_manager->reset();
			addflags( application_lostdev );
		}
		/*********************************************/

		/*timer****************************************************/
		int64_t currenttimestamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currenttimestamp);
		m_last_frame_seconds = float(currenttimestamp - previous_timestamp) * secsPerCnt;
		m_last_frame_milliseconds = m_last_frame_seconds*1000.0f;
		previous_timestamp = currenttimestamp;
		/**********************************************************/

		coursor_second += m_last_frame_seconds;
		second += m_last_frame_seconds;
		frames++;
		/*stat display **************************************************/
		if(second >=1.0f){
			_string stats =_string("fps : ")+ _utility::inttostring(frames);
			fps_control->settext(" ");
			stats =_string("mspf: ")+ _utility::floattostring(m_last_frame_milliseconds,true);
			mspf_control->settext(" ");
			second = 0.0f;
			frames = 0;
		}
		/**************************************************************************/

		/* coursor display timer **************************************************/
		if(coursor_second >= 0.5f){
			if(testflags(application_coursor_on)){ removeflags(application_coursor_on); }
			else{ addflags(application_coursor_on); }
			coursor_second = 0.0f;
		}
		/**************************************************************************/

		if( !(testflags(application_lostdev))  && !(testflags(application_deverror)) &&!(testflags(application_paused))  ) {
			/* application update (render) ******************************************/
			update();

			application_error_hr(_api_manager->m_d3ddevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0));
			application_error_hr(_api_manager->m_d3ddevice->BeginScene());

			m_ui->update(); 
			m_pascal->update();

			application_error_hr(_api_manager->m_d3ddevice->EndScene());
			application_error_hr(_api_manager->m_d3ddevice->Present(0, 0, 0, 0));
			/************************************************************************/
		}

		if( testflags(application_deverror) ) { removeflags(application_running); }
		else { _window->update(); }/* winpoc (input) */
	}

	/* deallocate .. exiting */
	m_ui->clear();
	m_pascal->clear();
	clear();
	/**********************/
}

void application::clear(){
	if(_api_manager ) { _api_manager->clear();}
}

bool application::update(){


	//*view matrix**************************************************/
	m_view = _lookatrh( _vec3(-2.0f,5.0f,12.0f) , _vec3(0.0f,4.0f,0.0f) , _up_axis);
	//***************************************************/

	//*mouse pointer update*************************************/
	POINT cursor_position;
	if (GetCursorPos(&cursor_position)) {
		m_x_cursor_pos = float(cursor_position.x);
		m_y_cursor_pos = float(cursor_position.y);
	}else{ application_error("cursor pos");}
	//***************************************************/

	return true;
}

void application::onlostdevice() {
	application_error_hr(_fx->OnLostDevice());
	m_ui->onlostdevice();
}

void application::onresetdevice() {
	application_error_hr(_fx->OnResetDevice());

	/*resize causes reset, so update projection matrix*/
	float w = (float)_api_manager->m_d3dpp.BackBufferWidth;
	float h = (float)_api_manager->m_d3dpp.BackBufferHeight;
	m_projection = _perspectivefovrh(D3DX_PI * 0.25f, w,h, 1.0f, 1000.0f);
	/***************************************************/
	m_ui->onresetdevice();
}

bool application::readrtmeshfile(const char* path,_mesh * mesh){

	FILE * file = NULL;
	fopen_s(&file,path,"rb");
	if(!file){ application_throw("readrtmeshfile"); }

	char  header_[7];
	application_zero(header_,7);
	/* file marked with 6 bytes ascii string _mesh_ */
	if( fread(header_,1,6,file) != 6 ){ application_throw("fread"); }
	if(!application_scm(header_,"_mesh_")) { application_throw("not _mesh_ file"); }

	uint16_t submesh_count_ = 0;
	/* read 2 byte unsinged int (submesh count )*/
	if( fread((&submesh_count_),2,1,file) != 1 ){ application_throw("fread"); }
	printf("smcount : %u \n",submesh_count_);

	for(uint32_t i=0;i<submesh_count_;i++){

		_submesh submesh_;

		uint32_t index_count_ = 0;
		/* read 4 byte unsinged int (vertex indicies count ) */
		if( fread((&index_count_),4,1,file) != 1 ){ application_throw("fread"); }

		int32_t * indices = new int32_t[index_count_];
		/* read indices 4 bytes each  */
		if( fread(indices,4,index_count_,file) != index_count_ ){ application_throw("fread"); }
		for(uint32_t ii=0;ii<index_count_;ii++){
			submesh_.m_indices.pushback(indices[ii],true);
		}

		uint32_t vertex_count_ = 0;
		/* read 4 byte unsinged int (vertex count ) */
		if( fread((&vertex_count_),4,1,file) != 1 ){ application_throw("fread"); }

		_vertex * vertices = new _vertex[vertex_count_];
		/* read vertices  */
		if( fread(vertices,sizeof(_vertex),vertex_count_,file) != vertex_count_ ){ application_throw("fread"); }
		for(uint32_t ii=0;ii<vertex_count_;ii++){
			submesh_.m_vertices.pushback(vertices[ii],true);
		}
		mesh->m_submeshes.pushback(submesh_,true);
	}
	uint16_t bone_count = 0;
	/* read 2 byte unsinged int (bone transform count ) */
	if( fread((&bone_count),2,1,file) != 1 ){ application_throw("fread"); }

	if(bone_count){

		printf("bone_count: %i \n",bone_count);
		mesh->m_bones.allocate(bone_count);
		/* read bone transforms  */
		if( fread( (&(mesh->m_bones[0])) ,sizeof(_mat4 ),bone_count,file) != bone_count ){ application_throw("fread"); }

		uint16_t keyframe_count = 0;
		/* read 2 byte unsinged int (animation keyframe count ) */
		if( fread( (&keyframe_count) ,2,1,file) != 1 ){ application_throw("fread"); }
		if( keyframe_count){
			printf("keyframes: %i \n",keyframe_count);
			/* read animation bone transforms  */
			for(uint32_t ii=0;ii<keyframe_count;ii++){
				_matrix_array keyframe;
				keyframe.allocate(bone_count);
				if( fread( &(keyframe[0]) ,sizeof(_mat4 ),bone_count,file) != bone_count ){ application_throw("fread"); }
				mesh->m_keyframes.pushback(keyframe);
			}
		}
	}
	fclose(file);
	return true;
}

