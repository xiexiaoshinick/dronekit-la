#include "mavlink_reader.h"

#include <errno.h>
#include <stdio.h> // for perror
#include <stdlib.h> // for abort
#include "la-log.h"

#include <unistd.h> // for usleep

#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* This is used to prevent swamping the log with error messages if
   something unexpected happens.
   Returns -1 if we cannot log an error now, or returns the number of
   messages skipped due to rate limiting if we can, i.e. a return of
   2 means log, and we have skipped 2 messages due to rate limiting. */
int MAVLink_Reader::can_log_error()
{
    unsigned ret_val;
    uint64_t now_us = clock_gettime_us(CLOCK_MONOTONIC);
    if ((now_us - err_time_us) < err_interval_us) {
        /* can't log */
        err_skipped++;
        return -1;
    }
    /* yes; say we can and set err_time_us assuming we do log something */
    err_time_us = now_us;
    ret_val = err_skipped;
    err_skipped = 0;
    return ret_val;
}


void MAVLink_Reader::handle_message_received(uint64_t timestamp, mavlink_message_t msg)
{
    // ::fprintf(stderr, "msg.msgid=%u\n", msg.msgid);
    switch(msg.msgid) {
    case MAVLINK_MSG_ID_AHRS2: {
        mavlink_ahrs2_t decoded;
        mavlink_msg_ahrs2_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_ATTITUDE: {
        mavlink_attitude_t decoded;
        mavlink_msg_attitude_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_EKF_STATUS_REPORT: {
        mavlink_ekf_status_report_t decoded;
        mavlink_msg_ekf_status_report_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_GPS_RAW_INT: {
        mavlink_gps_raw_int_t decoded;
        mavlink_msg_gps_raw_int_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_HEARTBEAT: {
        mavlink_heartbeat_t decoded;
        mavlink_msg_heartbeat_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_MOUNT_STATUS: {
        mavlink_mount_status_t decoded;
        mavlink_msg_mount_status_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT: {
        mavlink_nav_controller_output_t decoded;
        mavlink_msg_nav_controller_output_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_PARAM_VALUE: {
        mavlink_param_value_t decoded;
        mavlink_msg_param_value_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_REMOTE_LOG_DATA_BLOCK: {
        mavlink_remote_log_data_block_t decoded;
        abort();
        mavlink_msg_remote_log_data_block_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW: {
        mavlink_servo_output_raw_t decoded;
        mavlink_msg_servo_output_raw_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_STATUSTEXT: {
        mavlink_statustext_t decoded;
        mavlink_msg_statustext_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_SYS_STATUS: {
        mavlink_sys_status_t decoded;
        mavlink_msg_sys_status_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    case MAVLINK_MSG_ID_VFR_HUD: {
        mavlink_vfr_hud_t decoded;
        mavlink_msg_vfr_hud_decode(&msg, &decoded);
        handle_decoded_message_received(timestamp, decoded); // template in .h
        break;
    }
    }
}

uint32_t MAVLink_Reader::feed(const uint8_t *buf, const uint32_t len)
{
    for (uint32_t i=0; i<len; i++) {
        // ::printf("Read (%d)\n", i);
        if (is_tlog() && !done_timestamp) {
            timestamp <<= 8;
            timestamp |= (uint8_t)(buf[i]);
            if (timestamp_offset++ == 7) {
                done_timestamp = true;
                // ::printf("timestamp (%lu)\n", timestamp);
                timestamp_offset = 0;
            }
        } else {
            if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &mav_msg, &mav_status)) {
                if (!is_tlog()) {
                    timestamp = now();
                }
                // printf("message received at %d (parse_errors=%u)\n", i, mav_status.parse_error);
                handle_message_received(timestamp, mav_msg);
                packet_count++;
                done_timestamp = false;
                timestamp = 0;
            } else {
                // printf("   %u state: %u\n", i, mav_status.parse_state);
            }
        }
    }
    return len; // at the moment we parse everything we receive
}

void MAVLink_Reader::end_of_log()
{
    for(int i=0; i<next_message_handler; i++) {
        message_handler[i]->end_of_log(packet_count);
    }
}
