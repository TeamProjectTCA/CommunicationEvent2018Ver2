#pragma once
#include "Base.h"
#include "smart_ptr.h"
#include "const.h"
#include <string>
#include <array>

PTR( SelectItem );
PTR( GlobalData );
PTR( Drawer );
PTR( Soundplayer );

class SelectItem : public Base {
public:
	SelectItem( GlobalDataPtr data );
	virtual ~SelectItem( );

public:
	std::string getTag( );
	void initialize( );
	void update( );

private:
	void checkHitNotSelectItem( );
	void checkHitSelectedItem( );
	void calcItemPosition( );
	void calcDragNotSelectItemPos( );
	void calcDragSelectedItemPos( );
	void calcButtonAction( );
	void setSelectItem( );
	void inputCsv( );
	bool isDrag( ) const;
	int getHitButton( ) const;

private:
	std::string convItemString( ITEM item );

private:
	void drawNotSelectItem( ) const;
	void drawBackGround( ) const;
	void drawSelectedItemFrame( ) const;
	void drawSelectedItem( ) const;
	void drawSelectingItem( ) const;
	void drawSaveButton( ) const;
	void drawReturnButton( ) const;

private:
	enum BUTTON {
		NONE_BUTTON,
		SAVE_BUTTON,
		RETURN_BUTTON,
		BUTTON_MAX,
	};
	struct BoxObject {
		BoxCollider collider;
		LightImageProperty image;
	};
	struct SelectedItem {
		bool flag;
		short int type;
		BoxObject object;
	};
	
	//�萔
	static const int BUTTON_TYPE_MAX = 2;

	//�ϐ�
	short int _past_hit_item;
	short int _hit_notselect_item;//�I���\�A�C�e��
	short int _hit_selected_item;//�I���ς݃A�C�e��
	short int _wait_time;
	short int _bar_rate;
	short int _bar_width_half;
	short int _bar_height_half;
	bool _drag;
	bool _drop;
	bool _savebutton_clicking;
	bool _returnbutton_clicking;
	bool _input; //����
	std::array< BoxCollider, ITEM_POSSESSION_MAX > _select_boxes;

	//�摜�֌W
	int _background_handle;
	short int _item_image_halfsize;
	int _item_frame_handle;
	std::array< int, ITEM_MAX > _item_handles;
	std::array< int, BUTTON_TYPE_MAX > _savebutton_handles;
	std::array< int, BUTTON_TYPE_MAX > _returnbutton_handles;
	std::array< BoxObject, ITEM_MAX > _items;
	std::array< SelectedItem, ITEM_POSSESSION_MAX > _selected;
	BoxObject _savebutton;
	BoxObject _returnbutton;
	LightImageProperty _frame;
	LightImageProperty _bar_frame;
	LightImageProperty _bar;
	
	//�T�E���h�֌W
	SoundProperty _button_se;
	SoundProperty _save_se;
	SoundProperty _back_se;
	SoundProperty _select_se;

	GlobalDataPtr _data;
	DrawerPtr _drawer;
	SoundplayerPtr _soundplayer;
};

