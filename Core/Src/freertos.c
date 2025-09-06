/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwip/udp.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/**
  * @brief  UDP受信コールバック関数（エコーバック用）
  * @param  arg: ユーザー引数 (今回は未使用)
  * @param  upcb: 受信したPCB
  * @param  p: 受信したデータを含むpbuf
  * @param  addr: 送信元のIPアドレス
  * @param  port: 送信元のポート番号
  * @retval None
  */
void udp_echo_recv_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  LWIP_UNUSED_ARG(arg); // 未使用引数の警告を抑制

  // 受信データがあれば、そのまま送り返す
  if (p != NULL)
  {
    // H7シリーズ: DMAが書き込んだ受信データをCPUが正しく読むために、キャッシュを無効化
    SCB_InvalidateDCache_by_Addr((uint32_t*)p->payload, p->len);

    // 受信したデータを送信元にそのまま送り返す
    udp_sendto(upcb, p, addr, port);

    // 受信したpbufのメモリを解放（非常に重要）
    pbuf_free(p);
  }
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  struct udp_pcb *upcb;
  err_t err;
  ip_addr_t dest_addr;
  const u16_t dest_port = 5000;
  const u16_t local_port = 5000;
  static uint32_t counter = 1; // メッセージ用のカウンター変数

  // 宛先PCのIPアドレスを設定
  IP4_ADDR(&dest_addr, 192, 168, 11, 2);

  // 新しいUDP PCBを作成
  upcb = udp_new();

  if (upcb != NULL)
  {
    // ローカルポートにバインド
    err = udp_bind(upcb, IP_ADDR_ANY, local_port);

    if (err == ERR_OK)
    {
      // 受信コールバック関数を登録する
      // これで、どの相手からでもデータを受信できるようになる
      udp_recv(upcb, udp_echo_recv_callback, NULL);
    }
    else
    {
      // バインドに失敗した場合はPCBを削除
      udp_remove(upcb);
      upcb = NULL;
    }
  }

  /* Infinite loop */
  for(;;)
  {
    if (upcb != NULL)
    {
      // 送信するメッセージを格納するバッファ
      char msg[64];
      // sprintfを使ってカウンター付きのメッセージを生成
      int msg_len = sprintf(msg, "Hello from STM32! %lu\r\n", counter);

      struct pbuf *p;

      // pbufを確保 (メッセージの実際の長さに合わせる)
      p = pbuf_alloc(PBUF_TRANSPORT, msg_len, PBUF_RAM);
      if (p != NULL)
      {
        // pbufにメッセージをコピー
        pbuf_take(p, msg, msg_len);

        // H7シリーズ: CPUが書き込んだ送信データをDMAが正しく読むために、キャッシュをクリーン（フラッシュ）
        SCB_CleanDCache_by_Addr((uint32_t*)p->payload, p->len);

        // udp_send の代わりに udp_sendto を使い、毎回宛先を指定する
        udp_sendto(upcb, p, &dest_addr, dest_port);
        // pbufを解放
        pbuf_free(p);
      }
    }
    // カウンターをインクリメント
    counter++;

    // 1秒待機
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
