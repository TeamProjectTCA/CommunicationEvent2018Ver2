#include "PhaseSetMirror.h"
#include "Field.h"
#include "GlobalData.h"
#include "Client.h"
#include "Cutin.h"
#include "Sound.h"
#include "Soundplayer.h"

PhaseSetMirror::PhaseSetMirror( GlobalDataPtr data, FieldPtr field, CutinPtr cutin, short int player_num ) :
Phase( player_num ),
_data( data ),
_field( field ),
_cutin( cutin ) {
	_client = _data->getClientPtr( );
	_soundplayer = _data->getSoundplayerPtr( );

	_recv = false;
	_double_mirror = false;
	_order_past = -1;
	_clicking = 0;
	
	//SE
	SoundPtr sound_ptr = _data->getSoundPtr( );
	{
		_hitfield_se = Base::SoundProperty( );
		_hitfield_se.isLoop = false;
		_hitfield_se.top = true;
		_hitfield_se.wav = sound_ptr->getWav( EFFECT_SOUND, SELECT_SE ).wav;
	}

	{
		_button_se = Base::SoundProperty( );
		_button_se.isLoop = false;
		_button_se.top = true;
		_button_se.wav = sound_ptr->getWav( EFFECT_SOUND, BUTTON_SE ).wav;
	}

	{
		_mirrorselect_se = Base::SoundProperty( );
		_mirrorselect_se.isLoop = false;
		_mirrorselect_se.top = true;
		_mirrorselect_se.wav = sound_ptr->getWav( EFFECT_SOUND, SELECT_SE ).wav;
	}

	{
		_itemselect_se = Base::SoundProperty( );
		_itemselect_se.isLoop = false;
		_itemselect_se.top = true;
		_itemselect_se.wav = sound_ptr->getWav( EFFECT_SOUND, SELECT_SE ).wav;
	}

	{
		_cancel_se = Base::SoundProperty( );
		_cancel_se.isLoop = false;
		_cancel_se.top = true;
		_cancel_se.wav = sound_ptr->getWav( EFFECT_SOUND, BACK_SE ).wav;
	}
}

PhaseSetMirror::~PhaseSetMirror( ) {
}

void PhaseSetMirror::update( ) {
	if ( _cutin->isCutin( ) ) {
		return;
	}

	recvItem( );
	if ( _use_item ) {
		//�A�C�e���J�b�g�C��
		if ( !_cutin->isCutin( ) && _item != ( short int )REVERSE_MIRROR ) {
			switch ( ( ITEM )_item ) {
			case LAZER_RESET:
				_cutin->setImage( Cutin::CUTIN_TYPE_ITEM, Cutin::CUTIN_ITEM_LAZER_RESET );
				break;
			case DOUBLE_MIRROR:
				_cutin->setImage( Cutin::CUTIN_TYPE_ITEM, Cutin::CUTIN_ITEM_DOUBLE_MIRROR );
				break;
			}
			_cutin->setSpeed( 1.8f );
		}

		invocationItem( );
		return;
	}

	recv( );

	if ( !_recv ) {
		return;
	}

	setTmpMirror( );
	setMirror( );

	updateItem( );
}

void PhaseSetMirror::setMirror( ) {
	if ( !_tmp_mirror.flag ) {
		return;
	}
	_field->activeButtonLighting( );
	_field->changeClickButton( );
	
	if ( _data->getClickingLeft( ) ) {
		_clicking = _data->getClickingLeft( );	
		return;
	} else if ( _clicking == 0 ) {
		return;
	}

	if ( !_field->isHitDecisionButton( ) ) {
		_clicking = 0;
		if ( _field->getHitItemIdx( ) != -1 ) {
			_field->resetTmpMirror( );
			_tmp_mirror = Field::Mirror( );
		}
		return;
	}

	_field->mirrorPosSelected( );

	_client->setCtsPlayerNum( );
	_client->setCtsAngle( _tmp_mirror.angle );
	_client->setCtsX( _tmp_mirror.x );
	_client->setCtsY( _tmp_mirror.y );
	_client->setCtsFlag( _tmp_mirror.flag );

	_tmp_mirror = Field::Mirror( );
	_field->resetTmpMirror( );

	if ( _field->getSelectItem( ) != -1 ) {
		return;
	}
	_clicking = 0;
	_client->sendTcp( );
}

