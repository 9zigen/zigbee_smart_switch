/**
 * Copyright (c) 2018 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup zigbee_examples_light_bulb main.c
 * @{
 * @ingroup zigbee_examples
 * @brief Dimmable light sample (HA profile)
 */

#include <nrf_delay.h>
#include <nrf_802154.h>
#include "sdk_config.h"
#include "zboss_api.h"
#include "zboss_api_addons.h"
#include "zb_mem_config_med.h"
#include "zb_ha_on_off_switch.h"
#include "zb_error_handler.h"
#include "zb_nrf52_internal.h"
#include "zigbee_helpers.h"

#include "bsp.h"
#include "include/pwm.h"
#include "include/relay.h"
#include "boards.h"
#include "include/button.h"
#include "include/storage.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MAX_CHILDREN                      10                                    /**< The maximum amount of connected devices. Setting this value to 0 disables association to this device.  */
#define IEEE_CHANNEL_MASK                 (1l << ZIGBEE_CHANNEL)                /**< Scan only one, predefined channel to find the coordinator. */
#define HA_ON_OFF_ENDPOINT                1                                     /**< Device endpoint, used to receive light controlling commands. */
#define ERASE_PERSISTENT_CONFIG           ZB_FALSE                              /**< Do not erase NVRAM to save the network parameters after device reboot or power-off. */

/* Basic cluster attributes initial values. */
#define BULB_INIT_BASIC_APP_VERSION       01                                    /**< Version of the application software (1 byte). */
#define BULB_INIT_BASIC_STACK_VERSION     10                                    /**< Version of the implementation of the Zigbee stack (1 byte). */
#define BULB_INIT_BASIC_HW_VERSION        11                                    /**< Version of the hardware of the device (1 byte). */
#define BULB_INIT_BASIC_MANUF_NAME        "Custom devices (DiY)"                /**< Manufacturer name (32 bytes). */
#define BULB_INIT_BASIC_MODEL_ID          "ptvo.switch"                         /**< Model number assigned by manufacturer (32-bytes long string). */
#define BULB_INIT_BASIC_DATE_CODE         "20210101"                            /**< First 8 bytes specify the date of manufacturer of the device in ISO 8601 format (YYYYMMDD). The rest (8 bytes) are manufacturer specific. */
#if defined ZB_ED_ROLE
#define BULB_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_BATTERY     /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#else
#define BULB_INIT_BASIC_POWER_SOURCE      ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE   /**< Type of power sources available for the device. For possible values see section 3.2.2.2.8 of ZCL specification. */
#endif
#define BULB_INIT_BASIC_LOCATION_DESC     "Office desk"                         /**< Describes the physical location of the device (16 bytes). May be modified during commisioning process. */
#define BULB_INIT_BASIC_PH_ENV            ZB_ZCL_BASIC_ENV_UNSPECIFIED          /**< Describes the type of physical environment. For possible values see section 3.2.2.2.10 of ZCL specification. */

//#if !defined ZB_ROUTER_ROLE
//#error Define ZB_ROUTER_ROLE to compile light bulb (Router) source code.
//#endif

#define ZCL_DECLARE_ON_OFF_OUTPUT_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
      ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                    \
      ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name =            \
      {                                                                                       \
        ep_id,                                                                                \
        ZB_AF_HA_PROFILE_ID,                                                                  \
        ZB_HA_ON_OFF_OUTPUT_DEVICE_ID,                                                        \
        ZB_HA_DEVICE_VER_ON_OFF_OUTPUT,                                                       \
        0,                                                                                    \
        in_clust_num,                                                                         \
        out_clust_num,                                                                        \
        {                                                                                     \
          ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
          ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
          ZB_ZCL_CLUSTER_ID_SCENES,                                                           \
          ZB_ZCL_CLUSTER_ID_GROUPS,                                                           \
          ZB_ZCL_CLUSTER_ID_ON_OFF,                                                           \
        }                                                                                     \
      }

