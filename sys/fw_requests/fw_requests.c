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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fw_requests.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* calculate the total length of a message */
uint8_t total_len(struct firmware_packet* pkt)
{
    return pkt->len + ((char*)&pkt->data - (char*)pkt);
}

struct firmware_request* create_request(const char *source_address,
                                        const char *artifact,
                                        enum state initial_state)
{
    static uint16_t last_session_id = 1;
    struct firmware_request* r = (struct firmware_request*)malloc(sizeof(struct firmware_request));
    r->source_address = source_address;
    strcpy(artifact, r->firmware_name);
    r->session_id = last_session_id ++;
    r->state = initial_state;
    r->current_packet = 0;
    r->local_filename = NULL;
    r->fd = 0;
    r->nr_packets = 0;
    printf("INFO: DeploUnitRequest %p from %s for %s created!\n", r, source_address, artifact);
    return r;
}

enum message_processing_error_code process_message(const ipv6_addr_t *source_address,
                                                   uint16_t msg_len,
                                                   const uint8_t* msg,
                                                   const struct request_processing_callback* callbacks)
{
    uint16_t crc_c;
    char addr_str[IPV6_ADDR_MAX_STR_LEN];
    struct firmware_packet* pkt = (struct firmware_packet*)msg;
    uint16_t crc = pkt->crc; /* first check if it is a valid packet (no transmission errors) */

    pkt->crc = 0;
    crc_c = crc16_ccitt_calc((unsigned char*)pkt, msg_len);

    DEBUG("[fw_requests] Data received from %s with state %d\n",
            ipv6_addr_to_str(addr_str, source_address, sizeof(addr_str)), pkt->cmd);

    /* stop processing if the packet has errors */
    if (crc != crc_c) {
        DEBUG("[fw_requests] Wrong CRC!\n");
        return WRONG_CRC;
    }

    /* process message */
    switch (pkt->cmd) {
        case RESPONSE_SUMMARY:
            callbacks->on_new_summary(pkt->data.summary.session_id,
                    pkt->data.summary.nr_chunks);
            break;

        case RESPONSE_CHUNK:
            callbacks->on_new_chunk(pkt->data.chunk.chunk_id,
                    pkt->len - sizeof(uint16_t), pkt->data.chunk.data);
            break;

        case RESPONSE_ACK_LOOKING_FOR_PACKET:
            callbacks->on_ack_firmware_request();
            break;

        case GET_ARTIFACT:
            callbacks->on_firmware_request(source_address,
                    pkt->data.get_artifact.artifact);
            break;

        case GET_CHUNK:
            callbacks->on_chunk_request(pkt->data.get_chunk.session_id,
                    pkt->data.get_chunk.chunk_id);
            break;

        case GET_REPO_ADDR:
            callbacks->on_repo_request(source_address,
                    &pkt->data.get_repo.src,
                    pkt->data.get_repo.fw_name);
            break;

        case RESPONSE_REPO_ADDR:
            callbacks->on_repo_addr(source_address,
                    pkt->data.get_artifact.artifact);
            break;

        case END_CHUNKS:
            callbacks->on_end_chunks();
            break;

        default:
            int tt = pkt->cmd;
            DEBUG("[fw_requests] ERROR: Unknown type : %d\n", tt);
            return UNKNOWN_MESSAGE_TYPE;
    }

    return NONE;
}

