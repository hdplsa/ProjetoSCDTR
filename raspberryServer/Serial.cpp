#include "Serial.h"

/* Construtor, inicia o objeto da porta serial do boost
 */
Serial::Serial() : arduino(io)
{

    //arduino = new boost::asio::serial_port(io);
}

/* Função que faz setup da serial port e a abre
 *  Input:
 * baudrate -> baudrate igual à do arduino
 * port     -> "COMX" ou "ttyX" onde está ligado o arduino
 *  Retorna:
 * 0  -> setup bem feito
 * -1 -> setup falhou
 */
int Serial::Begin(long baudrate, const char *port)
{

    arduino.open(port, error); //connect to port

    // Error handling
    if (error)
    {
        cout << "Error" << endl;
        return -1;
    }

    //set baud rate
    arduino.set_option(boost::asio::serial_port_base::baud_rate(baudrate), error);
    if (error)
    {
        cout << "Error" << endl;
        return -1;
    }

    return 0;
}

/* Faz set da função de callback de leitura
 */
void Serial::set_Readcallback(std::function<void(string)> fcn)
{

    this->onRead = fcn;
}

/* Faz set da função de callback de escrita
 */
void Serial::set_Writecallback(std::function<void(void)> fcn)
{

    this->onWrite = fcn;
}

/* Faz set da função de callback de erro de leitura
 */
void Serial::set_ReadErrorcallback(std::function<void(void)> fcn)
{

    this->onReadError = fcn;
}

/* Faz set da função de callback de erro de escrita
 */
void Serial::set_WriteErrorcallback(std::function<void(void)> fcn)
{

    this->onWriteError = fcn;
}

/* Função qua inicia a leitura do serial até um endl
 *  Erros possiveis:
 * "Porta fechada": se o arduino tiver sido close()
 */
void Serial::Read_ln()
{

    // Se a porta estiver aberta
    if (arduino.is_open())
    {

        async_read_until(arduino, buf, '\n',
                         boost::bind(
                             &Serial::read_complete,
                             this, boost::asio::placeholders::error,
                             boost::asio::placeholders::bytes_transferred));

        t = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
        working = true;
    }
    else
    {
        cout << "Closed connection" << endl;
        throw std::runtime_error("Porta fechada");
    }
}

/* Função qua inicia a escrita de uma string para o arduino
 *  Input:
 * send -> string a enviar ao arduino
 *  Erros possiveis:
 * "Porta fechada": se o arduino tiver sido close()
 */
void Serial::Write(std::string send)
{

    if (arduino.is_open())
    {

        // Escreve os dados no buffer
        ostream os(&buf);
        os.write(send.c_str(), send.length());

        async_write(arduino, buf,
                    boost::bind(
                        &Serial::write_complete,
                        this, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

        t = boost::thread(boost::bind(&boost::asio::io_service::run, &io));
        working = true;
    }
    else
    {
        cout << "Closed connection" << endl;
        throw std::runtime_error("Porta fechada");
    }
}
/* Função iniciada assim que se recebe a string
 *  Input:
 * e    -> erro do async_read_until()
 * size -> tamanho da resposta
 *  Chama:
 * Chama a função de callback definida na classe
 */
void Serial::read_complete(const boost::system::error_code &e, std::size_t size)
{

    working = false;

    // Verifica se houve erro
    if (!e)
    {
        // Reescreve os dados do buffer na string data
        istream is(&buf);
        string data(size, '\0');
        is.read(&data[0], size);

        std::cout << "Received data:" << data;

        if (onRead != NULL)
        {
            onRead(data);
        }
    }
    else
    {
        // Se ocorrer erro, é executado o callback de erro
        if (onReadError != NULL)
        {
            onReadError();
        }
    }
}

/* Função chamada assim que é enviada a string
 *  Input:
 * e    -> erro do async_write()
 * size -> tamanho da string enviada (?)
 *  Chama:
 * Chama a função de callback definida na classe
 */
void Serial::write_complete(const boost::system::error_code &e, std::size_t size)
{

    working = false;

    // Se não tiver ocorrido executa normalmente o callback
    if (!e)
    {

        cout << "Imprimi" << endl;

        if (onWrite != NULL)
        {
            onWrite();
        }
    }
    else
    {
        // Caso tenha acontecido um erro, executa o callback de erro
        if (onWriteError != NULL)
        {
            onWriteError();
        }
    }
}

/* Função que fecha a porta do arduino
 */
void Serial::Close()
{

    arduino.close();
}
/* Deconstrutor
 */
Serial::~Serial()
{

    arduino.close();

    // Não é preciso apagar o arduino, porque é um smart pointer
}