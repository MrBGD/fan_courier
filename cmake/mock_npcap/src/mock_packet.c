/*
 * Mock Packet.dll stub implementation for CI builds.
 * This provides stub implementations of Packet32 functions
 * required by the pre-built PcapPlusPlus libraries.
 * For real builds, use the npcap SDK downloaded via scripts/download_npcap_sdk.ps1.
 */
#include "Packet32.h"
#include <stdio.h>
#include <stdlib.h>

/* Stub implementations */

LPADAPTER PacketOpenAdapter(char *AdapterName) {
    return (LPADAPTER)calloc(1, sizeof(ADAPTER));
}

BOOL PacketGetVersion(LPADAPTER AdapterObject, PWORD VersionNumber) {
    if (VersionNumber) *VersionNumber = 0x0202; /* v2.2 */
    return TRUE;
}

BOOL PacketSendPacket(LPADAPTER AdapterObject, LPPACKET Packet, BOOL Sync) {
    return TRUE;
}

BOOL PacketSetReadTimeout(LPADAPTER AdapterObject, int Timeout) {
    return TRUE;
}

void PacketFreePacket(LPPACKET Packet) {
    free(Packet);
}

BOOL PacketReceivePacket(LPADAPTER AdapterObject, LPPACKET Packet, BOOL Sync) {
    return FALSE;
}

BOOL PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program *fp, int install) {
    return TRUE;
}

BOOL PacketSetLoopbackBehavior(LPADAPTER AdapterObject, UINT LoopbackBehavior) {
    return TRUE;
}

void PacketSetHwFilter(LPADAPTER AdapterObject, ULONG Filter) {}

UINT PacketGetNumStats(LPADAPTER AdapterObject) { return 0; }

UINT PacketStats(LPADAPTER AdapterObject, struct bpf_stat *s) { return 0; }

ULONG PacketRequest(LPADAPTER AdapterObject, BOOL Set, PACKET_OID_DATA *OidData) {
    return 0;
}
