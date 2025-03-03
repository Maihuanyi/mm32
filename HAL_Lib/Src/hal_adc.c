/***********************************************************************************************************************
    @file     hal_adc.c
    @author   VV TEAM
    @brief    THIS FILE PROVIDES ALL THE ADC FIRMWARE FUNCTIONS.
  **********************************************************************************************************************
    @attention

    <h2><center>&copy; Copyright(c) <2023> <MindMotion></center></h2>

      Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
    following conditions are met:
    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
       the following disclaimer in the documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
       promote products derived from this software without specific prior written permission.

      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *********************************************************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#define _HAL_ADC_C_

/* Files includes ------------------------------------------------------------*/
#include "hal_adc.h"

/** @addtogroup MM32_Hardware_Abstract_Layer
  * @{
  */

/** @addtogroup ADC_HAL
  * @{
  */

/** @addtogroup ADC_Exported_Functions
  * @{
  */

/**
  * @brief  Deinitializes the adc peripheral registers to their default
  *         reset values.
  * @param  adc: select the ADC peripheral.
  * @retval None.
  */
void ADC_DeInit(ADC_TypeDef *adc)
{
    if (adc == ADC1)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
    }
    else if (adc == ADC2)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, DISABLE);
    }
}

/**
  * @brief  Configure the ADC calibration clock before ADC initialization.
  * @param  adc: select the ADC peripheral.
  * @param  precal: PRECAL must be even, ranging from 0 to 0x1FF.
  * @retval None.
  */
void ADC_CalibrationConfig(ADC_TypeDef *adc, uint16_t precal)
{
    adc->ADDATA     = 0;
    adc->ADSTA_EXT &= ~((0x01U << ADC_ADSTA_EXT_EOCALIF_Pos) | (0x01U << ADC_ADSTA_EXT_FREOCIF_Pos));

    if (adc == ADC1)
    {
        MODIFY_REG(RCC->ADC1CFGR, RCC_ADC1CFGR_PRECAL_Msk, precal << RCC_ADC1CFGR_PRECAL_Pos);
    }
    else if (adc == ADC2)
    {
        MODIFY_REG(RCC->ADC2CFGR, RCC_ADC2CFGR_PRECAL_Msk, precal << RCC_ADC2CFGR_PRECAL_Pos);
    }

    MODIFY_REG(adc->ANY_CR, ADC_ANY_CR_ADCAL_Msk, 0x01 << ADC_ANY_CR_ADCAL_Pos);
    MODIFY_REG(adc->ADCFG, ADC_ADCFG_ADEN_Msk, 0x01U << ADC_ADCFG_ADEN_Pos);

    while ((adc->ADSTA_EXT & ADC_ADSTA_EXT_EOCALIF_Msk) == 0)
    {
        __ASM("nop");                  /* __NOP(); */
    }

    /* @note Clears the adc's calibration pending bits. */
    adc->ADSTA_EXT |= ADC_ADSTA_EXT_EOCALIF_Msk;
}

/**
  * @brief  Initializes the adc peripheral according to the specified parameters
  *         in the init_struct, Please use this function if you want to be
  *         compatible with older versions of the library.
  * @param  adc: select the ADC peripheral.
  * @param  init_struct: pointer to an ADC_InitTypeDef structure that contains
  *         the configuration information for the specified ADC peripheral.
  * @retval None.
  */
void ADC_Init(ADC_TypeDef *adc, ADC_InitTypeDef *init_struct)
{
    MODIFY_REG(adc->ADCFG, ADC_ADCFG_RSLTCTL_Msk, init_struct->ADC_Resolution);
    MODIFY_REG(adc->ADCR, ADC_ADCR_ALIGN_Msk, init_struct->ADC_DataAlign);
    MODIFY_REG(adc->ADCR, ADC_ADCR_ADMD_Msk, init_struct->ADC_Mode);

    if (adc == ADC1)
    {
        MODIFY_REG(RCC->ADC1CFGR, RCC_ADC1CFGR_PRE_Msk, init_struct->ADC_Prescaler);
    }
    else if (adc == ADC2)
    {
        MODIFY_REG(RCC->ADC2CFGR, RCC_ADC2CFGR_PRE_Msk, init_struct->ADC_Prescaler);
    }
}

/**
  * @brief  Fills each init_struct member with its default value.
  * @param  init_struct : pointer to an ADC_InitTypeDef structure which will be
  *         initialized.
  * @retval None.
  */