void PhaseSetMirror::setTmpMirror( ) {
	if ( _order == -1 ) {
		return;
	}

	//�v���C���[�J�b�g�C��
	if ( _order != _order_past ) {
		_cutin->setImage( Cutin::CUTIN_TYPE_PLAYER,
			              _player_num == _order ? 
			              Cutin::CUTIN_PLAYER_YOUR_TURN : 
			              Cutin::CUTIN_PLAYER_ENEMY_TURN,
						  true
		                );
		_cutin->setSpeed( 1.5f );
	}
	_order_past = _order;

	if ( _cutin->isCutin( ) ) {
		return;
	}

	if ( _order != _player_num ) {
		_field->setInfoText( "", YELLOW, Drawer::BIG );
		_field->setInfoText( "����", ( ( _player_num + 1 ) % PLAYER_NUM ? WATER : RED ), Drawer::SUPER_BIG );
		_field->setInfoText( "���z�u���Ă��܂�", YELLOW, Drawer::LITTLE_BIG );
		return;
	}
	//���Ԃ����炷
	_field->deadCount( );

	if ( _field->isSelectedMirror( ) ) {
		return;
	}

	_field->setInfoText( "����z�u���Ă��������B" );
	if ( _double_mirror ) {
		_field->setInfoText( "", RED, Drawer::LITTLE_BIG );
	} else {
		_field->setInfoText( "", RED, Drawer::BIG );
	}
	_field->setInfoText( "����̓I", ( ( _player_num + 1 ) % PLAYER_NUM ) ? WATER : RED, Drawer::BIG );
	_field->setInfoText( "" );
	_field->setInfoText( "�Ƀ��[�U�[�𓖂Ă܂��傤�I" );
	if ( _double_mirror ) {
		_field->setInfoText( "" );
		_field->setInfoText( "2���ڂ�z�u���Ă�������", RED, Drawer::LITTLE_BIG );
	}

	//���[�U�[���Z�b�g�I�����͖���
	if ( _field->getSelectItem( ) == LAZER_RESET ) {
		return;
	}

	//���o�[�X�~���[�I�����͖���
	if ( _field->getSelectItem( ) == REVERSE_MIRROR ) {
		return;
	}

	//�����蔻����v�Z
	_field->hitFieldPos( );

	//�q�b�g���Ă���}�X��T��
	int pos = _field->getFieldPosHitNum( );

	//�������������
	if ( _field->isHitDecisionButton( ) ) {
		return;
	}

	//�A�C�e���I����������
	if ( _field->getHitItemIdx( ) != -1 ) {
		return;
	}

	//�R�}���h���I������Ă��Ȃ����
	if ( _field->getMirrorCommand( ) == Field::COMMAND_NONE ) {
		return;
	}

	//�N���b�N����
	if ( !_data->getClickLeft( ) ) {
		return;
	}

	//�L�����Z���{�^����
	if ( _field->isHitItemCancelButton( ) ) {
		//�����I������Ă�����
		if ( _tmp_mirror.flag ) {
			//SE��炷
			_soundplayer->play( _cancel_se );
		}
	}

	_tmp_mirror = Field::Mirror( );
	_field->resetTmpMirror( );

	if ( pos < 0 ) {
		return;
	}

	//�����߂Ă���
	int x = pos % FIELD_COL;
	int y = pos / FIELD_COL;
	Field::COMMAND command = _field->getMirrorCommand( );
	switch ( command ) {
	case Field::COMMAND_RIGHT  : _tmp_mirror.angle = RIGHT     ; break;
	case Field::COMMAND_LEFT   : _tmp_mirror.angle = LEFT      ; break;
	case Field::COMMAND_DELETE : _tmp_mirror.angle = ANGLE_NONE; break;
	case Field::COMMAND_NONE   : return;
	}

	_tmp_mirror.x = x;
	_tmp_mirror.y = y;
	_tmp_mirror.flag = true;

	_field->setTmpMirrorPoint( _player_num, _tmp_mirror.x, _tmp_mirror.y, _tmp_mirror.angle );
}

void PhaseSetMirror::recv( ) {
	if ( !_client->isRecvingUdp( ) ) {
		return;
	}

	for ( int i = 0; i < PLAYER_NUM; i++ ) {
		int pos = _client->getPlayerPos( i );
		if ( pos == -1 ) {
			return;
		}
		_field->setPlayerPoint( i, pos );
	}

	int lazer_pos = _client->getLazerPoint( );
	if ( lazer_pos == ( unsigned char )-1 ) {
		return;
	}
	_field->setLazerPoint( lazer_pos );

	_recv = true;
}

