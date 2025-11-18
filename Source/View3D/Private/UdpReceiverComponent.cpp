#include "UdpReceiverComponent.h"
#include "MissileManager.h"
#include "Kismet/GameplayStatics.h"

UUdpReceiverComponent::UUdpReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUdpReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	FIPv4Endpoint Endpoint(FIPv4Address::Any, ListenPort);

	ListenSocket =
		FUdpSocketBuilder(TEXT("MissileUDPReceiver"))
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
}

void UUdpReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UDPReceiver)
	{
		UDPReceiver->Stop();
		delete UDPReceiver;
		UDPReceiver = nullptr;
	}

	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
		ListenSocket = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UUdpReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CachedManager)
	{
		CachedManager = Cast<AMissileManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMissileManager::StaticClass()));
	}
}

void UUdpReceiverComponent::OnUdpMessageReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Endpoint)
{
	if (Data->Num() < 5)
		return;
	uint8 ID = (*Data)[0];
	int16 PitchRaw = *((int16*)(&(*Data)[1]));
	int16 YawRaw = *((int16*)(&(*Data)[3]));

	float Pitch = PitchRaw / 10.0f;
	float Yaw = YawRaw / 10.0f;

	if (!CachedManager)
		return;
	CachedManager->ApplyAttitude(ID, Pitch, Yaw);
}