void ADC_StructInit(ADC_InitTypeDef *init_struct)
{
    init_struct->ADC_Resolution         = ADC_Resolution_12b;
    init_struct->ADC_Prescaler          = ADC_Prescaler_2;
    init_struct->ADC_Mode               = ADC_Mode_Imm; /*!< ADC_Mode_Single; */
    init_struct->ADC_DataAlign          = ADC_DataAlign_Right;
}

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  adc:select the ADC peripheral.
  * @param  state: new state of the adc peripheral.
  * @retval None.
  */
void ADC_Cmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                       \
    (adc->ADCFG |= (0x01U << ADC_ADCFG_ADEN_Pos)) : \
    (adc->ADCFG &= ~(0x01U << ADC_ADCFG_ADEN_Pos));
}

/**
  * @brief  Enables or disables the specified ADC DMA request.
  * @param  adc: select the ADC peripheral.
  * @param  state: New state of the selected ADC DMA transfer.
  * @retval None.
  */
void ADC_DMACmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                      \
    (adc->ADCR |= (0x01U << ADC_ADCR_DMAEN_Pos)) : \
    (adc->ADCR &= ~(0x01U << ADC_ADCR_DMAEN_Pos));
}

/**
  * @brief  Enables or disables the selected ADC software start conversion .
  * @param  adc:  select the ADC peripheral.
  * @param  state: New state of the selected ADC software start conversion.
  * @retval None.
  */
void ADC_SoftwareStartConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                     \
    (adc->ADCR |= (0x01U << ADC_ADCR_ADST_Pos)) : \
    (adc->ADCR &= ~(0x01U << ADC_ADCR_ADST_Pos));
}

/**
  * @brief  Gets the selected ADC Software start conversion Status.
  * @param  adc: select the ADC peripheral.
  * @retval The new state of ADC software start conversion (SET or RESET).
  */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *adc)
{
    return (((adc->ADCR & (0x01U << ADC_ADCR_ADST_Pos)) != (uint32_t)RESET) ? SET : RESET);
}

/**
  * @brief  Enable the selected ADC channel and configure its sample time.
  * @param  adc:  select the ADC peripheral.
  * @param  channel: the ADC channel to configure. the valaue may ADC_Channel_0 ADC_Channel_1 ... ADC_Channel_TempSensor etc.
  * @param  sample_time: the ADC Channel n Sample time to configure.
  *         This parameter can be one of the following values:
  * @arg   ADC_SampleTime_3_5
  * @arg   ADC_SampleTime_4_5
  * @arg   ADC_SampleTime_5_5
  * @arg   ADC_SampleTime_6_5
  * @arg   ADC_SampleTime_7_5
  * @arg   ADC_SampleTime_11_5
  * @arg   ADC_SampleTime_13_5
  * @arg   ADC_SampleTime_15_5
  * @arg   ADC_SampleTime_19_5
  * @arg   ADC_SampleTime_29_5
  * @arg   ADC_SampleTime_39_5
  * @arg   ADC_SampleTime_59_5
  * @arg   ADC_SampleTime_79_5
  * @arg   ADC_SampleTime_119_5
  * @arg   ADC_SampleTime_159_5
  * @arg   ADC_SampleTime_240_5
  * @retval   None.
  */
void ADC_SampleTimeConfig(ADC_TypeDef *adc, uint8_t channel, uint8_t sample_time)
{
    if (channel > 15)
    {
        channel = channel - 16;
        MODIFY_REG(adc->SMPR3, (0x0F << (channel * 4)), (sample_time << (channel * 4)));
    }
    else if ((channel > 7) && (channel <= 15))
    {
        channel = channel - 8;
        MODIFY_REG(adc->SMPR2, (0x0F << (channel * 4)), (sample_time << (channel * 4)));
    }
    else
    {
        MODIFY_REG(adc->SMPR1, (0x0F << (channel * 4)), (sample_time << (channel * 4)));
    }
}

/**
  * @brief  Enables or disables the ADC channel.
  * @param  adc:select the ADC peripheral.
  * @param  channel: the ADC channel to configure.the valaue may ADC_Channel_0 ADC_Channel_1 ... ADC_Channel_TempSensor etc.
  * @param  state: new state of the adc peripheral.
  * @retval None.
  */
void ADC_ChannelCmd(ADC_TypeDef *adc, uint8_t channel, FunctionalState state)
{
    (state) ?                            \
    (adc->ADCHS |= (0x01U << channel)) : \
    (adc->ADCHS &= ~(0x01U << channel));
}

/**
  * @brief  Enables or disables the adc conversion through external trigger.
  * @param  adc: select the ADC peripheral.
  * @param  state: New state of the selected ADC external trigger.
  * @retval None.
  */
