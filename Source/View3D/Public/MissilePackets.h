#pragma once

#include "CoreMinimal.h"

#pragma pack(push, 1)

// ===================================================================
//  공통 헤더 (C# Deserialize와 100% 동일)
// ===================================================================
struct FHeaderPacket
{
    char    SrcId[4];
    char    DestId[4];
    uint32  Seq;
    uint8   MsgSize;

    void FromBytes(const uint8* Data)
    {
        memcpy(SrcId, Data + 0, 4);
        memcpy(DestId, Data + 4, 4);

        Seq = ((uint32)Data[8] << 24) |
            ((uint32)Data[9] << 16) |
            ((uint32)Data[10] << 8) |
            (uint32)Data[11];

        MsgSize = Data[12];
    }
};

// ===================================================================
//  발사 신호
// ===================================================================
struct FMslLaunchSignal
{

    char    MissileId[4];
    uint16  MslYaw;

    void FromBytes(const uint8* Data)
    {
        memcpy(MissileId, Data, 4);

        memcpy(&MslYaw, Data+4, 2);

        /*MslYaw = ((uint16)Data[4] << 8) |
            ((uint16)Data[5]);*/
    }
};

// ===================================================================
//  실시간 정보 패킷
// ===================================================================
struct FMslInfoData
{
    char    MissileId[4];
    uint16  MslYaw;
    uint8   TelemetryStatus;
    char    FlightStatus;
    uint32  TargetDistance;
    uint16  TargetYaw;

    void FromBytes(const uint8* Data)
    {
        memcpy(MissileId, Data, 4);

        MslYaw = ((uint16)Data[4] << 8) |
            ((uint16)Data[5]);

        TelemetryStatus = Data[6];
        FlightStatus = (char)Data[7];

        TargetDistance = ((uint32)Data[8] << 24) |
            ((uint32)Data[9] << 16) |
            ((uint32)Data[10] << 8) |
            (uint32)Data[11];

        TargetYaw = ((uint16)Data[12] << 8) |
            ((uint16)Data[13]);
    }
};

// ===================================================================
//  폭발 신호
// ===================================================================
struct FMslDetonationSignal
{
    char MissileId[4];

    void FromBytes(const uint8* Data)
    {
        memcpy(MissileId, Data, 4);
    }
};

#pragma pack(pop)