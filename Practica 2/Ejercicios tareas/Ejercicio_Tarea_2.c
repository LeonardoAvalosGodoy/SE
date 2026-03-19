#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


//Escriba un programa que crea dos tareas, denominadas A y B. La tarea A parpadea un
//LED a la velocidad indicada por la tarea B. La tarea B le envía la velocidad en
//milisegundos por medio de una cola. La velocidad inicial que envía la tarea B es 100
//ms. La tarea B lee el estado de un botón y cada vez que el usuario lo presiona
//decrementa la velocidad en 100 ms. Por ejemplo, si el usuario presiona una vez el
//botón, la velocidad de parpadeo pasa de 100 a 200 ms.

QueueHandle_t HandlerQueue;

TaskHandle_t Handlertarea1;
TaskHandle_t Handlertarea2;

volatile int bandera;

int velocidad = 100;

static void IRAM_ATTR gpio_interrupt_handler(void *args){
  if(bandera == 0){
    bandera = 1;
    int pinNumber = (int)args;
    xQueueSendFromISR(HandlerQueue, &pinNumber, NULL);
  }
}

void init_gpio(){
  //led
  gpio_reset_pin(GPIO_NUM_21);
  gpio_set_direction(GPIO_NUM_21,GPIO_MODE_OUTPUT);
  gpio_set_level(GPIO_NUM_21,0);

  //boton
  gpio_reset_pin(GPIO_NUM_19);
  gpio_set_direction(GPIO_NUM_19,GPIO_MODE_INPUT);

  gpio_pullup_dis(GPIO_NUM_19);
  gpio_pulldown_en(GPIO_NUM_19);

  gpio_set_intr_type(GPIO_NUM_19,GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(GPIO_NUM_19,gpio_interrupt_handler,(void *)GPIO_NUM_19);
}

void tareaA(void *args){
  while(1){
  gpio_set_level(GPIO_NUM_21,0);
  vTaskDelay (velocidad / portTICK_PERIOD_MS);
  gpio_set_level(GPIO_NUM_21,1);
  vTaskDelay ( velocidad / portTICK_PERIOD_MS);
}
}

void tareaB(void *args){
  int pinNumber;

  while(1){
    if(xQueueReceive(HandlerQueue, &pinNumber, portMAX_DELAY)){
    velocidad = velocidad + 100;

  
    if(velocidad >= 2000){
        printf("Reinciando velocidad\n");
        velocidad = 100;
      }
    printf("%d\n",velocidad);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    bandera = 0;
    }
  }
}


void app_main(){
  gpio_install_isr_service(0);
  init_gpio();

  HandlerQueue = xQueueCreate(10,sizeof(int));

  xTaskCreate(tareaA,"tareaA",2048,NULL,10,&Handlertarea1);
  xTaskCreate(tareaB,"tareaB",2048,NULL,10,&Handlertarea2);

}