void ADC_ExternalTrigConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                      \
    (adc->ADCR |= (0x01U << ADC_ADCR_TRGEN_Pos)) : \
    (adc->ADCR &= ~(0x01U << ADC_ADCR_TRGEN_Pos));
}

/**
  * @brief  Configures the adc external trigger source for channels conversion.
  * @param  adc:  select the ADC peripheral.
  * @param  trig_edge:
  *     @arg    ADC_ExtTrig_Edge_Up
  * @param  shift: trigger after this vlaue
  *     @arg    ADC_ExtTrig_Shift_0
  *     @arg    ADC_ExtTrig_Shift_4
  *     @arg    ADC_ExtTrig_Shift_16
  *     @arg    ADC_ExtTrig_Shift_32
  *     @arg    ADC_ExtTrig_Shift_64
  *     @arg    ADC_ExtTrig_Shift_128
  *     @arg    ADC_ExtTrig_Shift_256
  *     @arg    ADC_ExtTrig_Shift_512
  * @retval None.
  */
void ADC_ExternalTrigSourceConfig(ADC_TypeDef *adc, uint32_t trig_edge, uint32_t shift)
{
    MODIFY_REG(adc->ADCR, ADC_ADCR_TRGSHIFT_Msk, shift);
    MODIFY_REG(adc->ADCR, ADC_ADCR_TRG_EDGE_Msk, trig_edge);
}

/**
  * @brief  Returns the last adc conversion result data for regular channel.
  * @param  adc: select the ADC peripheral.
  * @retval The data conversion value.
  */
uint16_t ADC_GetConversionValue(ADC_TypeDef *adc)
{
    return ((uint16_t)adc->ADDATA);
}

/**
  * @brief  Get channel convertion result.
  * @param  adc  :   select the ADC peripheral.
  * @param  channel : the valaue may ADC_Channel_0, ... , ADC_Channel_15 etc.
  * @retval The Data conversion value.
  */
uint16_t ADC_GetChannelConvertedValue(ADC_TypeDef *adc, uint8_t channel)
{
    return ((uint16_t)(*(__IO uint32_t *)((uint32_t)adc + 0x18 + ((uint32_t)channel << 2))));
}

/**
  * @brief  Enables or disables the analog watchdog.
  * @param  adc:  to select the ADC peripheral.
  * @param  state: New state of the selected ADC analog watchdog.
  * @retval None.
  */
void ADC_AnalogWatchdogCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?
    (adc->ADCFG |= (0x01U << ADC_ADCFG_AWDEN_Pos)) : \
    (adc->ADCFG &= ~(0x01U << ADC_ADCFG_AWDEN_Pos));
}

/**
  * @brief  Configures the high and low thresholds of the analog watchdog.
  * @param  adc:  select the ADC peripheral.
  * @param  high_threshold: the ADC analog watchdog High threshold value.
  *         This parameter must be a 12bit value.
  * @param  low_threshold: the ADC analog watchdog Low threshold value.
  *         This parameter must be a 12bit value.
  * @retval None.
  */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *adc, uint16_t high_threshold, uint16_t low_threshold)
{
    MODIFY_REG(adc->ADCMPR, ADC_ADCMPR_CMPHDATA_Msk, (high_threshold << ADC_ADCMPR_CMPHDATA_Pos));
    MODIFY_REG(adc->ADCMPR, ADC_ADCMPR_CMPLDATA_Msk, (low_threshold << ADC_ADCMPR_CMPLDATA_Pos));
}

/**
  * @brief  Configures the analog watchdog guarded single channel
  * @param  adc: select the ADC peripheral.
  * @param  channel: the ADC channel to configure for the analog watchdog.
  *         the valaue may ADC_Channel_0, ADC_Channel_1 ...  etc.
  * @retval None.
  */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *adc, uint8_t channel)
{
    uint32_t chl_val, chh_val;

    chl_val = (channel & 0x0F) << ADC_ADCR_CMPCHL_Pos;
    chh_val = (channel >> 4) << ADC_ADCR_CMPCHH_Pos;
    MODIFY_REG(adc->ADCR, ADC_ADCR_CMPCHH_Msk | ADC_ADCR_CMPCHL_Msk, chh_val | chl_val);
}

/**
  * @brief  Enables or disables the temperature sensor .
  * @param  state: New state of the temperature sensor.
  * @retval None.
  */
void ADC_TempSensorCmd(FunctionalState state)
{
    (state) ?                                        \
    (ADC2->ADCFG |= (0x01U << ADC_ADCFG_TSEN_Pos)) : \
    (ADC2->ADCFG &= ~(0x01U << ADC_ADCFG_TSEN_Pos));
}

