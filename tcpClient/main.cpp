#include <iostream>
#include <unistd.h>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "client.h"
using namespace std;

int main(){

    boost::asio::io_service io_service;
    tcp::resolver r(io_service);
    client c(io_service);

    c.start(r.resolve(tcp::resolver::query("127.0.0.1","4444")));

    boost::thread th = boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    c.start_send(string("s 0 0\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("s 1 0\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("s 0 1\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("s 1 1\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("s 0 0\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    c.start_send(string("s 1 0\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("s 0 1\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    c.start_send(string("s 1 1\n"));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
    c.start_send(string("m\n"));

    return 0;

}