void build_get_artifact_packet(struct firmware_packet* pkt, const char* artifact)
{
    pkt->crc = 0;
    pkt->cmd = GET_ARTIFACT;
    pkt->len = strlen(artifact) + 1;
    strcpy(pkt->data.get_artifact.artifact, artifact);
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_get_chunk_packet(struct firmware_packet* pkt,
                            uint16_t session_id, uint16_t chunk_id)
{
    pkt->crc = 0;
    pkt->cmd = GET_CHUNK;
    pkt->len = sizeof(uint16_t)*2;
    pkt->data.get_chunk.session_id = session_id;
    pkt->data.get_chunk.chunk_id = chunk_id;
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_summary_packet(struct firmware_packet* pkt,
                          uint16_t session_id, uint16_t nr_chunks)
{
    pkt->crc = 0;
    pkt->cmd = RESPONSE_SUMMARY;
    pkt->len = sizeof(uint16_t)*2;
    pkt->data.summary.session_id = session_id;
    pkt->data.summary.nr_chunks = nr_chunks;
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_chunk_packet(struct firmware_packet* pkt,
                        uint16_t chunk_id, uint16_t msg_len, const uint8_t* msg)
{
    pkt->crc = 0;
    pkt->cmd = RESPONSE_CHUNK;
    pkt->len = sizeof(uint16_t) + msg_len;
    pkt->data.chunk.chunk_id = chunk_id;
    memcpy(pkt->data.chunk.data, msg, msg_len);
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_ack_packet(struct firmware_packet* pkt)
{
    pkt->crc = 0;
    pkt->cmd = RESPONSE_ACK_LOOKING_FOR_PACKET;
    pkt->len = 0;
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_repo_addr(struct firmware_packet* pkt, char *name)
{
    pkt->crc = 0;
    pkt->cmd = RESPONSE_REPO_ADDR;
    strcpy(pkt->data.get_artifact.artifact, name);
    pkt->len = strlen(name) + 1;
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_req_repo_addr(struct firmware_packet *pkt, ipv6_addr_t *src, char *name)
{
    DEBUG("[fw_requests] Building packet to request repository\n");
    pkt->crc = 0;
    pkt->cmd = GET_REPO_ADDR;
    pkt->len = sizeof(ipv6_addr_t) + strlen(name) + 1;
    memcpy(&pkt->data.get_repo.src, src, sizeof(ipv6_addr_t));
    strcpy(pkt->data.get_repo.fw_name, name);
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

void build_end_chunks_packet(struct firmware_packet* pkt)
{
    DEBUG("[fw_requests] Building packet to finish chunk transmission\n");
    pkt->crc = 0;
    pkt->cmd = END_CHUNKS;
    pkt->len = 0;
    pkt->crc = crc16_ccitt_calc((unsigned char*)pkt, total_len(pkt));
}

/* dealing with requests */
struct firmware_request* find_request_by_source(clist_node_t *list,
                                                const char *source_address,
                                                const char* artifact)
{
    struct firmware_request* r;
    clist_node_t *node = list->next;

    if (!node) {
        puts("[fw_requests] Request list empty\n");
        return NULL;
    }

    do {
        node = node->next;
        r = (struct firmware_request*) node;
        if (strcmp(r->firmware_name, artifact) == 0 &&
            strcmp(r->source_address,source_address) == 0) {
            return r;
        }
    } while (node != list->next);

    DEBUG("[fw_requests] Firmware request for %s not found!\n", artifact);
    return NULL;
}

struct firmware_request* find_request_by_session(clist_node_t list, uint16_t session_id)
{
    struct firmware_request* r;
    clist_node_t *node = list->next;

    if (!node) {
        puts("[fw_requests] Request list empty\n");
        return NULL;
    }

    do {
        node = node->next;
        r = (struct firmware_request*) node;
        if (r->session_id == session_id) {
            return r;
        }
    } while (node != list->next);

    printf("[fw_requests] Firmware request for session %d not found!\n", session_id);
    return NULL;
}

struct firmware_request* find_request_by_artifact(clist_node_t list, const char* artifact)
{
    struct firmware_request* r;
    clist_node_t *node = list->next;

    if (!node) {
        puts("[fw_requests] Request list empty\n");
        return NULL;
    }

    do {
        node = node->next;
        r = (struct firmware_request*) node;
        if (strcmp(r->firmware_name, artifact) == 0) {
            return r;
        }
    } while (node != list->next);

    DEBUG("[fw_requests] Firmware request for %s not found!\n", artifact);
    return NULL;
}

struct map_entry* find_by_artifact(const clist_node_t *list, const char* name)
{
    struct map_entry* r = NULL;
    clist_node_t *node = list->next;

    if (!node) {
        puts("list empty");
        return NULL;
    }

    do {
        node = node->next;
        r = (struct map_entry*) node;
        if (strcmp(r->artifact, name) == 0) {
            return r;
        }
    } while (node != list->next);

    DEBUG("[fw_requests] artifact %s not found in server!\n", name);
    return NULL;
}

void dispose_request(struct firmware_request* r)
{
    printf("INFO: Deleting DeployUnitRequest %p for %s\n", r, r->firmware_name);

    if (r->firmware_name != NULL) {
        free(r->firmware_name);
    }

    if (r->local_filename != NULL) {
        free(r->local_filename);
    }

    if (r->source_address != NULL) {
        free(r->source_address);
    }

    free(r);
}
