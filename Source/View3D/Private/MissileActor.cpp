// Fill out your copyright notice in the Description page of Project Settings.


#include "MissileActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
AMissileActor::AMissileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	MisslieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MisslieMesh->SetupAttachment(RootComp);

	CameraComp = CreateDefaultSubobject<USceneComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComp);


	MisslieCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	if (MisslieCaptureComponent != nullptr) {
		MisslieCaptureComponent->SetupAttachment(CameraComp);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("[Missile] MisslieCaptureComponent NewObject FAILED"));
	}
	MisslieCaptureComponent->SetupAttachment(CameraComp);

	CameraComp->SetRelativeLocation(FVector(-300.f, 0.f, 100.f));


	//매 프레임 캡처 대신 필요 시 호출 (성능 ↑)
	/*MisslieCaptureComponent->bCaptureEveryFrame = false;
	MisslieCaptureComponent->bCaptureOnMovement = false;*/

}

// Called when the game starts or when spawned
void AMissileActor::BeginPlay()
{
	Super::BeginPlay();
	//RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	//RenderTarget->InitAutoFormat(960, 540);  // 원하는 해상도로 설정
	//RenderTarget->ClearColor = FLinearColor::Black;
	//RenderTarget->UpdateResource();

	//// SceneCapture에 연결
	//if (MisslieCaptureComponent != nullptr) {
	//	MisslieCaptureComponent->TextureTarget = RenderTarget; 
	//	UE_LOG(LogTemp, Warning,
	//		TEXT("[Missile] RenderTargets 호출됨 RT:%p"),
	//		RenderTarget); 
	//	UE_LOG(LogTemp, Warning,
	//		TEXT("[Missile] TextureTarget 호출됨 TT:%p"),
	//		*(MisslieCaptureComponent->TextureTarget));
	//}
	//else {
	//	UE_LOG(LogTemp, Error, TEXT("[Missile] MisslieCaptureComponent is nullptr"));
	//}

	MisslieCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
}


// Called every frame
void AMissileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	static float Accum = 0.f;
	Accum += DeltaTime;
	if (Accum > 0.01f) {
		MisslieCaptureComponent->CaptureSceneDeferred();
		Accum = 0.01f;
	}
	//const float SmoothSpeed = 5.0f;

	//// 보간 필수
	//float TargetPitchFixed = ShortestAngle(SmoothPitch, TargetPitch);
	//float TargetYawFixed = ShortestAngle(SmoothYaw, TargetYaw);

	//SmoothPitch = FMath::FInterpTo(SmoothPitch, TargetPitchFixed, DeltaTime, SmoothSpeed);
	//SmoothYaw = FMath::FInterpTo(SmoothYaw, TargetYawFixed, DeltaTime, SmoothSpeed);

	//// 최종 회전
	//FQuat QYaw = FQuat(FVector::UpVector, FMath::DegreesToRadians(SmoothYaw));
	//FQuat QPitch = FQuat(FVector::RightVector, FMath::DegreesToRadians(SmoothPitch));

	//FQuat FinalRot = QYaw * QPitch;
	FRotator newrot=FRotator(TargetYaw, TargetPitch,0 );
	MisslieMesh->SetWorldRotation(newrot);
}

void AMissileActor::ApplyAttitude(float InPitch, float InYaw)
{
	TargetPitch = InPitch;
	TargetYaw = InYaw;

	/*UE_LOG(LogTemp, Warning, TEXT("Target Pitch=%f, Yaw=%f"),
		TargetPitch, TargetYaw);*/
}

void AMissileActor::SetMissileID(uint8 InID)
{
	MissileID = InID; 
	ApplyRenderTargetByID();

}

void AMissileActor::ApplyRenderTargetByID()
{
	if (MissileID <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid MissileID=%d"), MissileID);
		return;
	}

	FString Path = FString::Printf(
		TEXT("/Game/RenderTargets/RT_MissileCam%d.RT_MissileCam%d"),
		MissileID, MissileID
	);

	UTextureRenderTarget2D* RT =
		LoadObject<UTextureRenderTarget2D>(nullptr, *Path);

	if (!RT)
	{
		UE_LOG(LogTemp, Error, TEXT("Missile %d : Load Fail %s"),
			MissileID, *Path);
		return;
	}

	if (MisslieCaptureComponent)
		MisslieCaptureComponent->TextureTarget = RT;

	UE_LOG(LogTemp, Warning,
		TEXT("Missile %d assigned RT=%p"), MissileID, RT);
}