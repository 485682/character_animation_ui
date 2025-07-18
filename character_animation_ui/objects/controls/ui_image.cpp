#include "ui_image.h"

#include "application.h"
#include "d3d_window.h"
#include "d3d_manager.h"


#include "pascal.h"
#include "ui_static.h"

ui_image::ui_image(){
	m_foreground_vertex_buffer = NULL;
	m_background_vertex_buffer = NULL;
	m_x = m_y = 0.0f;
	m_width  = 10.0f;
	m_height = 10.0f;

	m_image_texture = NULL;
}

bool ui_image::genbackgroundbuffer(){

	application_releasecom(m_foreground_vertex_buffer);

	application_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(6*sizeof(ui_vertex),D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&m_background_vertex_buffer,0));
	if(!m_background_vertex_buffer){ application_throw("vertex buffer"); }

	ui_vertex * v = 0;

	application_throw_hr(m_background_vertex_buffer->Lock(0, 0, (void**)&v, 0));

	_vec2 uv_up_right   = _vec2( 1.0f , 0.0f );
	_vec2 uv_up_left    = _vec2( 0.0f , 0.0f );
	_vec2 uv_down_right = _vec2( 1.0f , 1.0f );
	_vec2 uv_down_left  = _vec2( 0.0f , 1.0f );

	v[0].m_vertex.x  = m_width + m_x;
	v[0].m_vertex.y  = 0.0f + m_y;
	v[0].m_uv        = uv_up_right;

	v[1].m_vertex.x  = 0.0f + m_x;
	v[1].m_vertex.y  = m_height + m_y;
	v[1].m_uv        = uv_down_left;

	v[2].m_vertex.x  = 0.0f + m_x;
	v[2].m_vertex.y  = 0.0f + m_y;
	v[2].m_uv        = uv_up_left;

	v[3].m_vertex.x  = m_width + m_x;
	v[3].m_vertex.y  = 0.0f + m_y;
	v[3].m_uv        = uv_up_right;

	v[4].m_vertex.x  = m_width + m_x;
	v[4].m_vertex.y  = m_height + m_y;
	v[4].m_uv        = uv_down_right;

	v[5].m_vertex.x  = 0.0f + m_x;
	v[5].m_vertex.y  = m_height + m_y;
	v[5].m_uv        = uv_down_left;

	application_throw_hr(m_background_vertex_buffer->Unlock());


	return true;
}

bool ui_image::init(){
	genbackgroundbuffer();
	return true;
}

void ui_image::clear(){
	application_releasecom(m_background_vertex_buffer);
	application_releasecom(m_image_texture);
}

bool ui_image::update(){

	application_throw_hr(_fx->SetTechnique(_api_manager->m_htech_ui_foreground));
	application_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_ui_foreground_vertex_declaration ));
	application_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0,m_background_vertex_buffer, 0, sizeof(ui_vertex)));


	application_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_image_texture ));

	application_throw_hr(_fx->Begin(NULL, 0));
	application_throw_hr(_fx->BeginPass(0));
	application_throw_hr(_api_manager->m_d3ddevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0,2) );
	application_throw_hr(_fx->EndPass());
	application_throw_hr(_fx->End());

	return true;
}

bool ui_image::setimage( int resource_id ){
	application_throw_hr( D3DXCreateTextureFromResource( _api_manager->m_d3ddevice, NULL, MAKEINTRESOURCE( resource_id ), &m_image_texture ) );
	return true;
}

bool ui_image::setimage(const char * filename ){
	application_throw_hr(D3DXCreateTextureFromFile(_api_manager->m_d3ddevice,filename, &m_image_texture));
	return true;
}

