// Copyright Sam Hill


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	TargetingSphere = CreateDefaultSubobject<USphereComponent>("TargetingSphere");
	SetRootComponent(TargetingSphere);
	TargetingSphere->SetCollisionResponseToChannels(ECR_Ignore);
	TargetingSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
	
}

void AMagicCircle::SetTargetingRadius(float Radius)
{
	TargetingSphere->SetSphereRadius(Radius);
	MagicCircleDecal->DecalSize = FVector(Radius);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();

	TargetingSphere->OnComponentBeginOverlap.AddDynamic(this, &AMagicCircle::OnTargetingBeginOverlap);
	TargetingSphere->OnComponentEndOverlap.AddDynamic(this, &AMagicCircle::OnTargetingEndOverlap);
	
}

void AMagicCircle::OnTargetingBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UHighlightInterface>() && OtherActor->Implements<UEnemyInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(OtherActor);
	}
}

void AMagicCircle::OnTargetingEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UHighlightInterface>() && OtherActor->Implements<UEnemyInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(OtherActor);
	}
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

