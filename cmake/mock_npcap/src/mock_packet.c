#include <winsock2.h>
#include <windows.h>

// Rezolvă eroarea '_Post_invalid_' pentru compilatorul MinGW
#ifndef _Post_invalid_
#define _Post_invalid_
#endif

#include "Packet32.h"

LPADAPTER PacketOpenAdapter(const CHAR *AdapterName) { return NULL; }
VOID PacketCloseAdapter(LPADAPTER lpAdapter) {}
LPCSTR PacketGetVersion() { return "Mock Packet32"; }
BOOLEAN PacketSendPacket(LPADAPTER AdapterObject, LPPACKET pPacket, BOOLEAN Sync) { return 1; }
BOOLEAN PacketSetReadTimeout(LPADAPTER AdapterObject, int timeout) { return 1; }
BOOLEAN PacketReceivePacket(LPADAPTER AdapterObject, LPPACKET lpPacket, BOOLEAN Sync) { return 0; }
BOOLEAN PacketSetBpf(LPADAPTER AdapterObject, struct bpf_program* fp) { return 1; }
BOOLEAN PacketSetLoopbackBehavior(LPADAPTER AdapterObject, UINT LoopbackBehavior) { return 1; }
BOOLEAN PacketSetHwFilter(LPADAPTER AdapterObject, ULONG Filter) { return 1; }
BOOLEAN PacketRequest(LPADAPTER AdapterObject, BOOLEAN Set, PPACKET_OID_DATA OidData) { return 1; }