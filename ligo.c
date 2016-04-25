/* -*- c-basic-offset: 2 -*- */
/*
 * Copyright (c) 2015 Renaud AUBIN
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "ligo.h"

#define GET_PROTOCOL_VENDOR_COMMAND      51
#define SEND_STRING_VENDOR_COMMAND       52
#define START_ACCESSORY_VENDOR_COMMAND   53

/** built-in vendor list */
int builtInVendorIds[] = {
  VENDOR_ID_GOOGLE,
  VENDOR_ID_INTEL,
  VENDOR_ID_HTC,
  VENDOR_ID_SAMSUNG,
  VENDOR_ID_MOTOROLA,
  VENDOR_ID_LGE,
  VENDOR_ID_HUAWEI,
  VENDOR_ID_ACER,
  VENDOR_ID_SONY_ERICSSON,
  VENDOR_ID_FOXCONN,
  VENDOR_ID_DELL,
  VENDOR_ID_NVIDIA,
  VENDOR_ID_GARMIN_ASUS,
  VENDOR_ID_SHARP,
  VENDOR_ID_ZTE,
  VENDOR_ID_KYOCERA,
  VENDOR_ID_PANTECH,
  VENDOR_ID_QUALCOMM,
  VENDOR_ID_OTGV,
  VENDOR_ID_NEC,
  VENDOR_ID_PMC,
  VENDOR_ID_TOSHIBA,
  VENDOR_ID_SK_TELESYS,
  VENDOR_ID_KT_TECH,
  VENDOR_ID_ASUS,
  VENDOR_ID_PHILIPS,
  VENDOR_ID_TI,
  VENDOR_ID_FUNAI,
  VENDOR_ID_GIGABYTE,
  VENDOR_ID_IRIVER,
  VENDOR_ID_COMPAL,
  VENDOR_ID_T_AND_A,
  VENDOR_ID_LENOVOMOBILE,
  VENDOR_ID_LENOVO,
  VENDOR_ID_VIZIO,
  VENDOR_ID_K_TOUCH,
  VENDOR_ID_PEGATRON,
  VENDOR_ID_ARCHOS,
  VENDOR_ID_POSITIVO,
  VENDOR_ID_FUJITSU,
  VENDOR_ID_LUMIGON,
  VENDOR_ID_QUANTA,
  VENDOR_ID_INQ_MOBILE,
  VENDOR_ID_SONY,
  VENDOR_ID_LAB126,
  VENDOR_ID_YULONG_COOLPAD,
  VENDOR_ID_KOBO,
  VENDOR_ID_TELEEPOCH,
  VENDOR_ID_ANYDATA,
  VENDOR_ID_HARRIS,
  VENDOR_ID_OPPO,
  VENDOR_ID_XIAOMI,
  VENDOR_ID_BYD,
  VENDOR_ID_OUYA,
  VENDOR_ID_HAIER,
  VENDOR_ID_HISENSE,
  VENDOR_ID_MTK,
  VENDOR_ID_NOOK,
  VENDOR_ID_QISDA,
  VENDOR_ID_ECS,
};

#define BUILT_IN_VENDOR_COUNT    (sizeof(builtInVendorIds)/sizeof(builtInVendorIds[0]))


int ligo_set_configuration(libusb_device *dev);


static uint8_t endpoint_in = 0;
static uint8_t endpoint_out = 0;

static struct libusb_device_handle *iohandle = NULL;

struct libusb_device_handle * ligo_get_io_handle() {
  return iohandle;
}

int ligo_open_and_claim(libusb_device *dev, int interface_number) {
  int rc = 0;
  rc = libusb_open(dev, &iohandle);
  if (rc == LIBUSB_SUCCESS) {
    rc = libusb_claim_interface(iohandle, interface_number);
  }
  return rc;
}



uint8_t ligo_get_ep_in() {
  return endpoint_in;
}

uint8_t ligo_get_ep_out() {
  return endpoint_out;
}


/*******************/
/* ligo primitives */
/*******************/
void ligo_init_endpoints(libusb_device *dev) {
  struct libusb_config_descriptor *conf_desc;
  const struct libusb_endpoint_descriptor *endpoint;

  libusb_get_active_config_descriptor(dev, &conf_desc);
  int k;
  for (k=0; k<conf_desc->interface[0].altsetting[0].bNumEndpoints; k++) {
    endpoint = &conf_desc->interface[0].altsetting[0].endpoint[k];
    // Use the first interrupt or bulk IN/OUT endpoints as default for testing
    if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
      if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
        if (!endpoint_in)
          endpoint_in = endpoint->bEndpointAddress;
      } else {
        if (!endpoint_out)
          endpoint_out = endpoint->bEndpointAddress;
      }
    }
  }
  libusb_free_config_descriptor(conf_desc);
}