#define HA_DECLARE_ON_OFF_OUTPUT_EP(ep_name, ep_id, cluster_list)   \
      ZCL_DECLARE_ON_OFF_OUTPUT_SIMPLE_DESC(                        \
          ep_name,                                                     \
          ep_id,                                                       \
          ZB_HA_ON_OFF_OUTPUT_IN_CLUSTER_NUM,                          \
          ZB_HA_ON_OFF_OUTPUT_OUT_CLUSTER_NUM);                        \
      ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info## device_ctx_name,           \
                                         ZB_HA_ON_OFF_OUTPUT_REPORT_ATTR_COUNT);     \
      ZB_AF_DECLARE_ENDPOINT_DESC(ep_name,                                           \
                              ep_id,                                    \
            ZB_AF_HA_PROFILE_ID,                                                             \
            0,                                                                               \
            NULL,                                                                            \
            ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t),                          \
            cluster_list,                                              \
            (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name, \
            ZB_HA_ON_OFF_OUTPUT_REPORT_ATTR_COUNT, reporting_info## device_ctx_name, \
            0, NULL)

/* Main application customizable context. Stores all settings and static values. */
typedef struct
{
  uint8_t                          ep_id;                  /**< Endpoint ID. */
  zb_zcl_basic_attrs_ext_t         basic_attr;
  zb_zcl_identify_attrs_t          identify_attr;
  zb_zcl_scenes_attrs_t            scenes_attr;
  zb_zcl_groups_attrs_t            groups_attr;
  zb_zcl_on_off_attrs_t            on_off_attr;
} device_ctx_t;

device_ctx_t m_dev_ctx;

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attr_list, &m_dev_ctx.identify_attr.identify_time);

ZB_ZCL_DECLARE_GROUPS_ATTRIB_LIST(groups_attr_list, &m_dev_ctx.groups_attr.name_support);

ZB_ZCL_DECLARE_SCENES_ATTRIB_LIST(scenes_attr_list,
    &m_dev_ctx.scenes_attr.scene_count,
    &m_dev_ctx.scenes_attr.current_scene,
    &m_dev_ctx.scenes_attr.current_group,
    &m_dev_ctx.scenes_attr.scene_valid,
    &m_dev_ctx.scenes_attr.name_support);

ZB_ZCL_DECLARE_BASIC_ATTRIB_LIST_EXT(basic_attr_list,
    &m_dev_ctx.basic_attr.zcl_version,
    &m_dev_ctx.basic_attr.app_version,
    &m_dev_ctx.basic_attr.stack_version,
    &m_dev_ctx.basic_attr.hw_version,
    m_dev_ctx.basic_attr.mf_name,
    m_dev_ctx.basic_attr.model_id,
    m_dev_ctx.basic_attr.date_code,
    &m_dev_ctx.basic_attr.power_source,
    m_dev_ctx.basic_attr.location_id,
    &m_dev_ctx.basic_attr.ph_env,
    m_dev_ctx.basic_attr.sw_ver);

/* On/Off cluster attributes additions data */
//ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(on_off_attr_list,
//    &m_dev_ctx.on_off_attr.on_off,
//    &m_dev_ctx.on_off_attr.global_scene_ctrl,
//    &m_dev_ctx.on_off_attr.on_time,
//    &m_dev_ctx.on_off_attr.off_wait_time);

ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(on_off_attr_list,&m_dev_ctx.on_off_attr.on_off);

ZB_HA_DECLARE_ON_OFF_OUTPUT_CLUSTER_LIST(on_off_clusters,
    on_off_attr_list,
    basic_attr_list,
    identify_attr_list,
    groups_attr_list,
    scenes_attr_list);

HA_DECLARE_ON_OFF_OUTPUT_EP(on_off_ep,
                               HA_ON_OFF_ENDPOINT,
                               on_off_clusters);

ZB_HA_DECLARE_ON_OFF_OUTPUT_CTX(on_off_ctx, on_off_ep);

static zb_bool_t is_joined;

/**@brief Function for initializing the application timer.
 */
static void timer_init(void)
{
  uint32_t error_code = app_timer_init();
  APP_ERROR_CHECK(error_code);
}

