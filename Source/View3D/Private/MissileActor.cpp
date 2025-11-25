#include "MissileActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Target.h"
#include "MonitorPlayerController.h"
#include "WBP_MissileMonitorBase.h"

AMissileActor::AMissileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	MisslieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MisslieMesh->SetupAttachment(RootComp);
	MisslieMesh->SetRelativeRotation(FRotator(0, -90, -90));

	ForwardComp = CreateDefaultSubobject<USceneComponent>(TEXT("Forward"));
	ForwardComp->SetupAttachment(MisslieMesh);
	ForwardComp->SetRelativeLocation(FVector(0, 370, 0));

	BackComp = CreateDefaultSubobject<USceneComponent>(TEXT("Back"));
	BackComp->SetupAttachment(MisslieMesh);
	BackComp->SetRelativeLocation(FVector(0, -300, 0));

	CameraComp = CreateDefaultSubobject<USceneComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(RootComp);

	MisslieCaptureComponent =
		CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	MisslieCaptureComponent->SetupAttachment(CameraComp);

	CameraComp->SetRelativeLocation(FVector(-800.f, 0.f, 270.f));
	CameraComp->SetRelativeRotation(FRotator(-20.f, 0, 0));

	MisslieCaptureComponent->PostProcessSettings.bOverride_ColorSaturation = true;
	MisslieCaptureComponent->PostProcessSettings.ColorSaturation.W = 0.f; // 0 = 완전 흑백

	// 필요하면 Intensity도 줄일 수 있음
	MisslieCaptureComponent->PostProcessSettings.bOverride_ColorContrast = true;
	MisslieCaptureComponent->PostProcessSettings.ColorContrast = FVector4(1, 1, 1, 1);
}

void AMissileActor::BeginPlay()
{
	Super::BeginPlay();

	TargetYaw = -90;
	TargetRoll = -90;
	TargetPitch = 0;

	// Raw도 초기화 (중기유도 시작 시 급점프 방지)
	RawYaw = TargetYaw;
	RawRoll = TargetRoll;

	MisslieMesh->SetRelativeRotation(FRotator(TargetPitch, TargetYaw, TargetRoll));
	MisslieCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	if (MisslieCaptureComponent)
	{
		auto& PPS = MisslieCaptureComponent->PostProcessSettings;

		PPS.bOverride_ColorSaturation = true;
		PPS.ColorSaturation = FVector4(0.f, 0.f, 0.f, 0.f); // 완전 흑백
	}
	canspawn = false;
}

void AMissileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// =========================
	// 1) Capture
	// =========================
	static float Accum = 0.f;
	Accum += DeltaTime;
	if (Accum > 0.01f)
	{
		if (MisslieCaptureComponent)
			MisslieCaptureComponent->CaptureSceneDeferred();
		Accum = 0.01f;
	}

	if (mslstate == 3) return;

	// =========================
	// 2) Launch phase
	// =========================
	if (bIsLaunching)
	{
		LaunchTime += DeltaTime;

		float ZProgress = FMath::Clamp(LaunchTime / 4.f, 0.f, 1.f);
		float NewZ = InitialZ + (LaunchIncrease * ZProgress);

		FVector Loc = GetActorLocation();
		Loc.Z = NewZ;
		SetActorLocation(Loc);

		// 마지막 2초 Roll/Yaw 보정
		if (LaunchTime >= 2.f)
		{
			float RollAlpha = (LaunchTime - 2.f) / 2.f; // 0 → 1 (2~4초)
			RollAlpha = FMath::Clamp(RollAlpha, 0.f, 1.f);

			float NewRoll = FMath::Lerp(InitialRoll, LaunchTargetRoll, RollAlpha);
			TargetRoll = NewRoll;

			float YawAlpha = (LaunchTime - 2.f) / 2.f; // 0 → 1 (2~4초)
			YawAlpha = FMath::Clamp(YawAlpha, 0.f, 1.f);

			float NewYaw = FMath::Lerp(InitialYaw, LaunchTargetYaw, YawAlpha);
			TargetYaw = NewYaw;
			UE_LOG(LogTemp, Error, TEXT("Invalid Yaw=%f"), NewYaw);
		}

		if (LaunchTime >= 4.f)
		{
			bIsLaunching = false;
			TargetRoll = LaunchTargetRoll;

			// Launch 종료 직후 Raw를 Target으로 맞춰서
			// 중기유도 시작 시 튐 없게 함
			RawYaw = TargetYaw;
			RawRoll = TargetRoll;
		}
	}

	// =========================
	// 3) Terminal phase
	// =========================
	if (bIsTerminal)
	{
		TerminalStartTime += DeltaTime;

		if (TerminalStartTime <= 1.5f)
		{
			float Alpha = FMath::Clamp(TerminalStartTime / 1.5f, 0.f, 1.f);
			TargetYaw = FMath::Lerp(InitialYaw, TerminalTargetYaw, Alpha);
		}

		if (TerminalStartTime >= 1.5f)
		{
			bIsTerminal = false;
			TargetYaw = TerminalTargetYaw;

			RawYaw = TargetYaw; // terminal 끝나고도 튐 방지
			if (!canspawn) canspawn = true;
		}
	}

	// =========================
	// 4) Midcourse UDP smoothing
	//    - Launch/Terminal 중엔 절대 건드리지 않음
	// =========================
	if (!bIsLaunching && !bIsTerminal && mslstate == 1)
	{
		TargetYaw = FMath::FInterpTo(TargetYaw, RawYaw, DeltaTime, UdpInterpSpeed);
		TargetRoll = FMath::FInterpTo(TargetRoll, RawRoll, DeltaTime, UdpInterpSpeed);
	}

	// =========================
	// 5) Apply final rotation (항상 마지막)
	// =========================

	FRotator NewRot(TargetPitch, TargetYaw, TargetRoll);
	MisslieMesh->SetRelativeRotation(NewRot);

	// (종말유도 타겟 생성/이동 쪽은 너가 이어서 넣으면 됨)
	if (TargetActor && ForwardComp && MisslieMesh)
	{
		// 🔥 Abort 모션 진행 중이면 1초 안에 거리 0으로 줄이기
		if (bAbortMotion)
		{
			AbortStartTime += DeltaTime;

			float Alpha = FMath::Clamp(AbortStartTime / AbortDuration, 0.f, 1.f);
			UE_LOG(LogTemp, Error, TEXT("Alpha=%f"), Alpha);
			// SmoothDistance를 0까지 선형 보간
			SmoothDistance = FMath::Lerp(AbortInitialDistance, 0.f, Alpha);

			if (Alpha >= 1.f)
			{
				bAbortMotion = false;
				SmoothDistance = 0.f; // 확실히 0
				UE_LOG(LogTemp, Error, TEXT("Alphaend=%f"), Alpha);
			}
		}
		else
		{
			// 기존 중기유도 거리 보간
			SmoothDistance = FMath::FInterpTo(SmoothDistance, RawDistance, DeltaTime, DistanceInterpSpeed);
		}

		// 위치 적용
		FVector ForwardVector =
			(ForwardComp->GetComponentLocation() - MisslieMesh->GetComponentLocation()).GetSafeNormal();

		FVector TargetLoc =
			ForwardComp->GetComponentLocation() + ForwardVector * SmoothDistance;

		FRotator TargetRot = FRotator(0, RawTargetYaw - 180, 0);

		TargetActor->SetActorLocation(TargetLoc);
		TargetActor->SetActorRotation(TargetRot);
		if (TargetActor->TargetMesh)
		{
			FVector BackPos = FVector(1, 0, 0) * (SmoothDistance * 0.2f);
			TargetActor->TargetMesh->SetRelativeLocation(BackPos);
		}
		if (SmoothDistance==0.f) {
			mslstate = 3;
			if (ExplosionFX && ForwardComp)
			{
				FVector SpawnLoc = ForwardComp->GetComponentLocation();
				FRotator SpawnRot = ForwardComp->GetComponentRotation();
				FVector SpawnScale(5.f, 5.f, 5.f);

				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ExplosionFX,
					SpawnLoc,
					SpawnRot,
					SpawnScale,
					true
				);
			}
			TargetActor->Destroy();
			NoSignalChange();
		}
	}
}

float ShortAngle(float From, float To)
{
	float Delta = FMath::Fmod(To - From + 540.f, 360.f) - 180.f;
	return From + Delta;
}


//////////////////////////////////////////////////////////////////////////
// UDP Attitude
//////////////////////////////////////////////////////////////////////////

void AMissileActor::ApplyAttitude(float InRoll, float InYaw)
{
	// 중기유도(state 1)에서만 UDP 반영
	if (mslstate != 1) return; // 중기유도만 적용

	RawRoll = InRoll;

	float NewRawYaw = InYaw - 90.0f;

	// 현재 기준으로 최단 경로로 RawYaw 조정
	float CurrentYaw = MisslieMesh->GetRelativeRotation().Yaw;
	RawYaw = ShortAngle(CurrentYaw, NewRawYaw);
}

//////////////////////////////////////////////////////////////////////////
// RenderTarget
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
// Launch
//////////////////////////////////////////////////////////////////////////