/**
  * @brief  Enables or disables the VrefSensor channel.
  * @param  state: New state of the VrefSensor channel.
  * @retval None.
  */
void ADC_VrefSensorCmd(FunctionalState state)
{
    (state) ?                                        \
    (ADC2->ADCFG |= (0x01U << ADC_ADCFG_VSEN_Pos)) : \
    (ADC2->ADCFG &= ~(0x01U << ADC_ADCFG_VSEN_Pos));
}

/**
  * @brief  configure anychannel from channel 0 to channel 15 as an input channel in the operation of any channel mode.
  * @param  adc:  select the ADC peripheral.
  * @param  anychannel: ADC_AnyChannel_x , where x may 0..15
  * @param  channel: ADC_Channel_x , where x may 0..18
  * @retval None.
  */
void ADC_AnyChannelSelect(ADC_TypeDef *adc, uint8_t anychannel, uint8_t channel)
{
    uint8_t chany_sell, chany_selh;

    chany_selh = channel >> 4;
    chany_sell = channel & 0x0F;

    MODIFY_REG(adc->ANY_CFG, (0x01 << (anychannel + 8)), (chany_selh << (anychannel + 8)));

    if (anychannel > 7)
    {
        anychannel = anychannel - 8;
        MODIFY_REG(adc->CHANY1, (0x0F << (anychannel * 4)), (chany_sell << (anychannel * 4)));
    }
    else
    {
        MODIFY_REG(adc->CHANY0, (0x0F << (anychannel * 4)), (chany_sell << (anychannel * 4)));
    }
}

/**
  * @brief  Configures the adc any channels conversion Max  number
  * @param  adc: select the ADC peripheral.
  * @param  length: Configuring the max number for the ADC AnyChannel.
  * @retval None.
  */
void ADC_AnyChannelNumCfg(ADC_TypeDef *adc, uint8_t num)
{
    MODIFY_REG(adc->ANY_CFG, ADC_ANY_CFG_CHANY_NUM_Msk, (num << ADC_ANY_CFG_CHANY_NUM_Pos));
}

/**
  * @brief  Enables or disables the any channel converter.
  * @param  state: enable or disable the ANY channel converter mode.
  * @retval None.
  */
void ADC_AnyChannelCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                               \
    (adc->ANY_CR |= (0x01U << ADC_ANY_CR_CHANY_MDEN_Pos)) : \
    (adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_CHANY_MDEN_Pos));
}

/**
  * @brief  Get any channel convertion result.
  * @param  adc  :   select the ADC peripheral.
  * @param  channel : the valaue may ADC_AnyChannel_0, ... , ADC_AnyChannel_15 etc.
  * @retval The Data conversion value.
  */
uint16_t ADC_GetAnyChannelConvertedValue(ADC_TypeDef *adc, uint8_t anychannel)
{
    return ((uint16_t)(*(__IO uint32_t *)((uint32_t)adc + 0x18 + ((uint32_t)anychannel << 2))));
}

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  adc: select the ADC peripheral.
  * @param  adc_interrupt: specifies the ADC interrupt sources to be enabled or disabled.
  *         This parameter can be one of the following values:
  * @arg   ADC_IT_AWD
  * @arg   ADC_IT_EOS
  * @arg   ADC_IT_EOC
  * @arg   ADC_IT_EOSMP
  * @arg   ADC_IT_INJEOS
  * @arg   ADC_IT_INJEOC
  * @arg   ADC_IT_INJEOSMP
  * @param  state: New state of the specified ADC interrupts.
  * @retval None.
  */