/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/* ON/OFF relay or any load */
static void on_off_set_value(device_ctx_t * device_ctx, zb_bool_t on)
{
  NRF_LOG_INFO("Set ON/OFF value: %i for endpoint %u", on, device_ctx->ep_id);

  ZB_ZCL_SET_ATTRIBUTE(device_ctx->ep_id,
                       ZB_ZCL_CLUSTER_ID_ON_OFF,
                       ZB_ZCL_CLUSTER_SERVER_ROLE,
                       ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                       (zb_uint8_t *)&on,
                       ZB_FALSE);

  if (on)
  {
    relay_on(1);
#if defined ZB_ED_ROLE
    /* ToDO: short blink */
#else
    indicate_mode(INDICATE_ON);
#endif
  }
  else
  {
    if (relay_get_mode() == ON_DELAY_OFF)
    {
      relay_on(0);
    } else {
      relay_off();
    }
    relay_set_status(0);
    indicate_mode(INDICATE_OFF);
  }
}

static void reboot_application(zb_uint8_t param)
{
  UNUSED_VARIABLE(param);
  NRF_LOG_FINAL_FLUSH();

#if NRF_MODULE_ENABLED(NRF_LOG_BACKEND_RTT)
  // To allow the buffer to be flushed by the host.
  nrf_delay_ms(100);
#endif

  NVIC_SystemReset();
}

/**@brief Callback for button events.
 *
 * @param[in]   evt      Incoming event from the BSP subsystem.
 */
static void buttons_handler(button_state_t state)
{
  static uint8_t in_config = 0;

  switch (state)
  {
    case RELEASED:
      if (in_config)
      {
        relay_mode_t mode = !relay_get_mode();
        NRF_LOG_INFO("BTN SHORT PRESS, toggle mode %u -> %u", !mode, mode);
        relay_set_mode(mode);
        if (mode == 0) { indicate_mode(INDICATE_MODE_0); }
        if (mode == 1) { indicate_mode(INDICATE_MODE_1); }

      } else {
        uint8_t status = relay_get_status();
        NRF_LOG_INFO("BTN SHORT PRESS, toggle relay %d -> %d", status, !status);
        /* update endpoint */
        on_off_set_value(&m_dev_ctx, (zb_bool_t) !status);

      }
      break;
    case LONG_PRESSED:
      NRF_LOG_INFO("BTN LONG PRESS, in_config? %u", in_config);
      if (!in_config)
      {
        in_config = 1;
        NRF_LOG_INFO("start config mode %u", in_config);
        indicate_config_mode();
      } else {
        in_config = 0;
        NRF_LOG_INFO("finish config mode %u", in_config);
        indicate_config_mode_finish();
        set_settings(relay_get_mode());
      }
      break;

    case LONG_LONG_PRESSED:
      NRF_LOG_INFO("BTN LONG LONG PRESS, factory reset is_joined? %d", is_joined);
      if (is_joined)
      {
        /* leave network and soft reset */
        zb_bdb_reset_via_local_action(0);
      } else {
        zb_bool_t comm_status = bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        ZB_COMM_STATUS_CHECK(comm_status);
      }
      break;

    default:
      break;
  }
//  zb_ret_t zb_err_code;
//
//  switch(evt)
//  {
//    case IDENTIFY_MODE_BSP_EVT:
//      /* Check if endpoint is in identifying mode, if not put desired endpoint in identifying mode. */
//      if (m_dev_ctx.identify_attr.identify_time == ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE)
//      {
//        NRF_LOG_INFO("Bulb put in identifying mode");
//        zb_err_code = zb_bdb_finding_binding_target(HA_ON_OFF_ENDPOINT);
//        ZB_ERROR_CHECK(zb_err_code);
//      }
//      else
//      {
//        NRF_LOG_INFO("Cancel F&B target procedure");
//        zb_bdb_finding_binding_target_cancel();
//      }
//      break;
//
//    default:
//      NRF_LOG_INFO("Unhandled BSP Event received: %d", evt);
//      break;
//  }
}


/**@brief Function for initializing LEDs and a single PWM channel.
 */
static void leds_buttons_init(void)
{
  button_init(buttons_handler);
//  set_button_action_handler(buttons_handler);

  leds_init();
}

/**@brief Function for initializing clusters attributes.
 *
 * @param[IN]   p_light_ctx   Pointer to structure with device_ctx.
 * @param[IN]   ep_id          Endpoint ID.
 */
