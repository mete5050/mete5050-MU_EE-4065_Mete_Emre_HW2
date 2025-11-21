/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ALF.h"  
#include <stdio.h>
#include <string.h>
#include <stdint.h> 
#include <math.h>  
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern UART_HandleTypeDef huart3;   // <<< UART3
#define THR 64
#define IMG_SIZE (IMG_W * IMG_H)



/*-------------------- UART SEND ---------------------------*/
static void uart3_send(const uint8_t *data, uint32_t len)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)data, len, HAL_MAX_DELAY);
}
void uart_write(const char *msg)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

static void send_image_buffer(const uint8_t *buf)
{
    char hdr[64];
    int n = snprintf(hdr, sizeof(hdr), "P5\n%d %d\n255\n", IMG_W, IMG_H);
    uart3_send((uint8_t*)hdr, n);
    uart3_send(buf, IMG_SIZE);
}

/*============================================================
=                       Q1 - HISTOGRAM                        =
============================================================*/

static void img_histogram(const uint8_t *src, uint32_t *hist)
{
    for (int i = 0; i < 256; i++)
        hist[i] = 0;

    for (uint32_t i = 0; i < IMG_SIZE; i++)
        hist[src[i]]++;
}

static void img_histogram_and_send(const uint8_t *src)
{
    uint32_t hist[256] = {0};

    
    for (uint32_t i = 0; i < IMG_SIZE; i++)
        hist[src[i]]++;

    uart_write("Histogram : \n");
    uart_write("Histogram : \n");
    uart_write("Histogram : \n");
    char msg[64];
    for (int i = 0; i < 256; i++) {
    	if(hist[i]!=0){
        int n = snprintf(msg, sizeof(msg), "H[%03d] = %lu\r\n", i, hist[i]);
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, n, HAL_MAX_DELAY);
    	}
    }
}

/*============================================================
=                 Q3 - 2D CONVOLUTION KERNELS                =
============================================================*/

static const int8_t kernel_low[3][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};

static const int8_t kernel_high[3][3] = {
    {-1, -1, -1},
    {-1,  8, -1},
    {-1, -1, -1}
};

static void img_conv3x3(const uint8_t *src, uint8_t *dst,
                        const int8_t kernel[3][3], int div)
{
    for (int y = 1; y < IMG_H - 1; y++)
    {
        for (int x = 1; x < IMG_W - 1; x++)
        {
            int sum = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    uint8_t pix = src[(y + ky) * IMG_W + (x + kx)];
                    sum += pix * kernel[ky + 1][kx + 1];
                }
            }

            sum /= div;
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            dst[y * IMG_W + x] = (uint8_t)sum;
        }
    }
}

/*============================================================
=                Q4 - MEDIAN FILTER (3x3)                    =
============================================================*/

static uint8_t median9(uint8_t *v)
{
    for (int i = 0; i < 9; i++)
        for (int j = i + 1; j < 9; j++)
            if (v[j] < v[i]) {
                uint8_t t = v[i];
                v[i] = v[j];
                v[j] = t;
            }
    return v[4];
}

static void img_median3x3(const uint8_t *src, uint8_t *dst)
{
    uint8_t w[9];

    for (int y = 1; y < IMG_H - 1; y++)
    {
        for (int x = 1; x < IMG_W - 1; x++)
        {
            int k = 0;
            for (int ky = -1; ky <= 1; ky++)
                for (int kx = -1; kx <= 1; kx++)
                    w[k++] = src[(y + ky) * IMG_W + (x + kx)];

            dst[y * IMG_W + x] = median9(w);
        }
    }
}



static void img_histeq(const uint8_t *src, uint8_t *dst)
{
    uint32_t hist[256] = {0};
    uint32_t cdf[256]  = {0};
    uint8_t  lut[256];
    int total = IMG_SIZE;

    // Histogram
    for (int i = 0; i < total; i++)
        hist[src[i]]++;

    // CDF
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++)
        cdf[i] = cdf[i - 1] + hist[i];

    // CDF min (first non-zero)
    uint32_t cdf_min = 0;
    for (int i = 0; i < 256; i++)
        if (cdf[i] != 0) { cdf_min = cdf[i]; break; }

    // LUT (robust version)
        for (int i = 0; i < 256; i++) {
            float v = (float)(cdf[i] - cdf_min) / (float)(total - cdf_min);
            if (v < 0) v = 0;
            if (v > 1) v = 1;


            lut[i] = (uint8_t)(v * 255.0f + 0.5f);
        }

    // Apply LUT
    for (int i = 0; i < total; i++)
        dst[i] = lut[src[i]];
}

static void uart3_send_histogram_ascii(const uint32_t *hist)
{
    char msg[64];
    uart_write("Histogram  Equalization : \n");
    uart_write("Histogram  Equalization : \n");
    uart_write("Histogram  Equalization : \n");
    for (int i = 0; i < 256; i++) {
    	if(hist[i]!=0){
        int n = snprintf(msg, sizeof(msg), "H[%03d] = %lu\r\n", i, hist[i]);
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, n, HAL_MAX_DELAY);
    	}
    }
}


/*============================================================
=                 OUTPUT ORDER                  =
============================================================*/

extern const uint8_t g_img[IMG_SIZE];  

void run_image_pipeline(void)
{
    static uint8_t buf[IMG_SIZE];
    static uint32_t hist[256];

    // 1. Orijinal Görüntünün Histogramını Gönder
    img_histogram_and_send(g_img);
    HAL_Delay(2000);

    // 2. Eşitleme İşlemini Gerçekleştir (g_img -> buf)
    img_histeq(g_img, buf);
    uart_write("Histogram equalization finished.\n");

    // 3. Eşitlenmiş Görüntüyü Gönder
    send_image_buffer(buf);
    uart_write("Histogram equalized image sent.\n");

    // 4. Eşitlenmiş Görüntünün Histogramını Hesapla (buf -> hist)
    img_histogram(buf, hist);

    // 5. Equalized Histogramı ASCII olarak gönder
    uart3_send_histogram_ascii(hist);

    HAL_Delay(3000);


    img_conv3x3(g_img, buf, kernel_low, 9);
    send_image_buffer(buf);
    uart_write("Low-pass filtered image sent.\n");
    HAL_Delay(3000);

    img_conv3x3(g_img, buf, kernel_high, 1);
    send_image_buffer(buf);
    uart_write("High-pass filtered image sent.\n");
    HAL_Delay(3000);


    img_median3x3(g_img, buf);
    send_image_buffer(buf);
    uart_write("Median filtered image sent.\n");
    HAL_Delay(3000);

}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  HAL_Delay(3000);
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

     // static uint8_t buf[IMG_SIZE];
  /* USER CODE END 2 */
while(1){
	
	run_image_pipeline();


	    }  // 2 saniye bekle
    /* USER CODE END WHILE */
}
    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */
  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */
  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
