#include "PhaseSetPlayer.h"
#include "Field.h"
#include "GlobalData.h"
#include "Client.h"
#include "Cutin.h"

PhaseSetPlayer::PhaseSetPlayer( GlobalDataPtr data, FieldPtr field, CutinPtr cutin, short int player_num ) :
Phase( player_num ),
_data( data ),
_field( field ),
_cutin( cutin ) {
	_selected = false;
	_clicking = 0;
}

PhaseSetPlayer::~PhaseSetPlayer( ) {
}

void PhaseSetPlayer::update( ) {
	if ( _cutin->isCutin( ) ) {
		return;
	}

	if ( _selected ) {
		return;
	}

	_field->setInfoText( "" );
	_field->setInfoText( "���Ȃ��̔z�u�����Ă�������", YELLOW, Drawer::LITTLE_BIG );
	_field->setInfoText( "" );
	_field->setInfoText( "���Ȃ��̃v���C���[�J���[��", YELLOW, Drawer::LITTLE_BIG );
	_field->setInfoText( "", RED, Drawer::SUPER_BIG );
	std::string your_color = "��";
	switch ( _player_num ) {
		case 0: your_color = "��"; break;
		case 1: your_color = "��"; break;
	}
	_field->setInfoText( your_color, ( COLOR )( RED + _player_num ), Drawer::SUPER_BIG );
	_field->setInfoText( "" );
	_field->setInfoText( "�ł�", YELLOW, Drawer::LITTLE_BIG );


	bool select = false;
	selectPlayerPos( &select );

	if ( !select ) {
		return;
	}

	_field->setInfoText( "" );
	_field->setInfoText( "����������Ċm�肵�Ă�������", WATER, Drawer::LITTLE_BIG );
	_field->activeButtonLighting( );
	_field->changeClickButton( );
	
	if ( _data->getClickingLeft( ) ) {
		_clicking = _data->getClickingLeft( );	
		return;
	} else if ( _clicking == 0 ) {
		return;
	}
	_clicking = 0;

	if ( !_field->isHitDecisionButton( ) ) {
		return;
	}

	if ( _field->isSelectedPlayer( ) ) {
		return;
	}

	_field->playerPosSelected( );
	_field->setPlayerPoint( _player_num, _field->getTmpPlayerPoint( ) );

	_selected = true;
	
	_data->getClientPtr( )->setPlayerPos( _field->getPlayerPosIdx( _player_num ) );
	_data->getClientPtr( )->sendTcp( );
}

void PhaseSetPlayer::selectPlayerPos( bool *select ) {
	//�q�b�g���Ă���|�W�V������T��
	if ( _field->getTmpPlayerPoint( ) != -1 ) {
		*select = true;
	}

	_field->hitPlayerPos( );

	int pos = _field->getPlayerPosHitNum( );

	if ( !_data->getClickLeft( ) ) {
		return;
	}

	if ( _field->isHitDecisionButton( ) ) {
		return;
	}

	_field->setTmpPlayerPoint( );

	*select = false;
	if ( pos != -1 ) {
		*select = true;
	}
}