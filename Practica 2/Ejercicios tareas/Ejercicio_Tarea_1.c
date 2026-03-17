#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include "esp_timer.h"
#include "freertos/event_groups.h"


int botones[] = {21,19};
int leds[]={25,26,27};

QueueHandle_t handlerQueue;

int bandera = 0;
TaskHandle_t handleTarea1 = NULL;
TaskHandle_t handleTarea2 = NULL;

volatile uint64_t last_time = 0;

static void IRAM_ATTR gpio_interrupt_handler(void *args){
  int pinNumber = (int)args;
  uint64_t time = esp_timer_get_time() / 1000;
  if (time - last_time > 200){
    last_time = time;
    xQueueSendFromISR(handlerQueue, &pinNumber, NULL);

  }
}



void init_gpio(void){
    for(int i=0; i < 2; i++){
        gpio_reset_pin(botones[i]);
        gpio_set_direction(botones[i],GPIO_MODE_INPUT   );

        gpio_pulldown_en(botones[i]);
        gpio_pullup_dis(botones[i]);

        gpio_set_intr_type(botones[i],GPIO_INTR_POSEDGE);
        gpio_isr_handler_add(botones[i],gpio_interrupt_handler,(void *)i);
        
    }
    for(int i=0; i < 3 ; i++ ){
        gpio_reset_pin(leds[i]);
        gpio_set_direction(leds[i],GPIO_MODE_OUTPUT);
        gpio_set_level(leds[i],0);
    }
}

void tareaA(void *arg){
    while(1){
        if(bandera == 0){
        gpio_set_level(leds[0],0);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        gpio_set_level(leds[0],1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        }else{
            gpio_set_level(leds[0],0);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}

void tareaB(void *arg){

    int pinNumber;
    while(1){
      if (xQueueReceive(handlerQueue, &pinNumber, portMAX_DELAY)){
       vTaskDelay(20 / portTICK_PERIOD_MS);
       
      if(pinNumber == 0){
        if(bandera == 0){
            bandera = 1;
            printf("Sistema Pausado\n");
        }else{C
            bandera = 0;
            printf("Sistema Reanudado\n");
        }

      }
}
}
}


void app_main(void){

    handlerQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(tareaA, "tareaA", 2048, NULL, 10 , &handleTarea1);
    xTaskCreate(tareaB, "tareaB", 2048, NULL, 10 , &handleTarea2);

    gpio_install_isr_service(0);
    init_gpio();
}
