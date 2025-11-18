// Fill out your copyright notice in the Description page of Project Settings.


#include "MonitorPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "WBP_MissileMonitorBase.h"
#include "MissileManager.h"

void AMonitorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	if (MonitorWidgetClass)
	{
		MonitorWidgetInstance = CreateWidget<UUserWidget>(this, MonitorWidgetClass);
		if (MonitorWidgetInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("[PC] Widget OK : %s"),
				*MonitorWidgetInstance->GetName());

			MonitorWidgetInstance->AddToViewport();
		}
	}
	FActorSpawnParameters Params;
	Params.Owner = this;

	FVector SpawnLoc(0,0,0);

		AMissileManager* NewMissileManager = GetWorld()->SpawnActor<AMissileManager>(
		MissileManagerClass,
		SpawnLoc,
		FRotator::ZeroRotator,
		Params
	);

}
UWBP_MissileMonitorBase* AMonitorPlayerController::GetMonitorWidget() const
{
	return Cast<UWBP_MissileMonitorBase>(MonitorWidgetInstance);
}