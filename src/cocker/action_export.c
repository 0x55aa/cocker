/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "cocker_in.h"

int DoAction_export( struct CockerEnvironment *env )
{
	char		image_id[ IMAGES_ID_LEN_MAX + 1 ] ;
	char		*p2 = NULL ;
	char		version[ PATH_MAX + 1 ] ;
	char		image_file[ PATH_MAX + 1 ] ;
	char		current_path[ PATH_MAX + 1 ] ;
	char		cmd[ 4096 ] ;

	int		nret = 0 ;
	
	/* preprocess input parameters */
	Snprintf( image_id , sizeof(image_id) , "%s" , env->cmd_para.__image_id );
	p2 = strchr( image_id , ':' ) ;
	if( p2 == NULL )
	{
		strcpy( version , "_" );
	}
	else
	{
		strncpy( version , p2+1 , sizeof(version)-1 );
		(*p2) = '\0' ;
	}
	
	Snprintf( image_file , sizeof(image_file) , "%s%s%s.cockerimage" , image_id , (version[0]!='_'?version:"") , version );
	
	Snprintf( env->image_path_base , sizeof(env->image_path_base)-1 , "%s/%s/%s" , env->images_path_base , image_id , version );
	nret = access( env->image_path_base , F_OK ) ;
	I1TER1( "*** ERROR : image '%s' not found\n" , env->cmd_para.__image_id )
	
	/* pack image folders and files */
	memset( current_path , 0x00 , sizeof(current_path) );
	getcwd( current_path , sizeof(current_path) );
	
	nret = SnprintfAndSystem( cmd , sizeof(cmd) , "cd %s/rlayer/ && tar czf %s * && mv %s %s/" , env->image_path_base , image_file , image_file , current_path ) ;
	INTER1( "*** ERROR : SnprintfAndSystem [cd %s/rlayer/ && tar czf %s * && mv %s %s/] failed[%d] , errno[%d]\n" , env->image_path_base , image_file , image_file , current_path , nret , errno )
	EIDTI( "system [%s] ok\n" , cmd )
	
	printf( "OK\n" );
	
	return 0;
}