void PhaseSetMirror::updateItem( ) {
	if ( _order != _player_num ) {
		return;
	}

	if ( _cutin->isCutin( ) ) {
		return;
	}

	int idx = _field->getHitItemIdx( );
	_field->changeClickButton( );

	int item = _field->getSelectItem( );
	//�{�^���_��
	switch ( item ) {
	case LAZER_RESET:
		_field->activeButtonLighting( );
		break;

	case DOUBLE_MIRROR: 
		if ( _field->isSelectedMirror( ) ) {
			_field->activeButtonLighting( );
		}
		break;

	case REVERSE_MIRROR:
		_field->activeButtonLighting( );
		break;

	default:
		break;
	}
	
	if ( _data->getClickingLeft( ) ) {
		_clicking = _data->getClickingLeft( );	
		return;
	} else if ( _clicking == 0 ) {
		return;
	}
	_clicking = 0;

	if ( _field->getFieldPosHitNum( ) != -1 ) {
		return;
	}


	//����{�^���������Ȃ���
	if ( !_field->isHitDecisionButton( ) ) {
		//�~���[�R�}���h��I�����ĂȂ���
		if ( _field->getHitMirrorCommandIdx( ) < 0 ) {
			//�t�B�[���h���I�����Ă��Ȃ����
			if ( _field->getFieldPosHitNum( ) < 0 ) {
				//�L�����Z���{�^���ł����-1
				if ( _field->isHitItemCancelButton( ) ) {
					//�A�C�e�����I������Ă�����SE��炷
					if (  _field->getSelectItem( ) != -1 ) {
						_soundplayer->play( _cancel_se );
					}
					idx = -1;
				}
				//�A�C�e���ԍ��Z�b�g
				if ( idx != -1 ) {
					_soundplayer->play( _itemselect_se );
				}
				_field->selectItem( idx );
			}
		}
		return;
	}

	item = _field->getSelectItem( );
	if ( item < 0 ) {
		return;
	}
	if ( item == ( int )DOUBLE_MIRROR ) {
		if ( !_field->isSelectedMirror( ) ) {
			return;
		}
		int order = 0;
		if ( !_player_num ) {
			order = ( _turn + 1 ) % 2 + 1;
		} else {
			order = _turn % 2 + 1;
		}

		if ( order != 1 ) {
			_field->mirrorPosNotSelected( );
			return;
		}
	}

	_client->setItemFlag( true );
	_client->setItem( _field->getSelectItem( ) );
	_client->setItemUser( );
	_client->sendTcp( );

	_field->useItem( );
}

void PhaseSetMirror::invocationItem( ) {
	if ( _item_callback ) {
		return;
	}

	switch ( ( ITEM )_item ) {
	case LAZER_RESET:
		{//���[�U�[�̈ʒu��ύX
			int lazer_pos = _client->getLazerPoint( );
			if ( lazer_pos == -1 ) {
				return;
			}
			_field->setLazerPoint( lazer_pos );
			_field->mirrorPosNotSelected( );
			_tmp_mirror = Field::Mirror( );
			_field->nextTurn( );
			_order_past = -1;
		}
		break;

	case DOUBLE_MIRROR:
		{//��2���z�u
			int idx = _client->getItemUser( );
			Field::Mirror mirror = Field::Mirror( );
			mirror.x = _client->getStcX( idx );
			mirror.y = _client->getStcY( idx );
			mirror.angle = _client->getStcAngle( idx );
			mirror.player_num = idx;

			_field->setMirrorPoint( mirror.player_num, mirror.x, mirror.y, mirror.angle );

			if ( idx == _player_num ) {
				_field->mirrorPosNotSelected( );
				_tmp_mirror = Field::Mirror( );
				_field->nextTurn( );
				_double_mirror = true;
			}
			_order_past = -1;
		}
		break;

	case REVERSE_MIRROR:
		{
			_field->setReverseFlag( );
		}
		break;
	}

	_client->setItemFlag( false );
	_client->setItem( 0 );
	_client->sendTcp( );
	_item_callback = true;
	_item = ( short int )ITEM_NONE;
}

void PhaseSetMirror::recvItem( ) {
	if ( !_client->isRecvingUdp( ) ) {
		return;
	}

	bool use = _client->isItemFlag( );

	if ( use != _use_item ) {
		_use_item = use;
		if ( !_use_item ) {
			return;
		}
		_item = _client->getItem( );
		_item_callback = false;
	}
}