void AMissileActor::LaunchMissile(float inYaw)
{
	bIsLaunching = true;
	LaunchTime = 0.f;

	InitialZ = GetActorLocation().Z;

	FRotator initialRot = MisslieMesh->GetRelativeRotation();
	InitialRoll = initialRot.Roll;
	InitialYaw = initialRot.Yaw;
	// 목표 Yaw (UDP로 받은 것)
	float smoothTargetYaw = inYaw - 90.0f;

	// 최단 회전 경로 목표값 계산!!
	LaunchTargetYaw = ShortAngle(InitialYaw, smoothTargetYaw);
	//LaunchTargetYaw = inYaw - 90.f;

	mslstate = 1; // launch 이후 중기유도 진입한다고 가정(네 구조 유지)

	UE_LOG(LogTemp, Warning, TEXT("[Missile %d] FireFX Spawn"), MissileID);

	if (FireFX && BackComp)
	{
		UParticleSystemComponent* FirePSC =
			UGameplayStatics::SpawnEmitterAttached(
				FireFX,
				BackComp,
				NAME_None,
				FVector::ZeroVector,
				FRotator(0, 0, -90),
				FVector(2.f, 2.f, 2.f),
				EAttachLocation::KeepRelativeOffset,
				true
			);

		if (FirePSC)
		{
			FirePSC->SetRelativeLocation(FVector::ZeroVector);
			FirePSC->CustomTimeDilation = 1.0f;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Terminal
//////////////////////////////////////////////////////////////////////////

void AMissileActor::TerminalChange(float inYaw)
{
	bIsTerminal = true;
	TerminalStartTime = 0.f;

	FRotator initialRot = MisslieMesh->GetRelativeRotation();
	InitialYaw = initialRot.Yaw;

	TerminalTargetYaw = inYaw;
	mslstate = 2;
}

//////////////////////////////////////////////////////////////////////////
// Target
//////////////////////////////////////////////////////////////////////////

void AMissileActor::UpdateTarget(float Distance, float yaw)
{
	// UDP로 받은 원본 값 저장
	RawDistance = Distance*5;
	RawTargetYaw = yaw;

	// 타겟이 아직 없으면 생성만 한다
	if (!ForwardComp || !MisslieMesh) return;
	if (!canspawn) return;

	// 최초 생성 시는 그대로 생성해야 함
	if (TargetActor == nullptr)
	{
		FVector ForwardVector =
			(ForwardComp->GetComponentLocation() - MisslieMesh->GetComponentLocation()).GetSafeNormal();

		FVector TargetLoc =
			ForwardComp->GetComponentLocation() + ForwardVector * RawDistance;

		FRotator TargetRot = FRotator(0, RawTargetYaw - 180, 0);

		if (!TargetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("TargetClass is NOT set!"));
			return;
		}

		TargetActor = GetWorld()->SpawnActor<ATarget>(
			TargetClass,
			TargetLoc,
			TargetRot
		);
		if (TargetActor->TargetMesh)
		{
			FVector BackPos = FVector(1,0,0) * (RawDistance * 0.2f);
			TargetActor->TargetMesh->SetRelativeLocation(BackPos);
		}
		if (!TargetActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Target spawn failed!"));
			return;
		}
		SmoothDistance = RawDistance;
		UE_LOG(LogTemp, Warning, TEXT("★ Target created at %d"), MissileID);
	}
}

//////////////////////////////////////////////////////////////////////////
// UI Signal / Abort
//////////////////////////////////////////////////////////////////////////

void AMissileActor::NoSignalChange()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AMonitorPlayerController* MPC = Cast<AMonitorPlayerController>(PC);
	if (!MPC) return;

	UWBP_MissileMonitorBase* WBP = MPC->GetMonitorWidget();
	if (!WBP) return;

	WBP->NosignalRun(MissileID);
}

void AMissileActor::AbortChange()
{
	if (TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("bAbortMotion"));
		bAbortMotion = true;
		AbortStartTime = 0.f;
		AbortInitialDistance = SmoothDistance; // 현 시점 거리 저장
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("bAbortMotion fail"));
		if (ExplosionFX && ForwardComp)
		{
			FVector SpawnLoc = ForwardComp->GetComponentLocation();
			FRotator SpawnRot = ForwardComp->GetComponentRotation();
			FVector SpawnScale(5.f, 5.f, 5.f);

			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ExplosionFX,
				SpawnLoc,
				SpawnRot,
				SpawnScale,
				true
			);
		}
		NoSignalChange();
	}
	
}

void AMissileActor::TelemetryChange(uint8 TelemetryByte)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AMonitorPlayerController* MPC = Cast<AMonitorPlayerController>(PC);
	if (!MPC) return;

	UWBP_MissileMonitorBase* WBP = MPC->GetMonitorWidget();
	if (!WBP) return;

	WBP->UpdateTelemetry(MissileID,TelemetryByte);
}