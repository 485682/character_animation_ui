#include "ui_button.h"

#include "application.h"
#include "d3d_window.h"
#include "d3d_manager.h"

#include "pascal.h"
#include "ui_static.h"

ui_button::ui_button(){
	m_foreground_vertex_buffer = NULL;
	m_background_vertex_buffer = NULL;
	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

}

bool ui_button::genbackgroundbuffer(){

	application_releasecom(m_foreground_vertex_buffer);

	application_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6 * sizeof(_vec3),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_background_vertex_buffer,0));
	if(!m_background_vertex_buffer){ application_throw("vertex buffer"); }

	_vec3 * v = 0;
	application_throw_hr(m_background_vertex_buffer->Lock(0, 0, (void**)&v, 0));

	v[0].x = m_width + m_x;
	v[0].y = 0.0f + m_y;

	v[1].x = 0.0f + m_x;
	v[1].y = m_height + m_y;

	v[2].x = 0.0f + m_x;
	v[2].y = 0.0f + m_y;

	v[3].x = m_width + m_x;
	v[3].y = 0.0f + m_y;

	v[4].x = m_width + m_x;
	v[4].y = m_height + m_y;

	v[5].x = 0.0f + m_x;
	v[5].y = m_height + m_y;

	application_throw_hr(m_background_vertex_buffer->Unlock());
	return true;
}
bool ui_button::genforegroundbuffer(){

	if(m_string.m_count == 0 ){ return true;}

	application_releasecom(m_foreground_vertex_buffer);

	uint32_t vertex_count = 6*m_string.m_count;

	application_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(vertex_count * sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_foreground_vertex_buffer,0) );
	if(!m_foreground_vertex_buffer){ application_throw("vertex buffer"); }

	ui_vertex * v_ = 0;
	application_throw_hr(m_foreground_vertex_buffer->Lock(0, 0, (void**)&v_, 0));

	float font_width  = 8;
	float font_height = 16;
	float text_width  = font_width*m_string.m_count;

	bool  larger_text_width = m_width<text_width;
	float x = larger_text_width? m_x: ((m_width-text_width)/2)+m_x;

	bool  larger_text_height   = m_height<font_height;
	float y = larger_text_height? m_y: ((m_height-font_height)/2)+m_y;

	float count = larger_text_width?m_width/font_width:m_string.m_count;
	for(uint32_t i=0;i<count;i++){

		_vec2 character = ui::s_font_vectors[ m_string[i] ];

		_vec3 vertex_up_left    = _vec3( x+i*font_width           , y ,0);
		_vec3 vertex_up_right   = _vec3( x+i*font_width+font_width, y ,0);
		_vec3 vertex_down_right = _vec3( x+i*font_width+font_width, y+font_height ,0);
		_vec3 vertex_down_left  = _vec3( x+i*font_width           , y+font_height ,0);

		float font_with_part = (1.0f/16.0f)/16.0f;

		_vec2 uv_up_right   = _vec2( character.x +  font_with_part*font_width , character.y );
		_vec2 uv_up_left    = _vec2( character.x,character.y);
		_vec2 uv_down_right = _vec2( character.x +  font_with_part*font_width ,character.y+ (1.0f/16.0f) );
		_vec2 uv_down_left  = _vec2( character.x, character.y+ (1.0f/16.0f) );

		v_[(i*6)+0].m_vertex = vertex_up_right;
		v_[(i*6)+0].m_uv = uv_up_right;

		v_[(i*6)+1].m_vertex = vertex_down_left;
		v_[(i*6)+1].m_uv = uv_down_left;

		v_[(i*6)+2].m_vertex = vertex_up_left;
		v_[(i*6)+2].m_uv = uv_up_left;

		v_[(i*6)+3].m_vertex = vertex_up_right;
		v_[(i*6)+3].m_uv = uv_up_right;

		v_[(i*6)+4].m_vertex = vertex_down_right;
		v_[(i*6)+4].m_uv = uv_down_right;

		v_[(i*6)+5].m_vertex = vertex_down_left;
		v_[(i*6)+5].m_uv = uv_down_left;

	}
	application_throw_hr(m_foreground_vertex_buffer->Unlock());
	return true;
}
bool ui_button::init(){

	application_throw_hr( D3DXCreateTexture(_api_manager->m_d3ddevice,font_texture_size,font_texture_size,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_font_texture) );

	D3DLOCKED_RECT rect;
	application_throw_hr( m_font_texture->LockRect(0,&rect,0,D3DLOCK_DISCARD) );

	D3DLOCKED_RECT main_rect;
	application_throw_hr( _application->m_ui->m_main_font_texture->LockRect(0,&main_rect,0,D3DLOCK_DISCARD) );

	D3DCOLOR * pixel = NULL;
	uint8_t  * pixel_pointer = (uint8_t*)rect.pBits;

	D3DCOLOR * main_pixel = NULL;
	uint8_t  * main_pixel_pointer = (uint8_t*)main_rect.pBits;

	int count =font_texture_size;
	for( uint32_t i = 0; i < font_texture_size; i++) {
		pixel_pointer += rect.Pitch;
		pixel = (D3DCOLOR*)pixel_pointer;

		main_pixel_pointer += main_rect.Pitch;
		main_pixel = (D3DCOLOR*)main_pixel_pointer;

		for( int32_t j = 0; j < font_texture_size; j++ ){
			if(   (  (uint8_t)0xFF & ( main_pixel[j] >>24) )     >0  ){
				/*set all white values to the desired color*/
				pixel[j] = D3DCOLOR_ARGB(
					uint8_t(m_foreground_color.w*0xFF),
					uint8_t(m_foreground_color.x*0xFF),
					uint8_t(m_foreground_color.y*0xFF),
					uint8_t(m_foreground_color.z*0xFF) );
			}else{ pixel[j] = 0; }
		}
	}
	m_font_texture->UnlockRect(0);
	_application->m_ui->m_main_font_texture->UnlockRect(0);

	genbackgroundbuffer();
	genforegroundbuffer();
	return true;
}

