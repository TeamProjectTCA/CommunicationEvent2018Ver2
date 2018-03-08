#include "DxLib.h"
#include "smart_ptr.h"
#include "GlobalData.h"
#include "Direction.h"
#include "Title.h"
#include "Console.h"
#include "Lazer.h"
#include "Field.h"
#include "Game.h"
#include "ResultClient.h"
#include "SelectItem.h"
#include "IpSetting.h"

/**********************************************************
*														  *
* Base �p���N���X�́@setFlag( 1 ); ���΂ɂ��邱��       *
*														  *
***********************************************************/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	SetWindowText( "Mirror" );
	ChangeWindowMode( 1 );
	//SetWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	SetGraphMode( WIDTH, HEIGHT, 32 );
	SetDoubleStartValidFlag( TRUE );
	SetAlwaysRunFlag( TRUE );
	SetConnectTimeOutWait( TIME_OUT );
	if ( DxLib_Init( ) != 0 ) {
		return -1;
	}
	SetDrawScreen( DX_SCREEN_BACK );

	{
		GlobalDataPtr data( new GlobalData( CLIENT ) );
		DirectionPtr direction( new Direction( CLIENT, data ) );

		//�v�Z�N���X
		ConsolePtr console( new Console( data ) );
		TitlePtr title( new Title( data ) );
		SelectItemPtr slct_item( new SelectItem( data ) );
		IpSettingPtr ip_setting( new IpSetting( data ) );
		FieldPtr field( new Field( data ) );
		data->setPtr( field );
		GamePtr game( new Game( data ) );
		ResultClientPtr result( new ResultClient( data, game ) );

		direction->add( ALL, data );
		direction->add( TITLE, title );
		direction->add( SELECT_ITEM, slct_item );
		direction->add( IP_SETTING, ip_setting );
		direction->add( CONNECT, console );
		direction->add( BATTLE, field );
		direction->add( BATTLE, game );
		direction->add( RESULT, result );

		// GlobalData �̃t���O�� 0 �ł���ΑS�v���Z�X�I��
		while ( data->getFlag( ) ) {
			if ( ScreenFlip( ) != 0 || ProcessMessage( ) != 0 || ClearDrawScreen( ) != 0 ) {
				break;
			}
			if ( data->isInitFlag( ) ) {
				direction->initialize( );
				data->setScene( TITLE );
			}
			direction->run( );
		}
	}

	DxLib_End( );
	return 0;
}