// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissileActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
UCLASS()
class VIEW3D_API AMissileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* RootComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* CameraComp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* MisslieMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneCaptureComponent2D* MisslieCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UTextureRenderTarget2D* ExternalRenderTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Missile")
	uint8 MissileID = 1;

	/** 네트워크로 받은 순수 Raw Pitch/Yaw */
	float TargetPitch = 0.f;
	float TargetYaw = 0.f;

	/** 실제 렌더링할 보간된 Pitch/Yaw */
	float SmoothPitch = 0.f;
	float SmoothYaw = 0.f;

	/** 네트워크 수신 적용 */
	void ApplyAttitude(float InPitch, float InYaw);

	void SetMissileID(uint8 InID);

	void ApplyRenderTargetByID();

};
