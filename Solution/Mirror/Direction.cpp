#include "Direction.h"
#include "DxLib.h"
#include "Base.h"
#include "GlobalData.h"
#include "Debug.h"
#include "Title.h"
#include "Image.h"

Direction::Direction( GlobalDataPtr data ) :
_data( data ) {
	initialize( );
}

Direction::~Direction( ) {
}

void Direction::initialize( ) {
	_scene = NONE;
	_data->initialize( );

	_debug = DebugPtr( new Debug( _data ) );
	_title = TitlePtr( new Title( _data ) );

	_data->setPtr( _debug );
	_data->setPtr( _title );

	add( ALL, _debug );
	add( TITLE, _title );
}

void Direction::update( ) {
	_data->update( );
	_scene = _data->getScene( );

	//デバッグON
	if ( _data->getKeyState( KEY_INPUT_SPACE ) == 1 ) {
		_debug->setFlag( ( _debug->getFlag( ) + 1 ) % 2 );
	}

	_debug->addLog( "test" );

	//終了
	if ( _data->getKeyState( KEY_INPUT_ESCAPE ) ) {
		_data->setFlag( 0 );
	}
}

void Direction::initNextProcess( ) {
}

void Direction::add( SCENE scene, BasePtr ptr ) {
	_exe[ scene ] = ptr;
}

void Direction::run( ) {
	update( );

	std::map< SCENE, BasePtr >::iterator ite;
	ite = _exe.begin( );
	for ( ite; ite != _exe.end( ); ite++ ) {
		if ( ite->first == _scene ) {
			if ( ite->second->getFlag( ) ) {
				ite->second->update( );
			}
		}
		if ( ite->first == ALL ) {
			if ( ite->second->getFlag( ) ) {
				ite->second->update( );
			}
		}
	}
}
