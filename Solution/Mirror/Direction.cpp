#include "Direction.h"
#include "DxLib.h"
#include "GlobalData.h"
#include "Debug.h"
#include "Title.h"
#include "Image.h"

Direction::Direction( std::shared_ptr< GlobalData > data, std::shared_ptr< Debug > debug ) :
_data( data ),
_debug( debug ) {
	initialize( );
}

Direction::~Direction( ) {
}

void Direction::initialize( ) {
	_scene = NONE;
	_data->initialize( );
}

void Direction::update( ) {
	_data->update( );
	_scene = _data->getScene( );

	std::map< SCENE, std::shared_ptr< Base > >::iterator ite;
	ite = _exe.begin( );
	for ( ite; ite != _exe.end( ); ite++ ) {
		//if ( ite->first == _scene ) {
		//	ite->second->update( );
		//}
		//if ( ite->first == ALL ) {
		//	ite->second->update( );
		//}
		ite->second->update( );
	}

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

void Direction::add( SCENE scene, std::shared_ptr< Base > ptr ) {
	_exe[ scene ] = ptr;
}

void Direction::run( ) {
	// GlobalData のフラグが 0 であれば全プロセス終了
	while ( _data->getFlag( ) ) {
		if ( ScreenFlip( ) != 0 || ProcessMessage( ) != 0 || ClearDrawScreen( ) != 0 ) {
			break;
		}

		//計算フェイズ
		update( );

		//描画
		Base::ImageProperty* data = _data->getTitlePtr( )->getBackPng( );
		DrawRotaGraphF( WIDTH/ 2, HEIGHT / 2, 1, 0, data->png, TRUE );
	}
}
