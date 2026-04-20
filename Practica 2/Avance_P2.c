#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include "esp_timer.h"
#include "esp_random.h"

int botones[] = {21,19,22};
QueueHandle_t handlerQueue;
uint64_t time_aux = 0;

int carril_actual;
int aparicion_piedra = 29;
int carril_piedra;
int chocaste = 0;

static void IRAM_ATTR gpio_interrupt_handler(void *args){
  int pinNumber = (int)args;
  uint64_t time = esp_timer_get_time() / 1000;
  if (time - time_aux > 200){
    time_aux = time;
    xQueueSendFromISR(handlerQueue, &pinNumber, NULL);
  }
}
void init_gpio(void){
  for(int i = 0; i < 3 ; i++){
    gpio_reset_pin(botones[i]);
    gpio_set_direction(botones[i],GPIO_MODE_INPUT);
    
    gpio_pulldown_en(botones[i]);
    gpio_pullup_dis(botones[i]);

    gpio_set_intr_type(botones[i],GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(botones[i],gpio_interrupt_handler,(void *)i);
  }
}
void movimiento_carro(void *params){
  int pinNumber;
  while(true){
    if(xQueueReceive(handlerQueue, &pinNumber, portMAX_DELAY)){

      //0 ARRIBA 1 MEDIO 2 ABAJO
      //Si tenemos carril = 1 lo que debemos hacer es lo siguiente
      //Si boton[0] esta siendo presionad y 1 > 0,lo que vamos hacer es decrementar
      //0 y el carril donde estaria seria el de arriba

      //Si boton[1] esta siendo presiondo y 1 < 2 entonces el carril cambia a 2
      //el carril en donde estaria el carro seria hasta abajo
      switch(pinNumber){
        case 0:
        if (carril_actual > 0){
          carril_actual--;
        }
        break;

        case 1:
        if (carril_actual < 2){
          carril_actual++;
        }
        break;
      
        case 2:
        printf("HASTA LUEGO\n");
        chocaste = 1;
        vTaskDelete(NULL);
        break;
    }
  }
}
}
void dibujar_pista(void *params){
  while(chocaste == 0){
  printf ("|------------------------------|\n");
  for(int i = 0; i < 3;i++){//anchura de la pista
    printf ("|");
  for(int j= 0; j < 30; j++){//largo de la pista
  //si i = 1 y j = 2,posicion (1,2) se va a imprimir CAR y a J se le sumara dos espacios mas
  //por las letras AR
    if ( i == carril_actual && j == 2){
      printf("CAR");
      j +=2;
    }else if(i == carril_piedra && j == aparicion_piedra){
      printf("0");
    
    }else{
    printf(" ");
  }
  }
  printf("|\n");
  }
  printf ("|------------------------------|\n\n");

// Si piedra llega a la posicion 2(en donde normalmente esta el carro) y tambien si la piedra esta en el mismo carril que el carro
if (aparicion_piedra == 2 && carril_piedra == carril_actual) {
    printf("SUERTE PARA LA PROXIMA\n");
    chocaste = 1;
    vTaskDelete(NULL);

  }

  aparicion_piedra--;
  if(aparicion_piedra < 0){
    aparicion_piedra = 29;
    carril_piedra = esp_random() % 3;
  }
  vTaskDelay(175 / portTICK_PERIOD_MS);
}
    vTaskDelete(NULL);

}

void app_main(){
  handlerQueue = xQueueCreate(10, sizeof(int));
  
  carril_actual = esp_random() % 3;

  xTaskCreate(dibujar_pista,"dibujar_pista",2048,NULL,1,NULL);
  xTaskCreate(movimiento_carro,"movimiento_carro",2048,NULL,2,NULL);

  gpio_install_isr_service(0);
  init_gpio();
}