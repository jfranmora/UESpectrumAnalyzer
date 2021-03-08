// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectrumVisualizer.h"
#include "TimeSynthComponent.h"

const float FREQ_BETWEEN_BARS = 50.f;
const int BAR_NUMBER = 20;

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
	for (int32 i = 1; i <= BAR_NUMBER; ++i)
	{
		TimeSynthComponent->FrequenciesToAnalyze.Add(i * FREQ_BETWEEN_BARS);

		FString SpectrumBarName = FString::Printf(TEXT("SpectrumBar%dHz"), i * FREQ_BETWEEN_BARS);
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
	for (int32 i = 0; i < BAR_NUMBER; ++i)
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
		int Index = (SpecData.FrequencyHz / FREQ_BETWEEN_BARS) - 1;
		UStaticMeshComponent* SpectrumBar = SpectrumBars[Index];

		float Amplitude = SpecData.Magnitude / GetNormalizedValue(SpecData.FrequencyHz);
		
		FVector BarScale = SpectrumBar->GetComponentScale();
		BarScale.Z = .1f + 10 * Amplitude;

		const FVector NewBarScale = FMath::VInterpTo(SpectrumBar->GetComponentScale(),
			BarScale, DeltaTime, 5.f); 
		SpectrumBar->SetWorldScale3D(NewBarScale);

		UMaterialInstanceDynamic* Material = SpectrumBar->CreateDynamicMaterialInstance(0);
		Material->SetScalarParameterValue(TEXT("Amplitude"), Amplitude);
		SpectrumBar->SetMaterial(0, Material);
	}
}

// y = 5.220585 + 146.3705*e^(-0.1226524*x)
inline float ASpectrumVisualizer::GetNormalizedValue(float Frequency)
{
	const float a = 5.220585f;
	const float b = 146.f;
	const float c = 0.1226524f / 50.f;
	return a + b*FMath::Exp(-c * Frequency);
}