static void clusters_attr_init(device_ctx_t * p_dev_ctx)
{
  /* Basic cluster attributes data */
  p_dev_ctx->basic_attr.zcl_version   = ZB_ZCL_VERSION;
  p_dev_ctx->basic_attr.app_version   = BULB_INIT_BASIC_APP_VERSION;
  p_dev_ctx->basic_attr.stack_version = BULB_INIT_BASIC_STACK_VERSION;
  p_dev_ctx->basic_attr.hw_version    = BULB_INIT_BASIC_HW_VERSION;

  /* Use ZB_ZCL_SET_STRING_VAL to set strings, because the first byte should
   * contain string length without trailing zero.
   *
   * For example "test" string wil be encoded as:
   *   [(0x4), 't', 'e', 's', 't']
   */
  ZB_ZCL_SET_STRING_VAL(p_dev_ctx->basic_attr.mf_name,
                        BULB_INIT_BASIC_MANUF_NAME,
                        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MANUF_NAME));

  ZB_ZCL_SET_STRING_VAL(p_dev_ctx->basic_attr.model_id,
                        BULB_INIT_BASIC_MODEL_ID,
                        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_MODEL_ID));

  ZB_ZCL_SET_STRING_VAL(p_dev_ctx->basic_attr.date_code,
                        BULB_INIT_BASIC_DATE_CODE,
                        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_DATE_CODE));

  p_dev_ctx->basic_attr.power_source = BULB_INIT_BASIC_POWER_SOURCE;

  ZB_ZCL_SET_STRING_VAL(p_dev_ctx->basic_attr.location_id,
                        BULB_INIT_BASIC_LOCATION_DESC,
                        ZB_ZCL_STRING_CONST_SIZE(BULB_INIT_BASIC_LOCATION_DESC));


  p_dev_ctx->basic_attr.ph_env = BULB_INIT_BASIC_PH_ENV;

  /* Identify cluster attributes data */
  p_dev_ctx->identify_attr.identify_time       = ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE;

  /* On/Off cluster attributes data */
  p_dev_ctx->on_off_attr.on_off                = (zb_bool_t)ZB_ZCL_ON_OFF_IS_ON;
//  p_dev_ctx->on_off_attr.global_scene_ctrl     = ZB_TRUE;
//  p_dev_ctx->on_off_attr.on_time               = 0;
//  p_dev_ctx->on_off_attr.off_wait_time         = 0;

  ZB_ZCL_SET_ATTRIBUTE(p_dev_ctx->ep_id,
                       ZB_ZCL_CLUSTER_ID_ON_OFF,
                       ZB_ZCL_CLUSTER_SERVER_ROLE,
                       ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,
                       (zb_uint8_t *)&p_dev_ctx->on_off_attr.on_off,
                       ZB_FALSE);
}

/**@brief Function for initializing all clusters attributes.
 */
static void on_off_clusters_attr_init(device_ctx_t * p_dev_ctx,
                                      uint8_t ep_id,
                                      zb_callback_t identify_cb)
{
  /* Initialize application context structure. */
  UNUSED_RETURN_VALUE(ZB_MEMSET(p_dev_ctx, 0, sizeof(device_ctx_t)));

  p_dev_ctx->ep_id = ep_id;

  clusters_attr_init(p_dev_ctx);

  /* Register handlers to identify notifications */
//  ZB_AF_SET_IDENTIFY_NOTIFICATION_HANDLER(p_light_ctx->ep_id, identify_cb);
}

/**@brief Function which tries to sleep down the MCU
 *
 * Function which sleeps the MCU on the non-sleepy End Devices to optimize the power saving.
 * The weak definition inside the OSIF layer provides some minimal working template
 */
zb_void_t zb_osif_go_idle(zb_void_t)
{
  //TODO: implement your own logic if needed
  zb_osif_wait_for_event();
}

/**@brief Callback function for handling ZCL commands.
 *
 * @param[in]   bufid   Reference to Zigbee stack buffer used to pass received data.
 */