void ui_image::msgproc(UINT msg, WPARAM wParam, LPARAM lParam){

	if( (msg != WM_LBUTTONDOWN) && (_application->m_ui->m_current_control != m_id) ){ return; }

	switch( msg )
	{
	case WM_LBUTTONDOWN : { if(intersection_test()){ 
		_application->m_ui->m_current_control = m_id; 

		/* 
		* all event code is placed in the classes main msg proc
		* a more practical way would be to derive the class leaving this function mostly empty
		*/


		/*animation switch**************************************************************************************/
		if( (m_id==_application->m_animation_increment_id) || (m_id==_application->m_animation_decrement_id) ){

			if(m_id==_application->m_animation_increment_id){ _application->m_keyframe++; }
			if(m_id==_application->m_animation_decrement_id){ _application->m_keyframe--; }

			if(_application->m_keyframe>3){ _application->m_keyframe=0; }
			if(_application->m_keyframe<0){ _application->m_keyframe=3; }

			if(_application->m_keyframe ==0 ){ 
				_application->m_pascal->setbindpose(); 
				_application->m_animation_control->settext("bindpose");
			}
			if(_application->m_keyframe ==1 ){
				/*walk animation*/
				_application->m_pascal->keyframe(0,5);
				/*seconds per frame*/
				_application->m_pascal->m_animation_length = 0.14f;
				_application->m_animation_control->settext("walk");
			}
			if(_application->m_keyframe ==2 ){
				/*wave animation*/
				_application->m_pascal->keyframe(7,31);
				_application->m_pascal->m_animation_length = 0.1f;
				_application->m_animation_control->settext("wave");
			}
			if(_application->m_keyframe ==3 ){
				/*stretch animation*/
				_application->m_pascal->keyframe(32,72);
				_application->m_pascal->m_animation_length = 0.1f;
				_application->m_animation_control->settext("strech");
			}
		}				  
		/******************************************************************************************************/

		/* rotate pascal **************************************************************************************/
		if( (m_id==_application->m_rotation_increment_id) || (m_id==_application->m_rotation_decrement_id) ){
            

			if(m_id==_application->m_rotation_increment_id){ _application->m_rotation_value+=10.0f; }
			if(m_id==_application->m_rotation_decrement_id){ _application->m_rotation_value-=10.0f; }

			if( _application->m_rotation_value>360.0f){ _application->m_rotation_value=0.0f;   }
			if( _application->m_rotation_value<0.0f  ){ _application->m_rotation_value=360.0f; }

			_application->m_pascal->m_world = _rotate(float(_radians(-90.0f)),_vec3(1.0f,0.0f,0.0f));
			_application->m_pascal->m_world = _application->m_pascal->m_world*_rotate(float(_radians( _application->m_rotation_value )),_vec3(0.0f,1.0f,0.0f));

			_application->m_rotation_control->settext(_utility::inttostring( int(_application->m_rotation_value) ).m_data );

		}
		/******************************************************************************************************/

		/* light intensity **************************************************************************************/
		if( (m_id==_application->m_light_increment_id) || (m_id==_application->m_light_decrement_id) ){

			if(m_id==_application->m_light_increment_id){ _application->m_light_value+=0.01f; }
			if(m_id==_application->m_light_decrement_id){ _application->m_light_value-=0.01f; }

			if( _application->m_light_value>1.0f) { _application->m_light_value=1.0f;   }
			if( _application->m_light_value<0.0f) { _application->m_light_value=0.0f; }
			float ambient  = 0.98f*_application->m_light_value;
			float diffuse  = 0.70f*_application->m_light_value;
			float specular = 0.30f*_application->m_light_value;

			_api_manager->m_light.ambient   = _vec4(ambient,ambient,ambient,1.0f);
			_api_manager->m_light.diffuse   = _vec4(diffuse,diffuse,diffuse,1.0f);
			_api_manager->m_light.specular  = _vec4(specular,specular,specular,1.0f);
			_api_manager->m_light.direction = _vec3(1.0,-1.0f,0.0f);
			application_error_hr(_fx->SetValue(_api_manager->m_hlight, &_api_manager->m_light, sizeof(_light)));

			_application->m_light_control->settext(_utility::inttostring( int(_application->m_light_value*100) ).m_data );

		}
		///******************************************************************************************************/


						  } 

						  }break;
	}

}