void ADC_ITConfig(ADC_TypeDef *adc, uint32_t adc_interrupt, FunctionalState state)
{
    if (state)
    {
        if (ADC_IT_EOS == adc_interrupt)
        {
            adc->ADCR |= (0x01U << ADC_ADCR_EOSIE_Pos);
        }
        else if (ADC_IT_AWD == adc_interrupt)
        {
            adc->ADCR |= (0x01U << ADC_ADCR_AWDIE_Pos);
        }
        else if (ADC_IT_EOC == adc_interrupt)
        {
            adc->ADCR |= (0X01U << ADC_ADCR_EOCIE_Pos);
        }
        else if (ADC_IT_EOSMP == adc_interrupt)
        {
            adc->ADCR |= (0X01U << ADC_ADCR_EOSMPIE_Pos);
        }
        else if (ADC_IT_INJEOS == adc_interrupt)
        {
            adc->ANY_CR |= (0x01U << ADC_ANY_CR_JEOSIE_Pos);
        }
        else if (ADC_IT_INJEOC == adc_interrupt)
        {
            adc->ANY_CR |= (0x01U << ADC_ANY_CR_JEOCIE_Pos);
        }
        else if (ADC_IT_INJEOSMP == adc_interrupt)
        {
            adc->ANY_CR |= (0x01U << ADC_ANY_CR_JEOSMPIE_Pos);
        }
    }
    else
    {
        if (ADC_IT_EOS == adc_interrupt)
        {
            adc->ADCR &= ~(0x01U << ADC_ADCR_EOSIE_Pos);
        }
        else if (ADC_IT_AWD == adc_interrupt)
        {
            adc->ADCR &= ~(0x01U << ADC_ADCR_AWDIE_Pos);
        }
        else if (ADC_IT_EOC == adc_interrupt)
        {
            adc->ADCR &= ~(0X01U << ADC_ADCR_EOCIE_Pos);
        }
        else if (ADC_IT_EOSMP == adc_interrupt)
        {
            adc->ADCR &= ~(0X01U << ADC_ADCR_EOSMPIE_Pos);
        }
        else if (ADC_IT_INJEOS == adc_interrupt)
        {
            adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JEOSIE_Pos);
        }
        else if (ADC_IT_INJEOC == adc_interrupt)
        {
            adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JEOCIE_Pos);
        }
        else if (ADC_IT_INJEOSMP == adc_interrupt)
        {
            adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JEOSMPIE_Pos);
        }
    }
}

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  adc: select the ADC peripheral.
  * @param  adc_flag: specifies the flag to check.
  * @arg    ADC_FLAG_AWD
  * @arg    ADC_FLAG_EOS
  * @arg    ADC_FLAG_EOC
  * @arg    ADC_FLAG_EOSMP
  * @arg    ADC_FLAG_INJEOS
  * @arg    ADC_FLAG_INJEOC
  * @arg    ADC_FLAG_INJEOSMP
  * @retval The New state of adc_flag (SET or RESET).
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *adc, uint32_t adc_flag)
{
    FlagStatus ret = RESET;

    if (ADC_FLAG_EOS == adc_flag)
    {
        ret = ((adc->ADSTA & ADC_ADSTA_EOSIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_AWD == adc_flag)
    {
        ret = ((adc->ADSTA & ADC_ADSTA_AWDIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_EOC == adc_flag)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_EOCIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_EOSMP == adc_flag)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_EOSMPIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_INJEOS == adc_flag)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOSIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_INJEOC == adc_flag)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOCIF_Msk) ? SET : RESET);
    }
    else if (ADC_FLAG_INJEOSMP == adc_flag)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOSMPIF_Msk) ? SET : RESET);
    }

    return (ret);
}

/**
  * @brief  Clears the adc's pending flags.
  * @param  adc: select the ADC peripheral.
  * @param  adc_flag: specifies the flag to clear.
  * @arg    ADC_FLAG_AWD
  * @arg    ADC_FLAG_EOS
  * @arg    ADC_FLAG_EOC
  * @arg    ADC_FLAG_EOSMP
  * @arg    ADC_FLAG_INJEOS
  * @arg    ADC_FLAG_INJEOC
  * @arg    ADC_FLAG_INJEOSMP
  * @retval None.
  */
void ADC_ClearFlag(ADC_TypeDef *adc, uint32_t adc_flag)
{
    if (ADC_FLAG_EOS == adc_flag)
    {
        adc->ADSTA = (0x01U << ADC_ADSTA_EOSIF_Pos);
    }
    else if (ADC_FLAG_AWD == adc_flag)
    {
        adc->ADSTA = (0x01U << ADC_ADSTA_AWDIF_Pos);
    }
    else if (ADC_FLAG_EOC == adc_flag)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_EOCIF_Pos);
    }
    else if (ADC_FLAG_EOSMP == adc_flag)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_EOSMPIF_Pos);
    }
    else if (ADC_FLAG_INJEOC == adc_flag)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOCIF_Pos);
    }
    else if (ADC_FLAG_INJEOSMP == adc_flag)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOSMPIF_Pos);
    }
    else if (ADC_FLAG_INJEOS == adc_flag)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOSIF_Pos);
    }
}

/**
  * @brief  Checks whether the specified adc's interrupt has occurred or not.
  * @param  adc: select the ADC peripheral.
  * @param  adc_interrupt: specifies the ADC interrupt source to check.
  * @retval The new state of adc_interrupt (SET or RESET).
  */