static zb_void_t zcl_device_cb(zb_bufid_t bufid)
{
  zb_uint8_t                       cluster_id;
  zb_uint8_t                       attr_id;
  zb_zcl_device_callback_param_t * p_device_cb_param = ZB_BUF_GET_PARAM(bufid, zb_zcl_device_callback_param_t);

  NRF_LOG_INFO("Received ZCL callback %hd on endpoint %hu",
      p_device_cb_param->device_cb_id, p_device_cb_param->endpoint);

  /* Set default response value. */
  p_device_cb_param->status = RET_OK;

  switch (p_device_cb_param->device_cb_id)
  {
    case ZB_ZCL_SET_ATTR_VALUE_CB_ID:
      cluster_id = p_device_cb_param->cb_param.set_attr_value_param.cluster_id;
      attr_id    = p_device_cb_param->cb_param.set_attr_value_param.attr_id;

      if (cluster_id == ZB_ZCL_CLUSTER_ID_ON_OFF)
      {
        uint8_t value = p_device_cb_param->cb_param.set_attr_value_param.values.data8;

        NRF_LOG_INFO("on/off attribute setting to %hd", value);
        if (attr_id == ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID)
        {
          on_off_set_value(&m_dev_ctx, (zb_bool_t) value);
        }
      }
      else
      {
        /* Other clusters can be processed here */
        NRF_LOG_INFO("Unhandled cluster attribute id: %d", cluster_id);
      }
      break;

    default:
      p_device_cb_param->status = RET_ERROR;
      break;
  }

  NRF_LOG_INFO("zcl_device_cb status: %hd", p_device_cb_param->status);
}

/**@brief Zigbee stack event handler.
 *
 * @param[in]   bufid   Reference to the Zigbee stack buffer used to pass signal.
 */
void zboss_signal_handler(zb_bufid_t bufid)
{
  zb_zdo_app_signal_hdr_t  * p_sg_p = NULL;
  zb_zdo_app_signal_type_t   sig    = zb_get_app_signal(bufid, &p_sg_p);
  zb_ret_t                   status = ZB_GET_APP_SIGNAL_STATUS(bufid);

  switch (sig)
  {
    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ZB_BDB_SIGNAL_DEVICE_REBOOT:
      /* fall-through */
    case ZB_BDB_SIGNAL_STEERING:
      if (status == RET_OK)
      {
        is_joined = ZB_TRUE;
        indicate_mode(INDICATE_JOINED);
#if defined ZB_ED_ROLE
        zb_zdo_pim_set_long_poll_interval(5000);
#endif
      }
      else
      {
        is_joined = ZB_FALSE;
        indicate_mode(INDICATE_SEARCHING);
      }
      break;

    case ZB_ZDO_SIGNAL_LEAVE:
      if (status == RET_OK)
      {
        is_joined = ZB_FALSE;
        zb_zdo_signal_leave_params_t * p_leave_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_leave_params_t);
        NRF_LOG_INFO("Network left (leave type: %d)", p_leave_params->leave_type);

        reboot_application(0);
      }
      else
      {
        NRF_LOG_ERROR("Unable to leave network (status: %d)", status);
      }
      #if defined ZB_ROUTER_ROLE
      zb_enable_auto_pan_id_conflict_resolution(ZB_FALSE);
      #endif
      break;
#if defined ZB_ED_ROLE
    case ZB_COMMON_SIGNAL_CAN_SLEEP:
      /* Zigbee stack can enter sleep state.
       * If the application wants to proceed, it should call zb_sleep_now() function.
       *
       * Note: if the application shares some resources between Zigbee stack and other tasks/contexts,
       *       device disabling should be overwritten by implementing one of the weak functions inside zb_nrf52840_common.c.
       */

      zb_sleep_now();
      break;
#endif

    default:
      break;
  }

  /* Update network status LED */
//  zigbee_led_status_update(bufid, ZIGBEE_NETWORK_STATE_LED);

  /* No application-specific behavior is required. Call default signal handler. */
  ZB_ERROR_CHECK(zigbee_default_signal_handler(bufid));
  zb_buf_free(bufid);
}

