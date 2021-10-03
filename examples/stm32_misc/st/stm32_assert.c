
/**
 ******************************************************************************
 * @file    stm32_assert.c
 * @brief   STM32 assert file.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

#if defined(USE_FULL_ASSERT)

/* Private includes ----------------------------------------------------------*/
#include "stm32_assert.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /*
   * User can add his own implementation to report the file name and line number,
   * ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
   */

}
#endif /* defined(USE_FULL_ASSERT) */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
