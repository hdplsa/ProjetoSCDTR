#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#define	BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
using namespace std;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket>   socket_ptr;


class session{

public:
    session(io_service& io);
    ip::tcp::socket& get_socket();
    void start_read();
    void Write(string send);
    ~session();

    void set_Readcallback(std::function<void(boost::system::error_code, string, session*)> fcn);
    void set_Writecallback(std::function<void(void)> fcn);
    void set_ReadErrorcallback(std::function<void(void)> fcn);
    void set_WriteErrorcallback(std::function<void(void)> fcn);

    // Funções
    void handle_read(const boost::system::error_code &ec);
    void handle_write(const boost::system::error_code &ec);

private:

    void check_deadline();

    // Callbacks
    std::function<void(const boost::system::error_code, string , session*)> onRead = NULL;
    std::function<void(void)> onWrite = NULL;

    // Callbacks para o erro
    std::function<void(void)> onReadError = NULL;
    std::function<void(void)> onWriteError = NULL;

    // Socket
    ip::tcp::socket socket_;

    // Timers para controlar os timeouts
    deadline_timer deadline;

    // buffer para enviar e receber dados
    boost::asio::streambuf buf;

};

#endif