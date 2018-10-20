#include "cocker_in.h"

int DoShow_containers( struct CockerEnvironment *cocker_env )
{
	DIR		*dir = NULL ;
	struct dirent	*dirent = NULL ;
	int		count ;
	
	char		container_images_file[ PATH_MAX + 1 ] ;
	char		images[ 4096 ] ;
	char		container_hostname_file[ PATH_MAX + 1 ] ;
	char		hostname[ HOST_NAME_MAX ] ;
	char		container_net_file[ PATH_MAX + 1 ] ;
	char		net[ NET_LEN_MAX ] ;
	char		container_netns_file[ PATH_MAX + 1 ] ;
	char		netns[ NETNS_NAME_LEN_MAX ] ;
	char		container_pid_file[ PATH_MAX + 1 ] ;
	char		pid_str[ PID_LEN_MAX ] ;
	pid_t		pid ;
	char		status[ 40 ] ;
	
	int		nret = 0 ;
	
	dir = opendir( cocker_env->containers_path_base ) ;
	count = 0 ;
	while(1)
	{
		dirent = readdir( dir ) ;
		if( dirent == NULL )
			break;
		
		if( STRCMP( dirent->d_name , == , "." ) || STRCMP( dirent->d_name , == , ".." ) )
			continue;
		if( dirent->d_type != DT_DIR )
			continue;
		
		memset( container_images_file , 0x00 , sizeof(container_images_file) );
		memset( images , 0x00 , sizeof(images) );
		nret = ReadFileLine( images , sizeof(images) , container_images_file , sizeof(container_images_file) , "%s/%s/images" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine images failed[%d]\n" , nret );
			memset( container_images_file , 0x00 , sizeof(container_images_file) );
			memset( images , 0x00 , sizeof(images) );
		}
		TrimEnter( images );
		
		memset( container_hostname_file , 0x00 , sizeof(container_hostname_file) );
		memset( hostname , 0x00 , sizeof(hostname) );
		nret = ReadFileLine( hostname , sizeof(hostname) , container_hostname_file , sizeof(container_hostname_file) , "%s/%s/hostname" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine hostname failed[%d]\n" , nret );
			memset( container_hostname_file , 0x00 , sizeof(container_hostname_file) );
			memset( hostname , 0x00 , sizeof(hostname) );
		}
		TrimEnter( hostname );
		
		memset( container_net_file , 0x00 , sizeof(container_net_file) );
		memset( net , 0x00 , sizeof(net) );
		nret = ReadFileLine( net , sizeof(net) , container_net_file , sizeof(container_net_file) , "%s/%s/net" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine net failed[%d]\n" , nret );
			memset( container_net_file , 0x00 , sizeof(container_net_file) );
			memset( net , 0x00 , sizeof(net) );
		}
		TrimEnter( net );
		
		memset( container_netns_file , 0x00 , sizeof(container_netns_file) );
		memset( netns , 0x00 , sizeof(netns) );
		nret = ReadFileLine( netns , sizeof(netns) , container_netns_file , sizeof(container_netns_file) , "%s/%s/netns" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			printf( "*** ERROR : ReadFileLine netns failed[%d]\n" , nret );
			memset( container_netns_file , 0x00 , sizeof(container_netns_file) );
			memset( netns , 0x00 , sizeof(netns) );
		}
		TrimEnter( netns );
		
		memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
		memset( pid_str , 0x00 , sizeof(pid_str) );
		nret = ReadFileLine( pid_str , sizeof(pid_str) , container_pid_file , sizeof(container_pid_file) , "%s/%s/pid" , cocker_env->containers_path_base , dirent->d_name ) ;
		if( nret )
		{
			memset( pid_str , 0x00 , sizeof(pid_str) );
			memset( container_pid_file , 0x00 , sizeof(container_pid_file) );
			if( nret > 0 )
			{
				Snprintf( status , sizeof(status) , "STOPED" );
			}
			else
			{
				Snprintf( status , sizeof(status) , "EXPECTION" );
			}
		}
		else
		{
			TrimEnter( pid_str );
			pid = atoi(pid_str) ;
			nret = kill( pid , 0 ) ;
			if( nret == -1 )
			{
				Snprintf( status , sizeof(status) , "EXPECTION(%s)" , pid_str );
			}
			else
			{
				Snprintf( status , sizeof(status) , "RUNNING(%s)" , pid_str );
			}
		}
		
		if( count == 0 )
		{
			printf( "%-20s %-10s %-10s %-10s %-26s %s\n" , "container_id" , "images" , "hostname" , "net" , "netns" , "status" );
			printf( "---------------------------------------------------------------------------------------\n" );
		}
		
		printf( "%-20s %-10s %-10s %-10s %-26s %s\n" , dirent->d_name , images , hostname , net , netns , status );
		
		count++;
	}
	
	return 0;
}

