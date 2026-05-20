/*
 * Mock npcap SDK headers for CI builds.
 * This file provides stub declarations for Packet32 functions.
 * For real builds, use the npcap SDK downloaded via scripts/download_npcap_sdk.ps1.
 */
#ifndef PACKET32_H
#define PACKET32_H

#ifdef __cplusplus
extern "C" {
#endif

/* Mock types */
typedef struct _ADAPTER{} ADAPTER, *LPADAPTER;
typedef struct _PACKET{} PACKET, *LPPACKET;
typedef struct _PACKET_OID_DATA{} PACKET_OID_DATA, *LPPACKET_OID_DATA;

/* Mock functions */
LPADAPTER PacketOpenAdapter(char *AdapterName);
BOOL PacketGetVersion(LPADAPTER AdapterObject, PWORD VersionNumber);
BOOL PacketSendPacket(LPADAPTER AdapterObject, LPPACKET Packet, BOOL Sync);
BOOL PacketSetReadTimeout(LPADAPTER AdapterObject, int Timeout);
void PacketFreePacket(LPPACKET Packet);
BOOL PacketReceivePacket(LPADAPTER AdapterObject, LPPACKET Packet, BOOL Sync);
BOOL PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program *fp, int install);
BOOL PacketSetLoopbackBehavior(LPADAPTER AdapterObject, UINT LoopbackBehavior);
void PacketSetHwFilter(LPADAPTER AdapterObject, ULONG Filter);
UINT PacketGetNumStats(LPADAPTER AdapterObject);
UINT PacketStats(LPADAPTER AdapterObject, struct bpf_stat *s);
ULONG PacketRequest(LPADAPTER AdapterObject, BOOL Set, PACKET_OID_DATA *OidData);

#ifdef __cplusplus
}
#endif

#endif /* PACKET32_H */
