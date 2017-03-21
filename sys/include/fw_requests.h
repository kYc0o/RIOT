/*
 * Copyright (C) 2017 Francisco Acosta <francisco.acosta@inria.fr>
 *                    Inti Gonzalez
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  sys_fw_requests
 * @{
 *
 * @file
 * @brief       Firmware requests format and helpers
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Inti Gonzalez
 */

#ifndef FW_REQUESTS_H
#define FW_REQUESTS_H

#include "net/ipv6/addr.h"
#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REQUEST_PACKET_SIZE        (58)
#define UDP_PORT                   (8888)
#define MAX_REQUESTS               (20)

/* a request may be in many states */
enum state {
    DONE,
    WAITING_FOR_SUMMARY,
    POSSIBLY_WAITING_FOR_SUMMARY,
    RECEIVING_CHUNKS,
    SENDING_SUMMARY,
    SENDING_CHUNKS,
    WAITING_FOR_LOCATION
};

/* the request serving protocol has many message types */
enum message_type {
    /* command types */
    END_CHUNKS=2,
    GET_ARTIFACT=3, // the message request an artifact
    GET_CHUNK=4, // the message request a packet

    /* response types */
    RESPONSE_SUMMARY=5, // the message is a response to GET_ARTIFACT when the artifact is locally available
    RESPONSE_CHUNK=6, // the message is a response to GET_CHUNK
    RESPONSE_ACK_LOOKING_FOR_PACKET=7, // the message is a response to GET_ARTIFACT when the artifact must be requested to another server
    GET_REPO_ADDR=8, // Get repository from parent
    RESPONSE_REPO_ADDR=9
};

enum message_processing_error_code {
    NONE = 0,
    WRONG_CRC = 1,
    UNKNOWN_MESSAGE_TYPE = 2
};

/* mapping from artifact name to file name*/
struct map_entry {
    struct map_entry* next;
    char* filename;
    char* artifact;
};

/* represent a request */
struct firmware_request {
    /* if we want to put them on a list */
    struct firmware_request* next;
    /* processing state */
    enum state current_state;
    /* source address of this request (can be NULL) */
    char source_address[IPV6_ADDR_MAX_STR_LEN];
    /* deploy unit being requested */
    char firmware_name[32];
    /* local file with the deploy unit */
    char local_filename[32];
    /* a file descriptor to the local file with the deploy unit */
    int fd;
    /* session id: used in multithread servers */
    uint16_t session_id;
    /* current packet being processed */
    uint16_t current_packet;
    /* number of packets in the file */
    uint16_t nr_packets;
};

struct artifact_request {
    void* next;
    char * artifact;
    void* user_data;
    void (*notification_routine)(const char* artifact, const char* filename, void* user_data);
};

/* general packet */
struct firmware_packet {
    uint16_t crc;
    uint8_t cmd;
    /* length of the data part (what is left) */
    uint8_t len;
    union {
        /* general packet */
        uint8_t data[REQUEST_PACKET_SIZE];
        /* used for GET_ARTIFACT messages */
        struct {
            char artifact[REQUEST_PACKET_SIZE];
        } get_artifact;
        /* summary packet */
        struct {
            uint16_t session_id;
            uint16_t nr_chunks;
        } summary;
        /* used to request a packet */
        struct {
            uint16_t session_id;
            uint16_t chunk_id;
        } get_chunk;
        /* fill chunk */
        struct {
            uint16_t chunk_id;
            uint8_t data[REQUEST_PACKET_SIZE];
        } chunk;

        /* relevant ip6 addresses */
        struct {
            ipv6_addr_t src;
            char fw_name[32];
        } get_repo;
    } data;

};

/* contains the callbacks to process different message types */
struct request_processing_callback {
    void (*on_new_summary)(uint16_t, uint16_t);
    void (*on_new_chunk)(uint16_t, uint16_t, const uint8_t*);
    void (*on_ack_firmware_request)(void);
    void (*on_firmware_request)(const ipv6_addr_t*, const char*);
    void (*on_chunk_request)(uint16_t, uint16_t);
    void (*on_repo_request)(const ipv6_addr_t*, ipv6_addr_t*, char*);
    void (*on_repo_addr)(const ipv6_addr_t*, char*);
    void (*on_end_chunks)(void);
};

struct map_entry *find_by_artifact(clist_node_t *list, const char *artifact);

/* calculate the total length of a message */
uint8_t total_len(struct firmware_packet *pkt);

/**
	\brief Process an incoming message
	\ret   0 if everything is Ok, a negative value otherwise
 */
enum message_processing_error_code process_message(const ipv6_addr_t *source_address,
        uint16_t msg_len,
        const uint8_t *msg,
        const struct request_processing_callback *callbacks);

/* create packets */
void build_get_artifact_packet(struct firmware_packet *dst, const char *artifact);
void build_get_chunk_packet(struct firmware_packet *dst, uint16_t session_id, uint16_t chunk_id);
void build_summary_packet(struct firmware_packet *dst, uint16_t session_id, uint16_t nr_chunks);
void build_chunk_packet(struct firmware_packet *dst, uint16_t chunk_id, uint16_t msg_len, const uint8_t* msg);
void build_ack_packet(struct firmware_packet *dst);
void build_routes_packet(struct firmware_packet *pkt, uint16_t *addrs, uint8_t length);
void build_end_chunks_packet(struct firmware_packet *dst);

/* dealing with requests */
struct firmware_request* create_request(const char *source_address, const char* artifact, enum state initial_state);
struct firmware_request* find_request_by_source(clist_node_t *list, const char *source_address, const char* artifact);
struct firmware_request* find_request_by_session(clist_node_t *list, uint16_t session_id);
struct firmware_request* find_request_by_artifact(clist_node_t *list, const char* artifact);

#ifdef __cplusplus
}
#endif

#endif /* FW_REQUESTS_H */
