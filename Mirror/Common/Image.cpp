#include "Image.h"
#include "DxLib.h"
#include "Debug.h"
#include "Loading.h"
#include "MultiThreadLoad.h"
#include "LoadCSV.h"
#include <assert.h>
#include <errno.h>

const std::string IMAGE_PATH = "Resources/image/";

Image::Image( MACHINE_TYPE type ) {
	_image_load = true;

	LoadCSVPtr csv = LoadCSVPtr( new LoadCSV( ) );
	std::vector< CsvData > data;
	csv->read( data, "flag" );
	int size = ( int )data.size( );
	bool load_scene = false;
	for ( int i = 0; i < size; i++ ) {
		//���[�h�V�[���`����s��������
		if ( "LOAD_SCENE" == data[ i ].tag ) {
			if ( atoi( data[ i ].values.begin( )->c_str( ) ) ) {
				load_scene = true;
			}
			continue;
		}

		if ( type != SERVER ) {
			continue;
		}

		//�摜���[�h���邩�ǂ���
		if ( "LIVE_DRAW" == data[ i ].tag ) {
			if ( atoi( data[ i ].values.begin( )->c_str( ) ) != 1 ) {
				_image_load = false;
			}
			continue;
		}
	}
	
	//�}���`�X���b�h
	if ( load_scene && _image_load ) {
		_load = MultiThreadLoadPtr( new MultiThreadLoad( "�摜�f�[�^��ǂݍ��ݒ�" ) );
	} else {
		_load = MultiThreadLoadPtr( new MultiThreadLoad( ) );
	}

	//�n���h���ǂݍ���
	initialize( );

	//�ǂݍ��݂��I���܂Ń��[�h��ʂ�`��
	_load->update( );

	//�摜�� Width, Height ���擾
	for ( int i = 0; i < ( int )_data.size( ); i++ ) {
		for ( int j = 0; j < ( int )_data[ i ].png.size( ); j++ ) {
			Png *png = &_data[ i ].png[ j ];
			GetGraphSize( png->png, &png->width, &png->height );
		}
	}
}

Image::~Image( ) {
	InitGraph( );
}

void Image::initialize( ) {
	if ( !_image_load ) {
		return;
	}

	_dir_num = 0;
	inputFileName( IMAGE_PATH );

	for ( int i = 0; i < _dir_num; i++ ) {
		Directory add;
		memset( &add, 0, sizeof( Directory ) );

		//�f�B���N�g���������
		switch ( ( IMAGE )i ) {
		case NONE_IMAGE          : add.name = "none"         ; break;
		case BUTTON_IMAGE        : add.name = "button"       ; break;
		case CUTIN_STRING_IMAGE  : add.name = "cutinString"  ; break;
		case CUTIN_BACK_IMAGE    : add.name = "cutinImage"   ; break;
		case BATTLE_IMAGE        : add.name = "battle"       ; break;
		case ITEM_IMAGE          : add.name = "item"         ; break;
		case PLAYER_IMAGE        : add.name = "player"       ; break;
		case EFFECT_IMAGE        : add.name = "effect"       ; break;
		case LAZER_IMAGE         : add.name = "lazer"        ; break;
		case BAR_IMAGE           : add.name = "bar"          ; break;
		case BACKGROUND_IMAGE    : add.name = "background"   ; break;
		case TITLE_IMAGE         : add.name = "title"        ; break;
		case COMMAND_IMAGE       : add.name = "command"      ; break;
		case TIME_IMAGE          : add.name = "time"         ; break;
		case NUMBER_IMAGE        : add.name = "number"       ; break;
		case OTHER_IMAGE         : add.name = "other"        ; break;
		default	:
			{
				errno_t directry_not_find = 0;
				DebugPtr debug( new Debug( ) );
				debug->error( "Image->initialize : _dir_num�̐��l���傫�����邩�A�f�B���N�g�����̒ǉ��Y��ł�" );
				assert( directry_not_find );
			}
		}
		_data.push_back( add );
	}

	FILE *fp;
	if ( fopen_s( &fp, "filelist.txt", "w" ) != 0 ) {
		exit( 0 );
	}
	for ( int i = 0; i < _file.size( ); i++ ) {
		if ( _file[ i ].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			fprintf_s( fp, "\n[ %s ]\n", _file[ i ].cFileName );
			continue;
		}
		fprintf_s( fp, "%s\n", _file[ i ].cFileName );
	}
	fclose( fp );

	inputImage( );
}

Png Image::getPng( IMAGE item, int num ) const {
	if ( !_image_load ) {
		return Png( );
	}

	int dir = item;
	Png tmp;
	memset( &tmp, 0, sizeof( Png ) );
	if ( item > IMAGE_DIR_MAX - 1 ) {
		DebugPtr debug( new Debug( ) );
		debug->error( "Image->getPng : �񋓌^�̒ǉ��Y��̉\��������܂�" );
	}

	int size = ( int )_data[ dir ].png.size( );
	if ( size < num ) {
		DebugPtr debug( new Debug( ) );
		debug->error( "Image->getPng : �v�f���I�[�o�[���Ă��܂�" );
	}
	return _data[ dir ].png[ num ];
}

void Image::check( int png ) const {
	if ( png != -1 ) {
		return;
	} else {
		DebugPtr debug( new Debug( ) );
		debug->error( "Image->check : �摜�ǂݍ��݃G���[!!" );
	}
}

void Image::inputImage( ) {
	int size = ( int )_file.size( );
	//�}���`�X���b�h�ɍő吔���Z�b�g
	_load->setMaxLength( ( float )size );

	int dir = 0;
	std::string path = IMAGE_PATH;
	std::string input = "\0";

	for ( int i = 0; i < size; i++ ) {
		if ( _file[ i ].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			for ( int j = 0; j < _dir_num; j++ ) {
				if ( strcmp( _file[ i ].cFileName, _data[ j ].name.c_str( ) ) == 0 ) {
					path = IMAGE_PATH + _file[ i ].cFileName + "/";
					input = _file[ i ].cFileName;
					dir = j;
					break;
				}
			}
			continue;
		}

		for ( int j = 0; j < _dir_num; j++ ) {
			if ( strcmp( input.c_str( ), _data[ j ].name.c_str( ) ) == 0 ) {
				Png add;
				memset( &add, 0, sizeof( Png ) );
				
				//�摜���C���v�b�g
				add.png = LoadGraph( ( path + _file[ i ].cFileName ).c_str( ) );

				check( add.png );
				_data[ dir ].png.push_back( add );
			}
		}
	}
}

void Image::inputFileName( std::string path ) {
	WIN32_FIND_DATA find;
	HANDLE handle;

	handle = FindFirstFile( ( path + "*" ).c_str( ), &find );

	if ( handle == INVALID_HANDLE_VALUE ) {
		DebugPtr debug( new Debug( ) );
		FindClose( handle );
		debug->error( "Image->inputFileName : " + path + "�����݂��܂���B\n"
					  "���f�B���N�g���̎w�肪�Ԉ���Ă��邩�t�@�C�������݂��Ȃ��\��������܂��B" );
	}

	do {
		if ( ( find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
			if ( strcmp( find.cFileName, "." ) != 0 && strcmp( find.cFileName, ".." ) != 0 ) {
				_dir_num++;
				_file.push_back( find );
				inputFileName( ( path + find.cFileName + "/" ) );
			}
		} else {
			std::string file_name = find.cFileName;
			if ( file_name.find( ".db" ) == std::string::npos ) {
				_file.push_back( find );
			}
		}
	} while ( FindNextFile( handle, &find ) );

	FindClose( handle );
}