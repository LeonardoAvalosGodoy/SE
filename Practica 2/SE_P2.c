#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include "esp_timer.h"
#include "esp_random.h"

int botones[] = {21, 19, 22};
int leds[] = {14,23,25,26,27};

QueueHandle_t handlerQueue;
uint64_t time_aux = 0;

int carril_actual;
int aparicion_piedra[3];
int carril_piedra[3];

volatile int chocaste = 0;
int segundos = 0;

static void IRAM_ATTR gpio_interrupt_handler(void *args){
  int pinNumber = (int)args;
  uint64_t time = esp_timer_get_time() / 1000;
  if (time - time_aux > 250){
    time_aux = time;
    xQueueSendFromISR(handlerQueue, &pinNumber, NULL);
  }
}

void init_gpio(void){
  for(int i = 0; i < 3 ; i++){
    gpio_reset_pin(botones[i]);
    gpio_set_direction(botones[i], GPIO_MODE_INPUT);
    gpio_pulldown_en(botones[i]);
    gpio_pullup_dis(botones[i]);
    gpio_set_intr_type(botones[i], GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(botones[i], gpio_interrupt_handler, (void *)i);
  }
  for(int i = 0; i < 5; i++){
    gpio_reset_pin(leds[i]);
    gpio_set_direction(leds[i], GPIO_MODE_OUTPUT);
    gpio_set_level(leds[i], 0);
  }
}

void dibujar_leds(int valor){
  for(int i = 0 ; i < 5 ; i++){
    gpio_set_level(leds[i], valor % 2);
    valor = valor / 2;
  }
}

void movimiento_carro(void *params){
  carril_actual = esp_random() % 3;
  int pinNumber;
  while(true){
    if(xQueueReceive(handlerQueue, &pinNumber, portMAX_DELAY)){
      switch(pinNumber){
        case 0:
          if (carril_actual > 0) carril_actual--;
          break;
        case 1:
          if (carril_actual < 2) carril_actual++;
          break;
        case 2:
          printf("HASTA LUEGO\n");
          printf("TIEMPO QUE ESTUVISTE: %d s\n", segundos);
          chocaste = 1;
          dibujar_leds(segundos);
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          vTaskDelete(NULL);
          break;
      }
    }
  }
}

void tiempo(void *params){
  while(chocaste == 0){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    segundos++;
  }
  vTaskDelete(NULL);
}

void dibujar_pista(void *params){
  while(chocaste == 0){
    printf("\033[2J\033[H");
    printf("\tTiempo %d segundos\n\n", segundos);
    printf("|--------------------------------------------------|\n");
    
    for(int i = 0; i < 3; i++){ 
      printf("|");
      for(int j = 0; j < 50; j++){ 
        
        int dibujado = 0; 

        if (i == carril_actual && j == 2){
          printf("CAR");
          j += 2; 
          dibujado = 1;
        } 
        else {
          for(int k = 0; k < 3; k++){
            if(i == carril_piedra[k] && j == aparicion_piedra[k] && aparicion_piedra[k] <= 49){
              printf("0");
              dibujado = 1;
              break; 
            }
          }
        }
        
        if(dibujado == 0){
          printf(" ");
        }
      }
      printf("|\n");
    }
    printf("|--------------------------------------------------|\n");

    for(int k = 0; k < 3; k++){
      if (aparicion_piedra[k] > 2 && aparicion_piedra[k] < 5 && carril_piedra[k] == carril_actual) {
        printf("SUERTE PARA LA PROXIMA\n");
        printf("TIEMPO QUE ESTUVISTE: %d s\n", segundos);
        chocaste = 1;

        dibujar_leds(segundos);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
      }
    }
    
    vTaskDelay(75 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void mover_piedra(void *params){
  aparicion_piedra[0] = 49 + (esp_random() % 5); 
  carril_piedra[0] = esp_random() % 3;

  aparicion_piedra[1] = aparicion_piedra[0] + 15 + (esp_random() % 15);
  carril_piedra[1] = esp_random() % 3;

  aparicion_piedra[2] = aparicion_piedra[1] + 15 + (esp_random() % 15);
  carril_piedra[2] = esp_random() % 3;
  
  while(chocaste == 0){
    
    for(int k = 0; k < 3; k++){
      aparicion_piedra[k]--;
      
      if(aparicion_piedra[k] < 0){
        aparicion_piedra[k] = 49 + 10 + (esp_random() % 31);
        carril_piedra[k] = esp_random() % 3;
      }
    }
    vTaskDelay(75 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void app_main(){
  handlerQueue = xQueueCreate(10, sizeof(int));
  
  xTaskCreatePinnedToCore(dibujar_pista, "dibujar_pista", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(movimiento_carro, "movimiento_carro", 2048, NULL, 2, NULL, 1);
  
  xTaskCreatePinnedToCore(tiempo, "tiempo", 2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(mover_piedra, "mover_piedra", 2048, NULL, 4, NULL, 0);

  gpio_install_isr_service(0);
  init_gpio();
}