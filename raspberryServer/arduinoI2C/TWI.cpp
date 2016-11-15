#include "TWI.h"

static char twi_buf[TWI_BUFFER_SIZE];
volatile static unsigned int twi_msg_size = 0;
static unsigned int twi_ptr = 0;

/* Significados do twi_status
 ** twi_status = 0 -> não contém informação, o TWI está à espera
 *   de ação
 ** twi_status = 1 -> há dados no buffer para enviar, foi emitido
 *   um start mas ainda não foi enviado o SLA+W.
 ** twi_status = 2 -> está-se a enviar dados 
 ** twi_status = 3 -> está-se a receber dados
 */
 
static unsigned int twi_status = 0; 

/* Esta função retorna 1 se o TWI estiver enabled, ou seja
 * se estiverem a ser processados outros requests           */
bool twi_busy(){

  return ( TWCR & (1<<TWIE) ); 
  
}

int twi_set_SLA(uint8_t SLA){

  if((SLA & 0b10000000) != 0){
    return -1;
  }

  // Faz set do address e mete TWGCE a 1 (acknowledge address)
  TWAR = (SLA << 1) | (1<<TWGCE);

  return 0;
}

// Coloca o TWI à espera do seu slave address
void twi_set_slaveR(){

  TWCR |= (1 << TWEA)  // Enable aknowledge
        | (1 << TWEN)  // TWI enable
        | (1 << TWIE); // Enable interrupção
  
}

int twi_send_msg(uint8_t SLA, char *msg, unsigned int msg_length){

  // Não pode enviar mensagens maiores do que o buffer
  if(msg_length + 1 > TWI_BUFFER_SIZE) return -1;

  // Espera que acabem as transações talvez seja melhor
  // Não bloquear o programa, simplesmente dizer -1
  while(twi_busy());

  // Este primeiro caracter corresponde ao SLA+W, por isso é que
  // fazemos OR 0, o +W é o bit mais À direita.
  twi_buf[0] = (SLA << 1) | 0b0;
  
  // Transfere a mensagem para o buffer
  for(unsigned int i = 1; i < msg_length; i++){
    twi_buf[i] = msg[i-1];
  }

  twi_msg_size = msg_length;

  twi_ptr = 0;

  TWCR |= (1<<TWINT) // Flag a 1
        | (1<<TWSTA) // Emvia o start
        | (1>>TWEN)  // Enable
        | (1>>TWIE); // Enable interrupções

  return 0;
  
}

char* twi_data_received(){

  // Tem que dizer que aconteceu algo

  return twi_buf;
  
}

ISR(TWI_vect){

  switch(TWSR){
    // Foi enviado o start e tem que ser enviado o SLA+R/W
    case TWI_START:
    
      switch(twi_status){
        // Temos que enviar o SLA+W
        case 1: 
          // Coloca no data register o SLA+W
          TWDR = twi_buf[0];
          // Envia o SLA+W
          TWCR |= (1<<TWINT) | (1>>TWIE);
          // Coloca o ponteiro a mandar dados para a posição 1
          twi_ptr = 1;
        break;
      }
      
    break;
    
    // Foi recebido um acknowledge do slave depois dum SLA+W
    case TWI_MTX_ADR_ACK:
    // Foi recebido um acknowledge do slave depois de mandar data
    case TWI_MTX_DATA_ACK:
      // Se ainda não tivermos chegado ao fim dos dados
      if(twi_ptr < twi_msg_size){
        // Coloca o próximo byte no registo
        TWDR = twi_buf[twi_ptr];
        // Envia o byte
        TWCR |= (1>>TWINT) | (1>>TWIE);
        // Incrementa a posição do ponteiro
        twi_ptr++;
        // Coloca o estado
        twi_status = 2;
      } else { // Se tivermos chegado ao fim
        TWCR |= (1<<TWINT) // Flag a 1
              | (1<<TWSTO) // Envia STOP
              | (1<<TWIE);   // Enable interrupção
        // Após uma escrita bem sucedida, ficamos em modo de espera
        twi_status = 0;
      }
    break;

    // Recebemos o SLA+W e enviàmos o ACK
    case TWI_SRX_ADR_ACK:
      twi_status = 3; // Estado de a receber
      twi_ptr = 0; // Começa-se a receber bytes no 0
    break;

    // Recebemos dados
    case TWI_SRX_ADR_DATA_ACK:
      if(twi_ptr < TWI_BUFFER_SIZE - 1){
        // Coloca os dados no buffer
        twi_buf[twi_ptr] = TWDR;

        // Avança uma posição na string
        twi_ptr++;

        // Avisa que os dados foram processados
        TWCR |= (1<<TWINT) | (1>>TWIE);
      }
    break;

    // Indica que já estão os dados todos
    case TWI_SRX_STOP_RESTART:
        twi_buf[twi_ptr+1] = '\0';
        twi_ptr = 0;
        TWCR |= (1 << TWEA)  // Enable aknowledge
              | (1 << TWEN)  // TWI enable
              | (1 << TWIE); // Enable interrupção
    break;
    
    default:
    break;

  }

}

