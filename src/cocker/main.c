#include "cocker_in.h"

static void usage()
{
	printf( "USAGE : cocker -s images\n" );
	printf( "               -s containers\n" );
	printf( "               -a create --image-id (image-id) [ --container-id (cid) ] [ --host-name (name) ] [ --net (BRIDGE|HOST|CUSTOM) ] [ --host-if-name (eth) ] [ --vip (ip) ]\n" );
	printf( "               -a start --container-id (cid) [ --attach ]\n" );
	printf( "               -a stop --container-id (cid) [ --forcely ]\n" );
	printf( "               -a destroy --container-id (cid) [ --forcely ]\n" );
	printf( "               -a install_test\n" );
	printf( "                    --debug\n" );
	return;
}

static int ParseCommandParameters( struct CockerEnvironment *cocker_env , int argc , char *argv[] )
{
	int		i ;
	
	int		nret = 0 ;
	
	for( i = 1 ; i < argc ; i++ )
	{
		if( STRCMP( argv[i] , == , "-v" ) )
		{
			printf( "cocker v%s build %s %s\n" , _COCKER_VERSION , __DATE__ , __TIME__ );
			DestroyCockerEnvironment( & cocker_env );
			exit(0);
		}
		else if( STRCMP( argv[i] , == , "-s" ) && i + 1 < argc )
		{
			cocker_env->cmd_para._show = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "-a" ) && i + 1 < argc )
		{
			cocker_env->cmd_para._action = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--image-id" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__image_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--container-id" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__container_id = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-name" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__host_name = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--net" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__net = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--host-if-name" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__host_if_name = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--vip" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__vip = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--port-mapping" ) && i + 1 < argc )
		{
			cocker_env->cmd_para.__port_mapping = argv[i+1] ;
			i++;
		}
		else if( STRCMP( argv[i] , == , "--attach" ) )
		{
			cocker_env->cmd_para.__attach = argv[i] ;
		}
		else if( STRCMP( argv[i] , == , "--debug" ) )
		{
			cocker_env->cmd_para.__debug = argv[i] ;
		}
		else if( STRCMP( argv[i] , == , "--forcely" ) )
		{
			cocker_env->cmd_para.__forcely = argv[i] ;
		}
		else
		{
			printf( "*** ERROR : invalid parameter '%s'\n" , argv[i] );
			usage();
			return -7;
		}
	}
	
	if( cocker_env->cmd_para.__net == NULL )
		cocker_env->cmd_para.__net = "BRIDGE" ;
	
	if( ! ( STRCMP( cocker_env->cmd_para.__net , == , "BRIDGE" ) || STRCMP( cocker_env->cmd_para.__net , == , "HOST" ) || STRCMP( cocker_env->cmd_para.__net , == , "CUSTOM" ) ) )
	{
		printf( "*** ERROR : '--net' value[%s] invalid\n" , cocker_env->cmd_para.__net );
		return -7;
	}
	
	memset( cocker_env->net , 0x00 , sizeof(cocker_env->net) );
	strncpy( cocker_env->net , cocker_env->cmd_para.__net , sizeof(cocker_env->net)-1 );
	
	if( cocker_env->cmd_para.__host_if_name )
	{
		memset( cocker_env->host_if_name , 0x00 , sizeof(cocker_env->host_if_name) );
		strncpy( cocker_env->host_if_name , cocker_env->cmd_para.__host_if_name , sizeof(cocker_env->host_if_name)-1 );
	}
	else
	{
		struct ifaddrs		*ifa_base = NULL ;
		struct ifaddrs		*ifa = NULL ;
		
		nret = getifaddrs( & ifa_base ) ;
		if( nret == -1 )
		{
			printf( "*** ERROR : getifaddrs failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
		
		memset( cocker_env->host_if_name , 0x00 , sizeof(cocker_env->host_if_name) );
		ifa = ifa_base ;
		while( ifa )
		{
			if( ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET )
			{
				if( STRCMP( ifa->ifa_name , != , "lo" ) && STRCMP( ifa->ifa_name , != , "cocker0" ) && STRNCMP( ifa->ifa_name , != , "veth" , 4 ) )
				{
					strncpy( cocker_env->host_if_name , ifa->ifa_name , sizeof(cocker_env->host_if_name)-1 );
					break;
				}
			}
			
			ifa = ifa->ifa_next ;
		}
		
		freeifaddrs( ifa_base );
		
		if( cocker_env->host_if_name[0] == '\0' )
		{
			printf( "*** ERROR : host if name not found\n" );
			return -1;
		}
	}
	
	return 0;
}

static int ExecuteCommandParameters( struct CockerEnvironment *cocker_env )
{
	if( cocker_env->cmd_para._show )
	{
		if( STRCMP( cocker_env->cmd_para._show , == , "images" ) )
		{
			return -DoShow_images( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._show , == , "containers" ) )
		{
			return -DoShow_containers( cocker_env );
		}
		else
		{
			printf( "*** ERROR : show[%s] invalid\n" , cocker_env->cmd_para._show );
			return -7;
		}
	}
	else if( cocker_env->cmd_para._action )
	{
		if( STRCMP( cocker_env->cmd_para._action , == , "create" ) )
		{
			if( cocker_env->cmd_para.__image_id == NULL || STRCMP( cocker_env->cmd_para.__image_id , == , "" ) )
			{
				printf( "*** ERROR : expect '--image-id' with action '-a create'\n" );
				return -7;
			}
			
			if( cocker_env->cmd_para.__net && STRCMP( cocker_env->cmd_para.__net , == , "BRIDGE" ) && cocker_env->cmd_para.__vip == NULL )
			{
				printf( "*** ERROR : expect '--vip' with action '-a create'\n" );
				return -7;
			}
			
			return -DoAction_create( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "start" ) )
		{
			if( cocker_env->cmd_para.__container_id == NULL || STRCMP( cocker_env->cmd_para.__container_id , == , "" ) )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a start'\n" );
				return -7;
			}
			
			return -DoAction_start( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "stop" ) )
		{
			if( cocker_env->cmd_para.__container_id == NULL || STRCMP( cocker_env->cmd_para.__container_id , == , "" ) )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a stop'\n" );
				return -7;
			}
			
			return -DoAction_stop( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "kill" ) )
		{
			if( cocker_env->cmd_para.__container_id == NULL || STRCMP( cocker_env->cmd_para.__container_id , == , "" ) )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a kill'\n" );
				return -7;
			}
			
			return -DoAction_kill( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "destroy" ) )
		{
			if( cocker_env->cmd_para.__container_id == NULL || STRCMP( cocker_env->cmd_para.__container_id , == , "" ) )
			{
				printf( "*** ERROR : expect '--container-id' with action '-a destroy'\n" );
				return -7;
			}
			
			return -DoAction_destroy( cocker_env );
		}
		else if( STRCMP( cocker_env->cmd_para._action , == , "install_test" ) )
		{
			return -DoAction_install_test( cocker_env );
		}
		else
		{
			printf( "*** ERROR : action[%s] invalid\n" , cocker_env->cmd_para._action );
			return -7;
		}
	}
	else
	{
		printf( "*** ERROR : cmd para action[%s] invalid\n" , cocker_env->cmd_para._action );
		return -7;
	}
}

int main( int argc , char *argv[] )
{
	struct CockerEnvironment	*cocker_env = NULL ;
	
	int				nret = 0 ;
	
	nret = CreateCockerEnvironment( & cocker_env ) ;
	if( nret )
	{
		printf( "*** ERROR : CreateCockerEnvironment failed[%d]\n" , nret );
		return -nret;
	}
	
	if( argc == 1 )
	{
		usage();
		return 0;
	}
	
	nret = ParseCommandParameters( cocker_env , argc , argv ) ;
	if( nret )
	{
		printf( "*** ERROR : ParseCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	nret = ExecuteCommandParameters( cocker_env ) ;
	DestroyCockerEnvironment( & cocker_env );
	if( nret )
	{
		printf( "*** ERROR : ExecuteCommandParameters failed[%d]\n" , nret );
		return -nret;
	}
	
	return 0;
}

