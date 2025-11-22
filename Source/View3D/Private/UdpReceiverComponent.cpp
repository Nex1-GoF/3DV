#include "UdpReceiverComponent.h"
#include "MissileManager.h"
#include "MissilePackets.h"
#include "Kismet/GameplayStatics.h"

UUdpReceiverComponent::UUdpReceiverComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UUdpReceiverComponent::BeginPlay()
{
    Super::BeginPlay();

    FIPv4Endpoint Endpoint(FIPv4Address::Any, ListenPort);

    ListenSocket = FUdpSocketBuilder(TEXT("MissileUDPReceiver"))
        .AsNonBlocking()
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .WithReceiveBufferSize(2 * 1024 * 1024);

    if (!ListenSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("UDP Socket 생성 실패!"));
        return;
    }

    UDPReceiver = new FUdpSocketReceiver(
        ListenSocket,
        FTimespan::FromMilliseconds(1),
        TEXT("UDPReceiverThread")
    );

    UDPReceiver->OnDataReceived().BindUObject(this, &UUdpReceiverComponent::OnUdpMessageReceived);
    UDPReceiver->Start();
    UE_LOG(LogTemp, Error, TEXT("UDP Socket Create!"));
}

void UUdpReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UDPReceiver)
    {

        UE_LOG(LogTemp, Error, TEXT("UDP Socket Stop!"));
        UDPReceiver->Stop();
        delete UDPReceiver;
        UDPReceiver = nullptr;
    }

    if (ListenSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("UDP Socket Stop!"));
        ListenSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
        ListenSocket = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void UUdpReceiverComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!CachedManager)
    {
        CachedManager = Cast<AMissileManager>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AMissileManager::StaticClass()));
    }
}

void UUdpReceiverComponent::OnUdpMessageReceived(
    const FArrayReaderPtr& Data,
    const FIPv4Endpoint& Endpoint)
{   
    UE_LOG(LogTemp, Error, TEXT("UDPRECEIVED!"));
    if (!CachedManager) return;

    const uint8* Bytes = Data->GetData();
    int32 PacketSize = Data->Num();

    if (PacketSize < 13)
    {
        UE_LOG(LogTemp, Warning, TEXT("패킷 너무 짧음! (%d bytes)"), PacketSize);
        return;
    }

    //--------------------------------------------------------------------
    // 1) 공통 헤더 파싱
    //--------------------------------------------------------------------
    FHeaderPacket Header{};
    Header.FromBytes(Bytes);

    FString Src = FString(4, Header.SrcId);
    FString Dst = FString(4, Header.DestId);

    UE_LOG(LogTemp, Warning, TEXT("Header: Src=%s Dest=%s Seq=%u MsgSize=%u"),
        *Src, *Dst, Header.Seq, Header.MsgSize);


    //--------------------------------------------------------------------
    // 2) 메시지 본문 파싱 (헤더 바로 뒤부터)
    //--------------------------------------------------------------------
    const uint8* Body = Bytes + sizeof(FHeaderPacket);

    // 메시지 타입은 DestId 로 구분한다고 가정
    FString DestId = FString(4, Header.DestId);
    uint32  Seq = Header.Seq;
    //---------------------------------------------------------
    // LAUNCH (예: DestId == "MLCH")
    //---------------------------------------------------------
    if (Seq == 1)
    {
        if (PacketSize < sizeof(FHeaderPacket) + 6)
        {
            UE_LOG(LogTemp, Error, TEXT("LaunchSignal 크기 오류!"));
            return;
        }
        FMslLaunchSignal Packet{};
        Packet.FromBytes(Body);

        int32 ID = FCStringAnsi::Atoi(Packet.MissileId);

        UE_LOG(LogTemp, Warning, TEXT("PacketYaw:%d"),Packet.MslYaw);
        float Yaw = Packet.MslYaw * 0.01f;

        AsyncTask(ENamedThreads::GameThread, [this,ID,Yaw]()
            {
                CachedManager->LaunchMissile(ID, Yaw);
            });
    }

    //---------------------------------------------------------
    // INFO (명령: 실시간 미사일 상태)
    //---------------------------------------------------------
    else if (Seq == 2)
    {
        if (PacketSize < sizeof(FHeaderPacket) + 14)
        {
            UE_LOG(LogTemp, Error, TEXT("MslInfo 패킷 짧음"));
            return;
        }

        FMslInfoData Packet{};
        Packet.FromBytes(Body);

        int32 ID = FCStringAnsi::Atoi(Packet.MissileId);

        float Yaw = Packet.MslYaw * 0.01f;
        float Dist = Packet.TargetDistance;
        float TargetYaw = Packet.TargetYaw;
        if (Packet.FlightStatus == 4) {

            AsyncTask(ENamedThreads::GameThread, [this, ID, Dist, TargetYaw]()
                {
                    CachedManager->TerminalMissile(ID);
                    CachedManager->UpdateTargetDistance(ID, Dist, TargetYaw);
                });
            return;
        }
        CachedManager->ApplyAttitude(ID, 0.f, Yaw);
        uint8 tel=Packet.TelemetryStatus;
        CachedManager->UpdateTelemetry(ID, tel);
        //CachedManager->UpdateTargetDistance(ID, Dist);
    }

    //---------------------------------------------------------
    // DETONATION (미사일 폭발)
    //---------------------------------------------------------
    else if (Seq == 3)
    {
        FMslDetonationSignal Packet{};
        Packet.FromBytes(Body);

        int32 ID = FCStringAnsi::Atoi(Packet.MissileId);
        AsyncTask(ENamedThreads::GameThread, [this, ID]()
            {
                CachedManager->Explode(ID);
            });
    }

    //---------------------------------------------------------
    // 알 수 없는 타입
    //---------------------------------------------------------
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Unknown DestId: %s"), *DestId);
    }
}