void ui_button::clear(){
	application_releasecom(m_foreground_vertex_buffer);
	application_releasecom(m_background_vertex_buffer);
}

bool ui_button::update(){

	application_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_background));
	application_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_background_vertex_declaration ));
	application_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(_vec3)));

	_vec4 background_color = testflags(ui_mouse_over)?m_alt_color:m_background_color;


	application_throw_hr(_fx->SetValue(_api_manager->m_hcolor, (D3DXCOLOR*)&background_color, sizeof(D3DXCOLOR)));
	application_throw_hr(_fx->Begin(NULL, 0));
	application_throw_hr(_fx->BeginPass(0));
	application_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	application_throw_hr(_fx->EndPass());
	application_throw_hr(_fx->End());

	application_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_font_texture ));
	application_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
	application_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
	application_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_foreground_vertex_buffer, 0, sizeof(ui_vertex)));

	application_throw_hr(_fx->Begin(NULL, 0));
	application_throw_hr(_fx->BeginPass(0));
	application_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,m_string.m_count*2) );
	application_throw_hr(_fx->EndPass());
	application_throw_hr(_fx->End());

	return true;
}

void ui_button::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	if( (msg != WM_MOUSEMOVE)&&(msg != WM_LBUTTONDOWN) && (_application->m_ui->m_current_control != m_id) ){ return; }

	switch( msg )
	{
	case WM_LBUTTONDOWN :{
		if(intersection_test()){
		if( testflags(ui_mouse_over) ){ 
			_application->m_ui->m_current_control = m_id; 
			addflags(ui_left_button_down);

			/* 
			* all event code is placed in the classes main msg proc
			* a more practical way would be to derive the class leaving this function mostly empty
			*/

			/* reset image control */
			if( m_id == _application->m_reset_id ){
				_application->m_keyframe =1; // walk animation
				_application->m_pascal->keyframe(0,5);
				_application->m_pascal->m_animation_length =0.14f;
				_application->m_light_value=1.0f;
				_application->m_light_control->settext("100");
				_application->m_rotation_value=0.0f;
				_application->m_rotation_control->settext("0");

				_application->m_pascal->m_world = _rotate(float(_radians(-90.0f)),_vec3(1.0f,0.0f,0.0f));


				_api_manager->m_light.ambient   = _vec4(0.98f,0.98f,0.98f,1.0f);
				_api_manager->m_light.diffuse   = _vec4(0.7f,0.7f,0.7f,1.0f);
				_api_manager->m_light.specular  = _vec4(0.3f,0.3f,0.3f,1.0f);
				_api_manager->m_light.direction = _vec3(1.0,-1.0f,0.0f);
				application_error_hr(_fx->SetValue(_api_manager->m_hlight, &_api_manager->m_light, sizeof(_light)));

			}
			/* vsync image control */
			if( m_id == _application->m_vsync_id ){
				if( _application->testflags(application_vsync)){
					_api_manager->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
					_application->removeflags(application_vsync);
					_application->m_vsync_button->settext("vsync");
				}else{
					_api_manager->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
					_application->addflags(application_vsync);
					_application->m_vsync_button->settext("vsync off");
				}
				_api_manager->reset();
			}
			/* fullscreen image control */
			if( m_id == _application->m_fullscreen_id ){

				if( _application->testflags(application_fullscreen)){
					_application->removeflags(application_fullscreen);
					_application->m_fullscreen_button->settext("fullscreen");
					_window->enablefullscreenmode(false);
				}else{
					_application->addflags(application_fullscreen);
					_application->m_fullscreen_button->settext("windowed");
					_window->enablefullscreenmode(true);
				}

			}




		}
		}
						 }break;
	case WM_LBUTTONUP :{
		removeflags(ui_left_button_down);
					   }break;

	case WM_MOUSEMOVE :{
		if(intersection_test()){ addflags(ui_mouse_over); }
		else{ removeflags(ui_mouse_over); }
					   }
					break;
	}

}

void ui_button::settext(const char* text){
	if(text){
		m_string = text;
		genforegroundbuffer();
	}
}

