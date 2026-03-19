#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

//Escriba un programa que crea dos tareas, denominadas A y B. 
//La tarea A parpadea un LED dos veces por segundo. 
//La tarea B lee el estado de un botón y cada vez que el usuario 
//lo presiona suspende o despierta a la tarea A. Es decir, 
//si la tarea A estaba despierta, la suspende. 
//Si estaba suspendida, la despierta.

QueueHandle_t HandlerQueue;

TaskHandle_t handlertarea1 = NULL;
TaskHandle_t handlertarea2 = NULL;

volatile int bandera;

static void IRAM_ATTR gpio_interrupt_handler(void *args){
  if (bandera == 0){
    bandera = 1;
    int pinNumber = (int)args;
    xQueueSendFromISR(HandlerQueue,&pinNumber,NULL);
  }
}

void init_gpio(){
  //leds
  gpio_reset_pin(GPIO_NUM_21);
  gpio_set_direction(GPIO_NUM_21,GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_21,0);

  //botones
  gpio_reset_pin(GPIO_NUM_19);
  gpio_set_direction(GPIO_NUM_19,GPIO_MODE_INPUT);

  //pullup
  gpio_pullup_dis(GPIO_NUM_19);
  gpio_pulldown_en(GPIO_NUM_19);

  gpio_set_intr_type(GPIO_NUM_19,GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(GPIO_NUM_19,gpio_interrupt_handler,(void *)GPIO_NUM_19);
}

void tareaA(void *args){
  while(1){
    gpio_set_level(GPIO_NUM_21,0);
    vTaskDelay (500 / portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_21,1);
    vTaskDelay (500 / portTICK_PERIOD_MS);
  }
}

void tareaB(void *args){
  int pinNumber;
  int suspender = 0;

  while(1){
    if (xQueueReceive(HandlerQueue, &pinNumber, portMAX_DELAY)){
      if(suspender == 0){
        vTaskSuspend(handlertarea1);
        gpio_set_level(GPIO_NUM_21,0);
        suspender = 1;
      }else{
        vTaskResume(handlertarea1);
        suspender = 0;
      }
    vTaskDelay(200 / portTICK_PERIOD_MS);
    bandera = 0;
    }
  }
}

void app_main(){
  gpio_install_isr_service(0);
  init_gpio();

  HandlerQueue = xQueueCreate(10,sizeof(int));

  xTaskCreate(tareaA,"tareaA",2048,NULL,10,&handlertarea1);
  xTaskCreate(tareaB,"tareaB",2048,NULL,10,&handlertarea2);

}