ITStatus ADC_GetITStatus(ADC_TypeDef *adc, uint32_t adc_interrupt)
{
    ITStatus ret = RESET;

    if (ADC_IT_EOS == adc_interrupt)
    {
        ret = ((adc->ADSTA & ADC_ADSTA_EOSIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_AWD == adc_interrupt)
    {
        ret = ((adc->ADSTA & ADC_ADSTA_AWDIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_EOC == adc_interrupt)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_EOCIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_EOSMP == adc_interrupt)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_EOSMPIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_INJEOS == adc_interrupt)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOSIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_INJEOC == adc_interrupt)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOCIF_Msk) ? SET : RESET);
    }
    else if (ADC_IT_INJEOSMP == adc_interrupt)
    {
        ret = ((adc->ADSTA_EXT & ADC_ADSTA_EXT_JEOSMPIF_Msk) ? SET : RESET);
    }

    return (ret);
}

/**
  * @brief  Clears the adc's interrupt pending bits.
  * @param  adc: select the ADC peripheral.
  * @param  adc_interrupt: specifies the ADC interrupt pending bit to clear.
  * @arg   ADC_IT_AWD
  * @arg   ADC_IT_EOS
  * @arg   ADC_IT_EOC
  * @arg   ADC_IT_EOSMP
  * @arg   ADC_IT_INJEOS
  * @arg   ADC_IT_INJEOC
  * @arg   ADC_IT_INJEOSMP
  * @retval None.
  */
void ADC_ClearITPendingBit(ADC_TypeDef *adc, uint32_t adc_interrupt)
{
    if (ADC_IT_EOS == adc_interrupt)
    {
        adc->ADSTA = (0x01U << ADC_ADSTA_EOSIF_Pos);
    }
    else if (ADC_IT_AWD == adc_interrupt)
    {
        adc->ADSTA = (0x01U << ADC_ADSTA_AWDIF_Pos);
    }
    else if (ADC_IT_EOC == adc_interrupt)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_EOCIF_Pos);
    }
    else if (ADC_IT_EOSMP == adc_interrupt)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_EOSMPIF_Pos);
    }
    else if (ADC_IT_INJEOC == adc_interrupt)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOCIF_Pos);
    }
    else if (ADC_IT_INJEOSMP == adc_interrupt)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOSMPIF_Pos);
    }
    else if (ADC_IT_INJEOS == adc_interrupt)
    {
        adc->ADSTA_EXT = (0x01U << ADC_ADSTA_EXT_JEOSIF_Pos);
    }
}

/**
  * @brief  Enables or disables the selected ADC automatic injected group
  *         conversion after regular one.
  * @param  adc: select the ADC peripheral.
  * @param  state: new state of the selected ADC auto injected conversion
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_AutoInjectedConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                          \
    (adc->ANY_CR |= (0x01U << ADC_ANY_CR_JAUTO_Pos)) : \
    (adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JAUTO_Pos));
}

/**
  * @brief  Configures the adc Injected external trigger for injected channels conversion.
  * @param  adc: select the ADC peripheral.
  * @param  trig_edge:
  *     @arg   ADC_InjectedExtTrig_Edge_Up
  * @param  shift: trigger after this vlaue
  *     @arg    ADC_InjectedExtTrig_Shift_0
  *     @arg    ADC_InjectedExtTrig_Shift_4
  *     @arg    ADC_InjectedExtTrig_Shift_16
  *     @arg    ADC_InjectedExtTrig_Shift_32
  *     @arg    ADC_InjectedExtTrig_Shift_64
  *     @arg    ADC_InjectedExtTrig_Shift_128
  *     @arg    ADC_InjectedExtTrig_Shift_256
  *     @arg    ADC_InjectedExtTrig_Shift_512
  * @retval None
  */
void ADC_InjectedExternalTrigSourceConfig(ADC_TypeDef *adc, uint32_t trig_edge, uint32_t shift)
{
    MODIFY_REG(adc->ANY_CR, ADC_ANY_CR_JTRGSHIFT_Msk, shift);
    MODIFY_REG(adc->ANY_CR, ADC_ANY_CR_JTRG_EDGE_Msk, trig_edge);
}

