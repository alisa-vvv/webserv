#include "Http.hpp"
#include <map>


// class Listener
// {
// 	private:
// 		int							_port;
// 		uint32_t					_ip_addr;
// 		int							_listenerFd;	// the server/listening socket
// 		struct sockaddr_in			_address;		// address of the socket

// 	public:
// 		int								_port;
// 		uint32_t						_ip_addr;
// 		int								_listenerFd;	// the server/listening socket
// 		struct sockaddr_in				_address;		// address of the socket
// 		std::vector<const cfg_server_t *>	_serverConfigs;
		
// 		public:
// 		Listener();
// 		~Listener();

// 		eListenerError	setup(void);
// 		eListenerError	_createSocket(void);
// 		eListenerError	_setSocketOptions();
// 		eListenerError	_setNonBlocking(int fd);
// 		eListenerError	_setAddress();
// 		eListenerError	_bindSocket(void);
// 		eListenerError	_listenSocket(void);
// 		eListenerError		setup(void);
// 		eListenerError		_createSocket(void);
// 		eListenerError		_setSocketOptions();
// 		eListenerError		_setNonBlocking(int fd);
// 		eListenerError		_setAddress();
// 		eListenerError		_bindSocket(void);
// 		eListenerError		_listenSocket(void);

// 		void			setPort(int port);
// 		int				getPort() const;
// 		void				setPort(int port);
// 		int					getPort() const;

// 		void			setIpAddr(uint32_t ip_addr);
// 		uint32_t		getIpAddr() const;
// 		void				setIpAddr(uint32_t ip_addr);
// 		uint32_t			getIpAddr() const;

// 		int				getListenerFd() const;
// 		void			setListenerFd(int fd);
// 		int					getListenerFd() const;
// 		void				setListenerFd(int fd);

// 		const cfg_server_t	*getServerConfig(int i);
// 		void				setServerConfig(const cfg_server_t *config);

// 		void closeSocket();
// 		void				closeSocket();

// 		//test
// 		void			printPortNumber();
//}

/// @brief this function sets references to the matching configuration of the current client. it also rewrites the uri
void Http::setRequestContext()
{
	/*===HOST, LOCATION population===*/
	//listener.port , host, ip
	//first compare the host -> if there is no server name, then use the first one as default	
	//check first for the listener.port
	//compare to config file
	//check for auto index


	
	/*============ALLOWED METHODS===============*/
	//check the map, iterator ;
	// for (const auto& pair : cfg.location.allowed_methods)
	// {
	// 	if (GET == TRUE)
	//	get == true;
	// }	
	//ticket04 ->check for server.port.allowed_methods
}
