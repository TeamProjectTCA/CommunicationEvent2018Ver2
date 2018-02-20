#include "SelectItem.h"
#include "GlobalData.h"
#include "Drawer.h"
#include "Image.h"

const float MARGIN  = WIDTH_F * 0.2f;

const float NOTSELECT_PITCH   = ( WIDTH_F - MARGIN * 2.0f ) / ITEM_MAX;
const float START_NOTSELECT_X = MARGIN + NOTSELECT_PITCH - NOTSELECT_PITCH * 0.5f;
const float START_NOTSELECT_Y = HEIGHT_F * 0.5f - HEIGHT_F * 0.2f;

const float SELECTED_PITCH   = ( WIDTH_F - MARGIN * 2.0f ) / ITEM_POSSESSION_MAX;
const float START_SELECTED_X = MARGIN + SELECTED_PITCH - SELECTED_PITCH * 0.5f;
const float START_SELECTED_Y = HEIGHT * 0.75;

SelectItem::SelectItem( GlobalDataPtr data ) :
_data( data ) {
	setFlag( 1 );
	_drawer = _data->getDrawerPtr( );

	std::array< ItemBox, ITEM_MAX >( ).swap( _items );
	std::array< BoxCollider, ITEM_POSSESSION_MAX >( ).swap( _select_boxes );

	//�w�i(���̓_�~�[)
	ImagePtr image_ptr = _data->getImagePtr( );
	_back_handle = image_ptr->getPng( NONE_IMAGE, 0 ).png;
	
	//�A�C�e���̉摜
	for ( int i = 0; i < ITEM_MAX; i++ ) {
		_items[ i ].image.png = image_ptr->getPng( ITEM_IMAGE, i ).png;
	}
	_item_image_hulfsize = ( short int )( ( float )image_ptr->getPng( ITEM_IMAGE, 0 ).width * 0.5f );

	//�I�𒆃A�C�e���̘g�̓����蔻��
	for ( int i = 0; i < ITEM_POSSESSION_MAX; i++ ) {
		_select_boxes[ i ].lx = START_SELECTED_X + SELECTED_PITCH * i - _item_image_hulfsize;
		_select_boxes[ i ].rx = START_SELECTED_X + SELECTED_PITCH * i + _item_image_hulfsize;
		_select_boxes[ i ].ly = START_SELECTED_Y - _item_image_hulfsize;
		_select_boxes[ i ].ry = START_SELECTED_Y + _item_image_hulfsize;
	}
}

SelectItem::~SelectItem( ) {
}

std::string SelectItem::getTag( ) {
	return "SELECT_ITEM";
}

void SelectItem::initialize( ) {
	_hit_item = -1;
	_drag = false;
	_drop = false;
	std::array< SelectedItem, ITEM_POSSESSION_MAX >( ).swap( _selected );
}

void SelectItem::update( ) {
	//���W�v�Z
	calcItemPosition( );

	//�����蔻��`�F�b�N
	checkHitItem( );
	if ( isDrag( ) ) {
		calcDragItemPos( );
	}

	//�`��
	drawItemImage( );
	drawBackGround( );
	drawSelectedItemFrame( );
	drawSelectedItem( );

	_drawer->setFrontString( false, 20, HEIGHT - 20, RED, std::to_string( _hit_item ) );
}

void SelectItem::calcItemPosition( ) {
	for ( int i = 0; i < ITEM_MAX; i++ ) {
		if ( _hit_item == i && isDrag( ) ) {
			continue;
		}
		//�摜�̍��W
		_items[ i ].image.cx = START_NOTSELECT_X + NOTSELECT_PITCH * i;
		_items[ i ].image.cy = START_NOTSELECT_Y;

		//�����蔻��
		_items[ i ].collider.lx = ( float )_items[ i ].image.cx - _item_image_hulfsize;
		_items[ i ].collider.ly = ( float )_items[ i ].image.cy - _item_image_hulfsize;
		_items[ i ].collider.rx = ( float )_items[ i ].image.cx + _item_image_hulfsize;
		_items[ i ].collider.ry = ( float )_items[ i ].image.cy + _item_image_hulfsize;
	}
}

void SelectItem::checkHitItem( ) {
	if ( _hit_item != -1 && isDrag( ) ) {
		return;
	}

	double mouse_x = _data->getMouseX( );
	double mouse_y = _data->getMouseY( );

	for ( int i = 0; i < ITEM_MAX; i++ ) {
		BoxCollider box = BoxCollider( );
		box.lx = _items[ i ].collider.lx;
		box.ly = _items[ i ].collider.ly;
		box.rx = _items[ i ].collider.rx;
		box.ry = _items[ i ].collider.ry;
		if ( box.lx <= mouse_x && mouse_x <= box.rx &&
			 box.ly <= mouse_y && mouse_y <= box.ry ) {
			_hit_item = i;
			return;
		}
	}
	_hit_item = -1;
}

