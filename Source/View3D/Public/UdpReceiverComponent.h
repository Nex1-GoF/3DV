// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Common/UdpSocketBuilder.h"


#include "UdpReceiverComponent.generated.h"

class AMissileManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VIEW3D_API UUdpReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUdpReceiverComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 수신할 포트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP")
	int32 ListenPort = 7777;

private:
	FSocket* ListenSocket = nullptr;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	/** 메시지 처리 */
	void OnUdpMessageReceived(const FArrayReaderPtr& Data, const FIPv4Endpoint& Endpoint);

	/** 미사일 매니저 캐싱 */
	AMissileManager* CachedManager = nullptr;
};
