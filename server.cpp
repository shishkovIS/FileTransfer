# include <iostream>
# include <fstream>
# include <iterator>
# include <vector>
# include <string>
#include <mysql.h>
# include "../headers/inetserverstream.hpp"
# include "../headers/exception.hpp"
# include <unistd.h>
# include <stdio.h>
# include <utility>
# include "../headers/socket.hpp"
# include "../headers/select.hpp"


int main(void)
{
    using std::string;

    using libsocket::inet_stream_server;
    using libsocket::inet_stream;
    using libsocket::selectset;
    MYSQL *connection, mysql;
    MYSQL_RES *result;
    MYSQL_ROW row;	
	

    mysql_init(&mysql);
    connection = mysql_real_connect(&mysql,"localhost","root","123","cloud",3306,0,0);
    if (connection == NULL)
		{ 
			std::cout << "Cannot connect to mysql";
			return 1;
		} 
	int query_state = mysql_query(connection,"select version(), current_date");
	if (query_state!=0)
	{
	    std::cout << "Query have been crashed";
		return 1;
	}
	result = mysql_store_result(connection);
	while ((row = mysql_fetch_row(result))!= NULL)
	{
	    std::cout << row[0] << " &" << row[1] << std::endl;
	}
	std::cout<< "Database connection succesful" << std::endl;
	
	//mysql_close(connection);
		
    string host = "10.0.1.3";
    string port = "1235";
    string answ;
    std::vector<char> buffer;
	
    try {
	inet_stream_server srv(host,port,LIBSOCKET_IPv4);
	inet_stream* cl1;

	selectset<inet_stream_server> set1;
	set1.add_fd(srv,LIBSOCKET_READ);

	for ( ;; )
	{
	    /********* SELECT PART **********/
	    std::cout << "Called select()\n";

	    libsocket::selectset<inet_stream_server>::ready_socks readypair; // Create pair (libsocket::fd_struct is the return type of selectset::wait()

	    readypair = set1.wait(); // Wait for a connection and save the pair to the var

	    inet_stream_server* ready_srv = dynamic_cast<inet_stream_server*>(readypair.first.back()); // Get the last fd of the LIBSOCKET_READ vector (.first) of the pair and cast the socket* to inet_stream_server*

	    readypair.first.pop_back(); // delete the fd from the pair

	    std::cout << "Ready for accepting\n";

	    /*******************************/

	    cl1 = ready_srv->accept();

	    *cl1 << "Hellohui\n";
		
		std::ofstream output("1.jpg",std::ios::binary);
	    answ.resize(512);
		bool endfile = false;
        while (!endfile)
		{
	       *cl1 >> answ;
		   if (answ.length()==0)
		   {
		       endfile = true;
		   } else
		   {
			   if (answ.length()!=0) std::cout << answ.length() << std::endl;
			   output << answ;
			//  *cl1 << "ok";
		   }
		
		
		}
		output.close();
	  //  std::cout << answ;

	    cl1->destroy();
	}

	srv.destroy();

    } catch (const libsocket::socket_exception& exc)
    {
	std::cerr << exc.mesg << std::endl;
    }
    return 0;
}
