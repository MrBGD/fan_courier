/*
 * Mock Packet32.h for CI builds.
 * Stub declarations aligned with mock_packet.c implementations.
 */
#ifndef PACKET32_H
#define PACKET32_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _Post_invalid_
#define _Post_invalid_
#endif

struct bpf_program;

typedef struct _ADAPTER       ADAPTER,       *LPADAPTER;
typedef struct _PACKET        PACKET,        *LPPACKET;
typedef struct _PACKET_OID_DATA PACKET_OID_DATA, *PPACKET_OID_DATA;

LPADAPTER PacketOpenAdapter(const CHAR *AdapterName);
VOID      PacketCloseAdapter(LPADAPTER lpAdapter);
LPCSTR    PacketGetVersion(void);
BOOLEAN   PacketSendPacket(LPADAPTER AdapterObject, LPPACKET pPacket, BOOLEAN Sync);
BOOLEAN   PacketSetReadTimeout(LPADAPTER AdapterObject, int timeout);
BOOLEAN   PacketReceivePacket(LPADAPTER AdapterObject, LPPACKET lpPacket, BOOLEAN Sync);
BOOLEAN   PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program *fp);
BOOLEAN   PacketSetLoopbackBehavior(LPADAPTER AdapterObject, UINT LoopbackBehavior);
BOOLEAN   PacketSetHwFilter(LPADAPTER AdapterObject, ULONG Filter);
BOOLEAN   PacketRequest(LPADAPTER AdapterObject, BOOLEAN Set, PPACKET_OID_DATA OidData);

#ifdef __cplusplus
}
#endif

#endif /* PACKET32_H */