//zb_uint8_t zcl_custom_cmd_handler(zb_uint8_t param) {
//  zb_buf_t *zcl_cmd_buf = (zb_buf_t *) ZB_BUF_FROM_REF(param);
//  zb_zcl_parsed_hdr_t *cmd_info = ZB_GET_BUF_PARAM(
//      zcl_cmd_buf,
//      zb_zcl_parsed_hdr_t);
//  /* Store some values - cmd_info will be overwritten */
//  zb_uint8_t processed = ZB_FALSE;
//  zb_uint16_t dst_addr = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_addr;
//  zb_uint8_t endpoint = ZB_ZCL_PARSED_HDR_SHORT_DATA(cmd_info).src_endpoint;
//  zb_uint8_t seq_num = cmd_info->seq_number;
//
//
//  ZB_ZCL_SEND_DEFAULT_RESP(
//      zcl_cmd_buf,
//      dst_addr,
//      ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
//      endpoint,
//      HA_ON_OFF_ENDPOINT,
//      ZB_AF_HA_PROFILE_ID,
//      ZB_ZCL_CLUSTER_ID_ON_OFF,
//      seq_num,
//      ZB_ZCL_CMD_ON_OFF_TOGGLE_ID,
//      ZB_ZCL_STATUS_UNSUP_CLUST_CMD);
//}
/**@brief Function for application main entry.
 */

int main(void)
{
  zb_ret_t       zb_err_code;
  zb_ieee_addr_t ieee_addr;

  /* 3.3v REGO out */
  if (NRF_UICR->REGOUT0 != UICR_REGOUT0_VOUT_3V3)
  {
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
    NRF_UICR->REGOUT0 = UICR_REGOUT0_VOUT_3V3;

    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
  }

  /* DCDC en */
  NRF_POWER->DCDCEN0 = 1;
  NRF_POWER->DCDCEN = 1;

  /* Initialize timer, logging system, Storage and GPIOs. */
  timer_init();
  log_init();
  leds_buttons_init();

  /* Initialize Relay, load settings */
  relay_init();
  storage_init();

  indicate_mode(INDICATE_OFF);

  NRF_LOG_INFO("Zigbee started");
  NRF_LOG_FLUSH();

  /* Set Zigbee stack logging level and traffic dump subsystem. */
  ZB_SET_TRACE_LEVEL(ZIGBEE_TRACE_LEVEL);
  ZB_SET_TRACE_MASK(ZIGBEE_TRACE_MASK);
  ZB_SET_TRAF_DUMP_OFF();

  /* Initialize Zigbee stack. */
  ZB_INIT("smart_relay");

  /* Set device address to the value read from FICR registers. */
  zb_osif_get_ieee_eui64(ieee_addr);
  zb_set_long_address(ieee_addr);

  /* ED Role */
#if defined ZB_ED_ROLE
  zb_set_network_ed_role(ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
  zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
  zb_set_ed_timeout(ED_AGING_TIMEOUT_64MIN);
  zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
  zb_set_rx_on_when_idle(ZB_FALSE);
#else
  /* Router Role Set static long IEEE address. */
  zb_set_network_router_role(ZB_TRANSCEIVER_ALL_CHANNELS_MASK);
  zb_set_max_children(MAX_CHILDREN);
  zigbee_erase_persistent_storage(ERASE_PERSISTENT_CONFIG);
  zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
#endif
  /* Register callback for handling ZCL commands. */
  ZB_ZCL_REGISTER_DEVICE_CB(zcl_device_cb);

  /* Register dimmer switch device context (endpoints). */
  ZB_AF_REGISTER_DEVICE_CTX(&on_off_ctx);

//  ZB_AF_SET_ENDPOINT_HANDLER(HA_ON_OFF_ENDPOINT, handler)

  on_off_clusters_attr_init(&m_dev_ctx, HA_ON_OFF_ENDPOINT, NULL);

  /*set tx power +8dbM */
  nrf_802154_tx_power_set(4);

  /** Start Zigbee Stack. */
  zb_err_code = zboss_start_no_autostart();
  ZB_ERROR_CHECK(zb_err_code);

  NRF_LOG_INFO("Start Zigbee Stack Finish");
  NRF_LOG_FLUSH();

  while(1)
  {
    zboss_main_loop_iteration();
    UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
  }
}