void SelectItem::calcDragItemPos( ) {
	if ( _hit_item < 0 ) {
		return;
	}
	//�h���b�O�t���O�𗧂Ă�
	_drag = true;

	double mouse_x = _data->getMouseX( );
	double mouse_y = _data->getMouseY( );

	//�}�E�X�̈ʒu����ɃA�C�e���̒��S���W�܂ł̋���
	double distance_x = _items[ _hit_item ].image.cx - mouse_x;
	double distance_y = _items[ _hit_item ].image.cy - mouse_y;

	//���W���}�E�X����_�Ɉړ�
	_items[ _hit_item ].image.cx = mouse_x;
	_items[ _hit_item ].image.cy = mouse_y;

	//��ʊO����
	if ( mouse_x < 0 || WIDTH  < mouse_x ||
		 mouse_y < 0 || HEIGHT < mouse_y ) {
		_hit_item = -1;
	}
}

bool SelectItem::isDrag( ) const {
	if ( _data->getClickingLeft( ) > 0 ) {
		return true;
	}
	return false;
}

void SelectItem::setSelectItem( ) {
	_drag = false;

	if ( _hit_item < 0 ) {
		return;
	}

	//�b�����u�Ԃ����m������
	if ( isDrag( ) ) {
		return;
	}

	double mouse_x = _data->getMouseX( );
	double mouse_y = _data->getMouseY( );

	BoxCollider *box = &_items[ _hit_item ].collider;
	box->lx = ( float )mouse_x - _item_image_hulfsize;
	box->ly = ( float )mouse_y - _item_image_hulfsize;
	box->rx = ( float )mouse_x + _item_image_hulfsize;
	box->ry = ( float )mouse_y + _item_image_hulfsize;

	for ( int i = 0; i < ITEM_POSSESSION_MAX; i++ ) {
		if ( _select_boxes[ i ].lx <= box->lx && box->lx <= _select_boxes[ i ].rx ||
			 _select_boxes[ i ].lx <= box->rx && box->rx <= _select_boxes[ i ].rx   //x
			&&
			 _select_boxes[ i ].ly <= box->ly && box->ly <= _select_boxes[ i ].ry ||
			 _select_boxes[ i ].ly <= box->ry && box->ry <= _select_boxes[ i ].ry    //y
			) {
			_selected[ i ].flag = true;
			_selected[ i ].type = _hit_item;
			break;
		}
	}
}

void SelectItem::drawItemImage( ) const {
	ImageProperty image = ImageProperty( );

	for ( int i = 0; i < ITEM_MAX; i++ ) {
		image = ImageProperty( );
		if ( _hit_item == i ) {
			continue;
		}
		image.cx = _items[ i ].image.cx;
		image.cy = _items[ i ].image.cy;
		image.png = _items[ i ].image.png;
		_drawer->setImage( image );
	}

	//�I�𒆃A�C�e���͈�ԏ�ɕ`��
	if ( _hit_item < 0 ) {
		return;
	}
	image = ImageProperty( );
	image.cx  = _items[ _hit_item ].image.cx;
	image.cy  = _items[ _hit_item ].image.cy;
	image.png = _items[ _hit_item ].image.png;
	_drawer->setImage( image );
}

void SelectItem::drawBackGround( ) const {
	ImageProperty image = ImageProperty( );
	image.cx = WIDTH / 2;
	image.cy = HEIGHT / 2;
	image.png = _back_handle;

	_drawer->setBackImage( image );
}

void SelectItem::drawSelectedItemFrame( ) const {
	for ( int i = 0; i < ITEM_POSSESSION_MAX; i++ ) {
		_drawer->setFrontBox( _select_boxes[ i ].lx, _select_boxes[ i ].ly, _select_boxes[ i ].rx, _select_boxes[ i ].ry, RED );
	}
}

void SelectItem::drawSelectedItem( ) const {
	ImageProperty image = ImageProperty( );
	for ( int i = 0; i < ITEM_POSSESSION_MAX; i++ ) {
		if ( !_selected[ i ].flag ) {
			continue;
		}
		image = ImageProperty( );
		image.cx = START_SELECTED_X + SELECTED_PITCH * i;
		image.cy = START_SELECTED_Y;
		image.png = _items[ _selected[ i ].type ].image.png;

		_drawer->setImage( image );
	}
}