int ligo_set_configuration(libusb_device *dev) {
  struct libusb_device_handle *h;

  int r = libusb_open(dev, &h);
  if (r == 0) {
    r = libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_ENDPOINT_OUT,
                                LIBUSB_REQUEST_SET_CONFIGURATION, 1, 0, NULL, 0, 0);
    libusb_close(h);
  }
  return r;
}


int ligo_send_start_command(libusb_device_handle *h) {
  int r = libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT,
                                  START_ACCESSORY_VENDOR_COMMAND, 0, 0, NULL, 0, 0);
  return r;
}


int ligo_send_accessory_id(libusb_device_handle *h, const ligo_accessory *acc) {
  int r = 0;
  int i;
  unsigned char *acc_str[] = {
    acc->manufacturer, acc->model, acc->description, acc->version, acc->uri, acc->serial
  };

  for (i=0; i < sizeof(acc_str)/sizeof(unsigned char*); i++) {
    int length = strlen((char*)acc_str[i]) + 1; // the \0 char must be sent too
    int bytes_received =
      libusb_control_transfer(h, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT,
                              SEND_STRING_VENDOR_COMMAND, 0, i, acc_str[i], length, 0);
    if (bytes_received != length){
      r = bytes_received; // to return the correct libusb error code
    }
  }
  return r;
}


int ligo_start_accessory_mode(libusb_device *dev, const ligo_accessory *acc) {
  struct libusb_device_handle *h;

  int r = libusb_open(dev, &h);
  if (r == 0) {
    ligo_send_accessory_id(h, acc);
    ligo_send_start_command(h);
    libusb_close(h);
  }
  return r;
}


int ligo_get_protocol_version(libusb_device *dev) {
  struct libusb_device_descriptor desc;
  struct libusb_device_handle *h;

  int i;
  int rc = -1;

  (void)libusb_get_device_descriptor(dev, &desc);

  for(i=0; i < BUILT_IN_VENDOR_COUNT; i++) {
    /* An Android device must have a bDeviceClass == 0 */
    if (desc.bDeviceClass == 0) {
      /* Its idVendor must be included within builtInVendorIds */
      if (desc.idVendor == builtInVendorIds[i] ) {
        /* The GET_PROTOCOL vendor command must return 2 bytes representing the supported AOAP
           version */
        rc = libusb_open(dev, &h);
        if (rc == LIBUSB_SUCCESS) {

          unsigned char data[2];
          memset(data, 0, sizeof(data));

          rc = libusb_control_transfer(h,
                                       LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN,
                                       GET_PROTOCOL_VENDOR_COMMAND, 0, 0, data, 2, 0);
          if ( rc == 2 ) {
            // Get the version value
            rc = data[1] << 8 | data[0];
          }

          libusb_close(h);
        }
      }
    }
  }
  return rc;
}


ssize_t ligo_get_device_list(libusb_context *ctx, libusb_device ***list) {

  ssize_t i, len;
  struct libusb_device **alldevs;
  struct libusb_device **ret;
  int pos = 0;

  len = libusb_get_device_list(NULL, &alldevs);
  ret = calloc(len + 1, sizeof(struct libusb_device *));
  if (!ret) {
    len = LIBUSB_ERROR_NO_MEM;
  }

  for (i = 0; i < len; i++) {
    if(ligo_get_protocol_version(alldevs[i]) == 2) {
      ret[pos] = libusb_ref_device(alldevs[i]);
      pos++;
    }
  }
  libusb_free_device_list(alldevs, 1);
  *list = ret;

  return (int) pos;
}


int ligo_read(unsigned char* buffer, unsigned int timeout) {
  int rc;
  static int transferred;

  rc = libusb_bulk_transfer(iohandle, endpoint_in,
                            buffer,
                            BUFFER_SIZE,
                            &transferred,
                            timeout);

  if (rc == LIBUSB_SUCCESS) {
    rc = transferred;
  }

  return rc;
}


int ligo_write(unsigned char* buffer, unsigned int length, unsigned int timeout) {
  int rc;
  static int transferred;

  rc = libusb_bulk_transfer(iohandle, endpoint_out,
                            buffer,
                            length,
                            &transferred,
                            timeout);

  if (rc == LIBUSB_SUCCESS) {
    rc = transferred;
  }

  return rc;
}


int ligo_init() {
  int rc;

  rc = libusb_init(NULL);
  if (LIBUSB_SUCCESS != rc) {
    libusb_exit(NULL);
    return EXIT_FAILURE;
  }

  return 0;
}


void ligo_exit() {

  libusb_release_interface(iohandle,0);
  libusb_close(iohandle);

  libusb_exit(NULL);
}
