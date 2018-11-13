/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_spull( struct CockerEnvironment *env )
{
	char		repo[ REPO_LEN_MAX + 1 ] ;
	char		repo_file[ PATH_MAX + 1 ] ;
	char		image_file[ PATH_MAX + 1 ] ;
	char		image_id[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		version[ PATH_MAX + 1 ] ;
	char		image_rlayer_path_base[ PATH_MAX + 1 ] ;
	char		current_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;
	
	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( image_file , sizeof(image_file) , "%s.%s" , env->cmd_para.__image , COCKERIMAGE_FILE_EXTNAME );
	
	memset( image_id , 0x00 , sizeof(image_id) );
	strncpy( image_id , env->cmd_para.__image , sizeof(image_id)-1 );
	SplitImageVersion( image_id , version , sizeof(version) );
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image_id , (version[0]?version:"_") );
	
	memset( repo , 0x00 , sizeof(repo) );
	memset( repo_file , 0x00 , sizeof(repo_file) );
	nret = ReadFileLine( repo , sizeof(repo) , repo_file , sizeof(repo_file) , "%s/repo" , env->cocker_home ) ;
	I1TER1( "read %s failed\n" , repo_file )
	EIDTI( "read %s ok\n" , repo_file )
	
	/* download and import image file */
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "ssh %s \"ls ./%s.cockerimage\" >/dev/null 2>/dev/null" , repo , env->cmd_para.__image ) ;
	if( nret )
	{
		E( "*** ERROR : image '%s' not found in repo\n" , env->cmd_para.__image_file );
		if( ! env->cmd_para.__forcely )
			return nret;
	}
	
	if( env->cmd_para.__debug )
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "scp %s:~/%s ." , repo , image_file ) ;
	else
		nret = SnprintfAndSystem( cmd , sizeof(cmd) , "scp %s:~/%s . >/dev/null 2>/dev/null" , repo , image_file ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s" , env->images_path_base , image_id ) ;
	nret = SnprintfAndMakeDir( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image_id , (version[0]?version:"_") ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir %s failed[%d] , errno[%d]\n" , env->image_path_base , nret , errno )
	EIDTI( "mkdir %s ok\n" , env->image_path_base )
	
	nret = SnprintfAndMakeDir( image_rlayer_path_base , sizeof(image_rlayer_path_base)-1 , "%s/rlayer" , env->image_path_base ) ;
	INTER1( "*** ERROR : SnprintfAndMakeDir %s failed[%d] , errno[%d]\n" , image_rlayer_path_base , nret , errno )
	EIDTI( "mkdir %s ok\n" , image_rlayer_path_base )
	
	memset( current_path , 0x00 , sizeof(current_path) );
	getcwd( current_path , sizeof(current_path) );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cd %s/rlayer/ && tar --force-local -xzf %s/%s" , env->image_path_base , current_path , image_file ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "rm %s" , image_file ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [%s] failed[%d] , errno[%d]\n" , cmd , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

