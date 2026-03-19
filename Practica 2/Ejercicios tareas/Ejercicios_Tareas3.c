#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "driver/gpio.h"
#include "freertos/event_groups.h"

//3. Escriba un programa que crea cuatro tareas, denominadas A, B, C y D. La tarea A se
//encarga de parpadear tres LEDs, los LED representan a las tareas B, C y D. 
//La tarea A sabe qué LED debe parpadear en base al estado de un Event group. Si el BIT0 del
//Event group está activo, parpadea el LED 1, si está activo el BIT1, parpadea el LED 2 y
//si el BIT2 está activo, parpadea el LED 3.
//– La tarea B se ejecuta cada medio segundo, al ejecutarse activa el BIT0 del Event
//group.
//– La tarea C se ejecuta cada segundo, al ejecutarse activa el BIT1 del Event
//group.
//– La tarea D se ejecuta tres segundo, al ejecutarse activa el BIT2 del Event group.

EventGroupHandle_t event_group;

int leds[] = {21,19,18};

void init_gpio(){
  for(int i = 0; i < 3;i++){
    gpio_reset_pin(leds[i]);
    gpio_set_direction(leds[i],GPIO_MODE_OUTPUT);
    gpio_set_level(leds[i],0);
  }
  
}

void TareaA(void *args){
while(1){
  if(xEventGroupGetBits(event_group) & BIT0){
    gpio_set_level(leds[0],1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(leds[0],0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xEventGroupClearBits(event_group, BIT0);

  }
  if(xEventGroupGetBits(event_group) & BIT1){
    gpio_set_level(leds[1],1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(leds[1],0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xEventGroupClearBits(event_group, BIT1);

  }
  
  if(xEventGroupGetBits(event_group) & BIT2){
    gpio_set_level(leds[2],1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(leds[2],0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    xEventGroupClearBits(event_group, BIT2);

  }
  vTaskDelay (20 / portTICK_PERIOD_MS);

}
}

void TareaB(void *args){
  while(1){
  vTaskDelay(500/ portTICK_PERIOD_MS);
  xEventGroupSetBits(event_group, BIT0);
}
}

void TareaC(void *args){
  while(1){
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xEventGroupSetBits(event_group, BIT1);
  }
}

void TareaD(void *args){
  while(1){
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  xEventGroupSetBits(event_group, BIT2);
  }
}

void app_main(){
  init_gpio();
  event_group = xEventGroupCreate();

  xTaskCreate(TareaA,"TareaA",2048,NULL,10,NULL);
  xTaskCreate(TareaB,"TareaB",2048,NULL,5,NULL);
  xTaskCreate(TareaC,"TareaC",2048,NULL,5,NULL);
  xTaskCreate(TareaD,"TareaD",2048,NULL,5,NULL);


}

