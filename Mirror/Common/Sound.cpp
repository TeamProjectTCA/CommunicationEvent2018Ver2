#include "Sound.h"
#include "DxLib.h"
#include "Debug.h"
#include "MultiThreadLoad.h"
#include "LoadCSV.h"
#include <assert.h>
#include <errno.h>

const std::string PATH = "Resources/sound/";

Sound::Sound( ) {
	_sound_load = false;

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

		//���y�ǂݍ��݂����邩�ǂ���
		if ( "SOUND_LOAD" == data[ i ].tag ) {
			if ( atoi( data[ i ].values.begin( )->c_str( ) ) ) {
				_sound_load = true;
			}
			break;
		}
	}

	//�}���`�X���b�h
	if ( load_scene && _sound_load ) {
		_load = MultiThreadLoadPtr( new MultiThreadLoad( "���y�f�[�^��ǂݍ��ݒ�" ) );
	} else {
		_load = MultiThreadLoadPtr( new MultiThreadLoad( ) );
	}

	//�n���h���ǂݍ���
	initialize( );

	//�ǂݍ��݂��I���܂Ń��[�h��ʂ�`��
	_load->update( );
}

Sound::~Sound( ) {
	InitSoundMem( );
}

void Sound::initialize( ) {
	_dir_num = 0;
	if ( !_sound_load ) {
		return;
	}

	inputFileName( PATH );

	for ( int i = 0; i < _dir_num; i++ ) {
		Directory add;
		memset( &add, 0, sizeof( Directory ) );

		//�f�B���N�g���������
		switch ( ( SOUND )i ) {
			case TITLE_BGM:     add.name = "TitleBGM";     break;
			case GAME_BGM:      add.name = "GameBGM";      break;
			case EFFECT_SOUND:  add.name = "SoundEffect";  break;
			default:
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

	inputSound( );
}

Wav Sound::getWav( SOUND item, int num ) const {
	if ( !_sound_load ) {
		return Wav( );
	}

	int dir = item;
	Wav tmp;
	memset( &tmp, 0, sizeof( Wav ) );
	if ( item > SOUND_MAX - 1 ) {
		DebugPtr debug( new Debug( ) );
		debug->error( "Sound->getPng : �񋓌^�̒ǉ��Y��̉\��������܂�" );
	}

	int size = ( int )_data[ dir ].wav.size( );
	if ( size < num ) {
		DebugPtr debug( new Debug( ) );
		debug->error( "Sound->getPng : �v�f���I�[�o�[���Ă��܂�" );
	}
	return _data[ dir ].wav[ num ];
}

void Sound::check( int wav ) const {
	if ( wav != -1 ) {
		return;
	} else {
		DebugPtr debug( new Debug( ) );
		debug->error( "Sound->check : �����ǂݍ��݃G���[!!" );
	}
}

void Sound::inputSound( ) {
	int size = ( int )_file.size( );
	//�}���`�X���b�h�ɍő吔���Z�b�g
	_load->setMaxLength( ( float )size );

	int dir = 0;
	std::string path = PATH;
	std::string input = "\0";

	for ( int i = 0; i < size; i++ ) {
		if ( _file[ i ].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			for ( int j = 0; j < _dir_num; j++ ) {
				if ( strcmp( _file[ i ].cFileName, _data[ j ].name.c_str( ) ) == 0 ) {
					path = PATH + _file[ i ].cFileName + "/";
					input = _file[ i ].cFileName;
					dir = j;
					break;
				}
			}
			continue;
		}

		for ( int j = 0; j < _dir_num; j++ ) {
			if ( strcmp( input.c_str( ), _data[ j ].name.c_str( ) ) == 0 ) {
				Wav add;
				memset( &add, 0, sizeof( Wav ) );

				//sound���C���v�b�g
				add.wav = LoadSoundMem( ( path + _file[ i ].cFileName ).c_str( ) );

				check( add.wav );
				_data[ dir ].wav.push_back( add );
			}
		}
	}
}

void Sound::inputFileName( std::string path ) {
	WIN32_FIND_DATA find;
	HANDLE handle;

	handle = FindFirstFile( ( path + "*" ).c_str( ), &find );

	if ( handle == INVALID_HANDLE_VALUE ) {
		DebugPtr debug( new Debug( ) );
		FindClose( handle );
		debug->error( "Sound->inputFileName : " + path + "�����݂��܂���B\n"
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