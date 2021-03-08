// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectrumVisualizer.h"
#include "TimeSynthComponent.h"

// Sets default values
ASpectrumVisualizer::ASpectrumVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpectrumBarSpacing = 100.f;		// 1m between bars

	TimeSynthComponent = CreateDefaultSubobject<UTimeSynthComponent>("TimeSynthComponent");
	TimeSynthComponent->bEnableSpectralAnalysis = true;

	SetRootComponent(TimeSynthComponent);
	
	// Create spectrum bars
	for (int32 i = 1; i <= 20; ++i)
	{
		// 50, 100, 150, 200, 250, ...
		TimeSynthComponent->FrequenciesToAnalyze.Add(i * 50);

		FString SpectrumBarName = FString::Printf(TEXT("SpectrumBar%dHz"), i * 50);
		UStaticMeshComponent* SpectrumBar = CreateDefaultSubobject<UStaticMeshComponent>(*SpectrumBarName);
		SpectrumBar->SetupAttachment(TimeSynthComponent);
		SpectrumBar->SetStaticMesh(SpectrumBarMesh);
		SpectrumBar->SetMaterial(0, SpectrumBarMaterial);
		SpectrumBar->SetRelativeLocation(FVector(i * SpectrumBarSpacing, 0.f, 0.f));
		SpectrumBars.Add(SpectrumBar);
	}
}

// Called when the game starts or when spawned
void ASpectrumVisualizer::BeginPlay()
{
	Super::BeginPlay();

	Refresh();

	TimeSynthComponent->PlayClip(TimeSynthClip);
}

void ASpectrumVisualizer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Refresh();
}

void ASpectrumVisualizer::Refresh()
{
	for (int32 i = 0; i < 20; ++i)
	{
		if (SpectrumBars.IsValidIndex(i))
		{
			UStaticMeshComponent* SpectrumBar = SpectrumBars[i];
			SpectrumBar->SetStaticMesh(SpectrumBarMesh);
			SpectrumBar->SetMaterial(0, SpectrumBarMaterial);
			SpectrumBar->SetRelativeLocation(FVector((i + 1) * SpectrumBarSpacing, 0.f, 0.f));
		}
	}
}

// Called every frame
void ASpectrumVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Refresh ticks
	for (auto SpecData : TimeSynthComponent->GetSpectralData())
	{
		int Index = SpecData.FrequencyHz / 50.f - 1;
		UStaticMeshComponent* SpectrumBar = SpectrumBars[Index];

		FVector BarScale = SpectrumBar->GetComponentScale();
		BarScale.Z = 1.f + SpecData.Magnitude / 5.f;

		const FVector NewBarScale = FMath::VInterpTo(SpectrumBar->GetComponentScale(),
			BarScale, DeltaTime, 5.f); 
		SpectrumBar->SetWorldScale3D(NewBarScale);
	}
}

