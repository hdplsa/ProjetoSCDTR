#include "TWI.h"

static unsigned char twi_buf[TWI_BUFFER_SIZE];
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

/* Coloca os pull ups das portas 4 e 5 ON para não
 * precisar de pull ups externos                    */
void twi_turn_pullUp(){
  PORTC |= (1 << 5) | (1 << 4); // Mete os pull up
}

/* A bit rate é dada pela equação 
 * f = CPU_freq / (16 + 2*TWBR*Prescaler) */
void twi_set_rate(){

  // Faz set do TWBR
  TWBR = 152;

  // Faz set do Prescaler
  // TWPS | TWPS1 | TWPS0 | Prescaler
  //      |   0   |   0   |     1
  //      |   0   |   1   |     4
  //      |   1   |   0   |     16  
  //      |   1   |   1   |     64
  // Se o prescaler for mudado TODOS os códigos
  // são mudados

  TWSR |= (0 << TWPS1) | (0 << TWPS0);
  
}

/* Esta função retorna 1 se o TWI estiver enabled, ou seja
 * se estiverem a ser processados outros requests           */
bool twi_busy(){

  return ( TWCR & (1 << TWIE) ); 
  
}

int twi_set_SLA(uint8_t SLA){

  if((SLA & 0b10000000) != 0){
    return -1;
  }

  // Faz set do address e mete TWGCE a 1 (acknowledge address)
  TWAR = (SLA << 1) | (1 << TWGCE);

  return 0;
}

// Coloca o TWI à espera do seu slave address
void twi_set_slaveR(){

  TWCR = (1 << TWEA)  // Enable aknowledge
       | (1 << TWEN)  // TWI enable
       | (1 << TWIE); // Enable interrupção
  
}

int twi_send_msg(uint8_t SLA, char *msg, unsigned int msg_length){

  // Não pode enviar mensagens maiores do que o buffer
  if(msg_length + 1 > TWI_BUFFER_SIZE) return -1;

  // Espera que acabem as transações talvez seja melhor
  // Não bloquear o programa, simplesmente dizer -1
  //while(twi_busy()){
  //  Serial.print("Busy\n");
  //}

  // Este primeiro caracter corresponde ao SLA+W, por isso é que
  // fazemos OR 0, o +W é o bit mais À direita.
  twi_buf[0] = (SLA << 1) | 0b0;
  
  // Transfere a mensagem para o buffer
  for(unsigned int i = 1; i <= msg_length; i++){
    twi_buf[i] = msg[i-1];
  }

  twi_msg_size = msg_length+1;

  twi_ptr = 0;

  twi_send_start();

  return 0;
  
}

void twi_send_start(){
    twi_status = 1;

  // Envia o start

  TWCR  = (1 << TWINT) // Flag a 1
        | (1 << TWSTA) // Envia o start
        | (1 << TWEN)  // Enable
        | (1 << TWIE); // Enable interrupções

  Serial.print("SETSTART\n");
}

unsigned char* twi_data_received(){

  // Tem que dizer que aconteceu algo

  Serial.println((char*) twi_buf);

  return twi_buf;
  
}

ISR(TWI_vect){
  byte registo = TWSR;
//  Serial.print(registo, HEX);
//  Serial.print('\n');
  switch(registo){
    
    // Foi enviado o start e tem que ser enviado o SLA+R/W
    case TWI_START:
    case TWI_REP_START:
      Serial.print("START\n"); 
      switch(twi_status){
        // Temos que enviar o SLA+W
        case 1: 
          // Coloca no data register o SLA+W
          TWDR = twi_buf[0];
          // Envia o SLA+W
          TWCR = (1<<TWINT) | (1 << TWEN) | (1<<TWIE);
          // Coloca o ponteiro a mandar dados para a posição 1
          twi_ptr = 1;
          Serial.print("SLA+W: ");
          Serial.print(twi_buf[0],BIN);
          Serial.print('\n');
          //Serial.print("SLA+W SENT\n");
        break;
      }
      
    break;
    
    // Foi recebido um acknowledge do slave depois dum SLA+W
    case TWI_MTX_ADR_ACK:
    
      Serial.print("ACK\n");
      twi_ptr = 0;
    
    // Foi recebido um acknowledge do slave depois de mandar data
    case TWI_MTX_DATA_ACK:
    
    Serial.print("DATA\n");
      // Se ainda não tivermos chegado ao fim dos dados
      if(twi_ptr < twi_msg_size){
        // Coloca o próximo byte no registo
        TWDR = twi_buf[twi_ptr];
        // Envia o byte
        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
        // Incrementa a posição do ponteiro
        twi_ptr++;
        // Coloca o estado
        twi_status = 2;
      } else { // Se tivermos chegado ao fim
        TWCR = (1<<TWINT) // Flag a 1
              | (1<<TWSTO) // Envia STOP
              | (1<<TWIE);   // Enable interrupção
        // Após uma escrita bem sucedida, ficamos em modo de espera
        twi_status = 0;
      }
    break;

    // O slave lançou um NACK
    case TWI_MTX_ADR_NACK:
      twi_send_start();
    break;

    // Recebemos o SLA+W e enviàmos o ACK
    case TWI_SRX_ADR_ACK:
      twi_status = 3; // Estado de a receber
      twi_ptr = 0; // Começa-se a receber bytes no 0

      TWCR = (1 << TWINT)
            | (1 << TWEA)
            | (1 << TWEN)
            | (1 << TWIE);
      Serial.print("Recebi SLA+W\n");
    break;

    // Recebemos dados
    case TWI_SRX_ADR_DATA_ACK:
      if(twi_ptr < (TWI_BUFFER_SIZE - 1)){
        // Coloca os dados no buffer
        twi_buf[twi_ptr] = TWDR;

        // Avança uma posição na string
        twi_ptr++;

        // Avisa que os dados foram processados
        TWCR = (1<<TWINT) 
          | ( 1 << TWEA) 
          | (1 << TWEN)
          | (1 << TWIE);
      }
      Serial.print("Recebi ");
      Serial.print((char)TWDR, HEX); 
      Serial.print('\n');
    break;

    // Indica que já estão os dados todos
    case TWI_SRX_STOP_RESTART:
      Serial.print("STOP\n");  
      twi_buf[twi_ptr+1] = '\0';
      twi_ptr = 0;
      TWCR = (1 << TWINT) // Toma conhecimento do STOP
           | (1 << TWEA)  // Enable aknowledge
           | (1 << TWEN)  // TWI enable
           | (1 << TWIE); // Enable interrupção
      twi_data_received();
    break;
    
    default:
      Serial.print("Recebi: "); 
      Serial.print(TWSR, HEX); 
      Serial.print('\n');
      TWCR = (1 << TWINT) 
           | (1 << TWSTO) 
           | (1 << TWEN)
           | (1 << TWIE);
    break;

  }

}