/**
  * @brief  Enables or disables the adc injected channels conversion through
  *         external trigger
  * @param  adc:  select the ADC peripheral.
  * @param  state: new state of the selected ADC external trigger start of
  *         injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_InjectedExternalTrigConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                           \
    (adc->ANY_CR |= (0x01U << ADC_ANY_CR_JTRGEN_Pos)) : \
    (adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JTRGEN_Pos));
}

/**
  * @brief  Enables or disables the selected ADC start of the injected
  *         channels conversion.
  * @param  adc: select the ADC peripheral.
  * @param  state: new state of the selected ADC software start injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_InjectedConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                         \
    (adc->ANY_CR |= (0x01U << ADC_ANY_CR_JCEN_Pos)) : \
    (adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JCEN_Pos));
}

/**
  * @brief  Enables or disables the selected ADC start of the injected
  *         channels conversion.
  * @param  adc:  select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                          \
    (adc->ANY_CR |= (0x01U << ADC_ANY_CR_JADST_Pos)) : \
    (adc->ANY_CR &= ~(0x01U << ADC_ANY_CR_JADST_Pos));
}

/**
  * @brief  Injection channel length configuration.
  * @param  adc:  select the ADC peripheral.
  * @param  num: Injection channel number.
  * @retval None.
  */
void ADC_InjectedChannelNumCfg(ADC_TypeDef *adc, uint8_t num)
{
    MODIFY_REG(adc->JSQR, ADC_JSQR_JNUM_Msk, (num << ADC_JSQR_JNUM_Pos));
}

/**
  * @brief  configure injected channel from channel 0 to channel 15 as an input channel .
  * @param  adc:  select the ADC peripheral.
  * @param  injected_channel: ADC_InjectedChannel_x , where x may 0..3
  * @param  channel: ADC_Channel_x , where x may 0..18
  * @retval None.
  */
void ADC_InjectedChannelSelect(ADC_TypeDef *adc, uint8_t injected_channel, uint8_t channel)
{
    MODIFY_REG(adc->JSQR, (0x1F << (injected_channel * 5)), (channel << (injected_channel * 5)));
}

/**
  * @brief  Injection channel  converted value.
  * @param  adc  :   select the ADC peripheral.
  * @retval value.
  */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef *adc)
{
    return ((uint16_t)adc->JADDATA);
}

/**
  * @brief  Injection channel  converted value.
  * @param  adc  :   select the ADC peripheral.
  * @param  injected_channel :ADC_InjectedChannel_x , where x may 0..3
  * @retval value.
  */
uint16_t ADC_GetInjectedChannelConvertedValue(ADC_TypeDef *adc, uint8_t injected_channel)
{
    uint32_t value = 0;

    switch (injected_channel)
    {
        case ADC_InjectedChannel_0:
            value  = adc->JDR0;
            break;

        case ADC_InjectedChannel_1:
            value  = adc->JDR1;
            break;

        case ADC_InjectedChannel_2:
            value  = adc->JDR2;
            break;

        case ADC_InjectedChannel_3:
            value  = adc->JDR3;
            break;

        default:
            break;
    }

    return ((uint16_t)value);
}

/**
  * @brief  Injection channel compensation configuration.
  * @param  adc         : select the ADC peripheral.
  * @param  injected_channel :ADC_InjectedChannel_x , where x may 0..3
  * @param  value       : compensation value.
  * @retval None.
  */
void ADC_InjectedChannelOffsetCfg(ADC_TypeDef *adc, uint8_t injected_channel, uint16_t value)
{
    switch (injected_channel)
    {
        case ADC_InjectedChannel_0:
            adc->JOFR0 = value;
            break;

        case ADC_InjectedChannel_1:
            adc->JOFR1 = value;
            break;

        case ADC_InjectedChannel_2:
            adc->JOFR2 = value;
            break;

        case ADC_InjectedChannel_3:
            adc->JOFR3 = value;
            break;

        default:
            break;
    }
}

/**
  * @brief  return injection channel  offset value.
  * @param  adc  :   select the ADC peripheral.
  * @param  injected_channel :   Injection channel.
  * @retval value.
  */
uint16_t ADC_GetInjectedChannelOffset(ADC_TypeDef *adc, uint8_t injected_channel)
{
    uint32_t value;

    switch (injected_channel)
    {
        case ADC_InjectedChannel_0:
            value = adc->JOFR0;
            break;

        case ADC_InjectedChannel_1:
            value = adc->JOFR1;
            break;

        case ADC_InjectedChannel_2:
            value = adc->JOFR2;
            break;

        case ADC_InjectedChannel_3:
            value = adc->JOFR3;
            break;

        default:
            break;
    }

    return ((uint16_t)value);
}

