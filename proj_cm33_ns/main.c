/*****************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for non-secure
*                    application in the CM33 CPU
*
* Related Document : See README.md
*
*******************************************************************************
# \copyright
# (c) 2024-2025, Infineon Technologies AG, or an affiliate of Infineon Technologies AG.
# SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/

#include <stdio.h>
#include "cybsp.h"
#include "cy_pdl.h"
#include "tfm_ns_interface.h"
#include "os_wrapper/common.h"
#include "psa/internal_trusted_storage.h"
#include "ifx_platform_api.h"


/*******************************************************************************
* Macros
*******************************************************************************/

/* Internal Trusted Storage UID */
#define ITS_UID                     (1U)

/* Buffer size for Internal Trusted Storage */
#define ITS_BUFF_SIZE               (20U)

/* The timeout value in microseconds used to wait for CM55 core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC    (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR          (CYMEM_CM33_0_m55_nvm_START + \
                                        CYBSP_MCUBOOT_HEADER_SIZE)


/*******************************************************************************
* Global Variables
*******************************************************************************/


/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function of the CM33 non-secure application. 
*
* It initializes the TF-M NS interface to communicate with TF-M FW. The app
* calls PSA APIs to use the Internal Trusted Storage secure services
* offered by TF-M.

* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t rslt;
    char set_data[] = "Hello World";
    char get_data[ITS_BUFF_SIZE] = {0};
    size_t get_len = 0;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    unsigned char out_buf[256];
    int buf_size;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize TF-M interface */
    rslt = tfm_ns_interface_init();
    if(rslt != OS_WRAPPER_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    buf_size = sprintf((char*)out_buf, "\x1b[2J\x1b[;H"
                "******* "
                "PSOC Edge MCU: Basic Trusted Firmware-M (TF-M) based Application "
                "******* \r\n\n");
    ifx_platform_log_msg(out_buf, buf_size);


    buf_size = sprintf((char*)out_buf, "*** TF-M Internal Trusted Storage (ITS) service ***\r\n\n");
    ifx_platform_log_msg(out_buf, buf_size);

    buf_size = sprintf((char*)out_buf, "ITS Storage data: %s\r\n", set_data);
    ifx_platform_log_msg(out_buf, buf_size);

    buf_size = sprintf((char*)out_buf, "Storing data in ITS...\r\n\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Start of Internal Trusted Storage code.
     * Internal Trusted Storage can store upto 10 assets. The maximum size of asset
     * can be upto 512 bytes.
     */
    status = psa_its_set(ITS_UID, sizeof(set_data), set_data, PSA_STORAGE_FLAG_NONE);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Retrieving data from ITS...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    status = psa_its_get(ITS_UID, 0, sizeof(set_data), get_data, &get_len);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Retrieved data: %s\r\n\n", get_data);
    ifx_platform_log_msg(out_buf, buf_size);

    /* End of Internal Trusted Storage code */

    /* Enable CM55. */
    /* CY_CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);

    for (;;)
    {

        /* Receive and forward the IPC requests from M55 to TF-M. 
         * M55 can request security aware PDL and TF-M for secure services,
         * and these requests are sent from M55 to M33 NS using Secure Request
         * Framework (SRF) over IPC.
         */
        result = mtb_srf_ipc_receive_request(&cybsp_mtb_srf_relay_context, MTB_IPC_NEVER_TIMEOUT);
        if(result != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);
        }
        result =  mtb_srf_ipc_process_pending_request(&cybsp_mtb_srf_relay_context);
        if(result != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);
        }
    }
}
/* [] END OF FILE */