/**
  * @brief  configure ADC oversampling.
  * @param  adc:  select the ADC peripheral.
  * @param  ovsr: oversampling ratio
  *     @arg    ADC_Oversampling_Ratio_2
  *     @arg    ADC_Oversampling_Ratio_4
  *     @arg    ADC_Oversampling_Ratio_8
  *     @arg    ADC_Oversampling_Ratio_16
  *     @arg    ADC_Oversampling_Ratio_32
  *     @arg    ADC_Oversampling_Ratio_64
  *     @arg    ADC_Oversampling_Ratio_128
  *     @arg    ADC_Oversampling_Ratio_256
  * @param  ovss: oversampling shift bits
  *     @arg    ADC_Oversampling_Shift_Bit_0
  *     @arg    ADC_Oversampling_Shift_Bit_1
  *     @arg    ADC_Oversampling_Shift_Bit_2
  *     @arg    ADC_Oversampling_Shift_Bit_3
  *     @arg    ADC_Oversampling_Shift_Bit_4
  *     @arg    ADC_Oversampling_Shift_Bit_5
  *     @arg    ADC_Oversampling_Shift_Bit_6
  *     @arg    ADC_Oversampling_Shift_Bit_7
  *     @arg    ADC_Oversampling_Shift_Bit_8
  * @param  trovs: trigger oversampling select
  *     @arg   ADC_TriggerOversampling_EachConversion
  *     @arg   ADC_TriggerOversampling_AllConversion
  * @retval None.
  */
void ADC_OversamplingConfig(ADC_TypeDef *adc, uint32_t ovsr, uint32_t ovss, uint32_t trovs)
{
    MODIFY_REG(adc->ADCFG2, ADC_ADCFG2_OVSR_Msk, ovsr);
    MODIFY_REG(adc->ADCFG2, ADC_ADCFG2_OVSS_Msk, ovss);
    MODIFY_REG(adc->ADCFG2, ADC_ADCFG2_TROVS_Msk, trovs);
}

/**
  * @brief  Enables or disables the Injected Oversampler.
  * @param  adc:  select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_InjectedOversamplerCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                          \
    (adc->ADCFG2 |= (0x01U << ADC_ADCFG2_JOVSE_Pos)) : \
    (adc->ADCFG2 &= ~(0x01U << ADC_ADCFG2_JOVSE_Pos));
}

/**
  * @brief  Enables or disables the Regular Oversampler.
  * @param  adc:  select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_RegularOversamplerCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                          \
    (adc->ADCFG2 |= (0x01U << ADC_ADCFG2_ROVSE_Pos)) : \
    (adc->ADCFG2 &= ~(0x01U << ADC_ADCFG2_ROVSE_Pos));
}

/**
  * @brief  configure differential conversion or pseudo differential conversion
  * @param  adc:  select the ADC peripheral.
  * @param  dcsel: differential conversion or pseudo differential conversion
  *     @arg    ADC_Differential_Conversion_0_1
  *     @arg    ADC_Differential_Conversion_2_3
  *     @arg    ADC_Differential_Conversion_4_5
  *     @arg    ADC_Differential_Conversion_6_7
  *     @arg    ADC_Pseudo_Differential_Conversion_0_1
  *     @arg    ADC_Pseudo_Differential_Conversion_2_3
  *     @arg    ADC_Pseudo_Differential_Conversion_4_5
  *     @arg    ADC_Pseudo_Differential_Conversion_6_7
  * @retval None.
  */
void ADC_DifferentialConversionConfig(ADC_TypeDef *adc, uint32_t dcsel)
{
    MODIFY_REG(adc->ADCFG2, ADC_ADCFG2_PSDC_Msk | ADC_ADCFG2_DC_Msk, dcsel);
}

/**
  * @brief  configure ADC reference voltage
  * @param  adc:  select the ADC peripheral.
  * @param  vref: Select ADC reference voltage
  *     @arg   ADC_Select_Vref_Internal
  *     @arg   ADC_Select_Vref_External
  * @retval None.
  */
void ADC_ReferenceVoltageConfig(ADC_TypeDef *adc, uint32_t vref)
{
    MODIFY_REG(adc->ADCFG2, ADC_ADCFG2_ADCSREF_Msk, vref);
}

/**
  * @brief  Enables or disables the ADC low power consumption.
  * @param  adc:  select the ADC peripheral.
  * @param  NewState: new state of the selected ADC software start injected conversion.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_LowPowerConsumptionCmd(ADC_TypeDef *adc, FunctionalState state)
{
    (state) ?                                         \
    (adc->ADCFG2 |= (0x01 << ADC_ADCFG2_ADCCR_Pos)) : \
    (adc->ADCFG2 &= ~(0x01 << ADC_ADCFG2_ADCCR_Pos));
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

