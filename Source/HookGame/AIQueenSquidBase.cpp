// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueenSquidBase.h"
#include "AIQueenSquidTentacle.h"
#include "PlayerBase.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SphereComponent.h"
#include "HookProjectile.h"

#define _DEBUG_QUEENSQUIDBASE

AAIQueenSquidBase::AAIQueenSquidBase()
{
	DamageHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	DamageHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("GrapplePoint")));
	LaserBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser Beam"));
	LaserBeam->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("LaserSocket")));
	LaserHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Laser Hitbox"));
	LaserHitbox->SetupAttachment(LaserBeam);
}

void AAIQueenSquidBase::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < NumberOfPlatforms; ++i)
	{
		SpawnPlatforms();
		SpawnedPlatforms++;
		PlatformSpawnPoint += PlatformSpawnOffset;
	}

	for (AAIQueenSquidTentacle* Element : SpawnedTentacles)
	{
		Element->SetActorScale3D(GetActorScale3D()/2);
	}
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
	bIsAttacking = true;
	DamageHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAIQueenSquidBase::OnBeginOverlap);
	DamageHitbox->SetGenerateOverlapEvents(false);
	LaserHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAIQueenSquidBase::OnLaserOverlap);
	LaserHitbox->SetGenerateOverlapEvents(false);
	LaserBeam->SetVisibility(false);
}

void AAIQueenSquidBase::SpawnPlatforms() // Currently not in use, might delete.
{
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		UWorld* world = GetWorld();
		if (TentacleBP) {
			AAIQueenSquidTentacle* Tentacle;
				Tentacle = world->SpawnActor<AAIQueenSquidTentacle>(TentacleBP, GetActorLocation() + PlatformSpawnPoint, GetActorRotation(), SpawnParams);
			SpawnedTentacles.Add(Tentacle);
		}
		else
#ifdef _DEBUG_QUEENSQUIDBASE
			UE_LOG(LogTemp, Error, TEXT(" Queen squidbird has no set platform BP! "));
#endif
		return;
	}
}


void AAIQueenSquidBase::AttemptAttack()
{
#ifdef _DEBUG_QUEENSQUIDBASE
	UE_LOG(LogTemp, Warning, TEXT("Attempting Attack"));
#endif
	if (TentacleBP) {
		for (AAIQueenSquidTentacle* Element : SpawnedTentacles)
		{
			Element->bHasAttacked = false;
		}
		int NumberOfAttacks = 0;
		for (int i = 0; i < NumberOfPlatforms; ++i)
		{
			if (FMath::RandBool() == true && NumberOfAttacks < NumberOfPlatforms - 2 && SpawnedTentacles[i]->bHasAttacked == false)
			{
				SpawnedTentacles[i]->Attack();
				NumberOfAttacks++;
				SpawnedTentacles[i]->bHasAttacked = true;
			}
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT(" SQUIDQUEEN HAS NO SET PLATFORM BLUEPRINT!"));


}

void AAIQueenSquidBase::InspectPlatforms(float DeltaTime)
{
	bIsVulnerable = true;
	bIsAttacking = false;
	bIsInspecting = true;
	bHasBegunRotating = false;
}

void AAIQueenSquidBase::Attack()
{
#ifdef _DEBUG_QUEENSQUIDBASE
	UE_LOG(LogTemp,Warning,TEXT("SQUIDQUEEN ATTACK CALLED"))
#endif
	bIsAttacking = true;
	bIsVulnerable = false;
	bTookDamage = false;
	bIsInspecting = false;
}

void AAIQueenSquidBase::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AHookProjectile::StaticClass()))
	{
		const auto Player = GetWorld()->GetFirstPlayerController();
		if (Player) {
			FDamageEvent DamageEvent;
			TakeDamage(25, DamageEvent, Controller, Player);
			bIsVulnerable = false;
			bTookDamage = true;
			TimeInspected = 0.f;
			bIsInspecting = false;
			LaserHitbox->SetGenerateOverlapEvents(false);
			DamageHitbox->SetGenerateOverlapEvents(false);
			LaserBeam->SetVisibility(false);
			SetActorRotation(OriginalRotation);
			HowManyAttacks = 0;
		}
	}
}

void AAIQueenSquidBase::OnLaserOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
#ifdef _DEBUG_QUEENSQUIDBASE
	UE_LOG(LogTemp, Warning, TEXT(" Laser overlapped something "));
#endif
	if(OtherActor->IsA(APlayerBase::StaticClass()))
	{
#ifdef _DEBUG_QUEENSQUIDBASE
		UE_LOG(LogTemp, Warning, TEXT(" Laser Overlapped player"));
#endif
		const auto Player = GetWorld()->GetFirstPlayerController();
		if(Player)
		{
			FDamageEvent DamageEvent;
			Player->GetPawn()->TakeDamage(LaserDamage, DamageEvent, Controller, this);
		}
	}
}

void AAIQueenSquidBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimeSinceAttack += DeltaSeconds;
	if(bIsAttacking)
	{
		
		if(TimeSinceAttack>TimeBetweenAttacks)
		{
			if (HowManyAttacks != 3)
			{
				AttemptAttack();
				HowManyAttacks++;
				TimeSinceAttack = 0.f;
			}
			else
			{
				TimeSinceAttack = 0.f;
				HowManyAttacks = 0;
				InspectPlatforms(DeltaSeconds);
				TimeInspected = 0.f;
			}
			
		}
		
	}
	else
	{
		MoveQueen(DeltaSeconds);
	}
	if(HealthTotal<1)
	{
		bIsInspecting = false;
		HandleDeath();
	}
	if(bIsDead)
	{
		DeathTimer += DeltaSeconds;
		if (DeathTimer > TimeUntilDespawn)
			Destroy();
	}
	RotateQueen(DeltaSeconds);
}

void AAIQueenSquidBase::HandleDeath()
{
	bIsVulnerable = false;
	bIsAttacking = false;
	for (AAIQueenSquidTentacle* Element : SpawnedTentacles)
	{
		Element->bIsDead=true;
	}
	bIsDead = true;
}

void AAIQueenSquidBase::RotateQueen(float DeltaSeconds)
{
	if (bIsInspecting) {

		UE_LOG(LogTemp, Warning, TEXT("ROTATE QUEEN"));
		
		DamageHitbox->SetGenerateOverlapEvents(true);
		LaserHitbox->SetGenerateOverlapEvents(true);
		LaserBeam->SetVisibility(true);
		if (!bHasBegunRotating) {
			SetActorRotation(InspectRotationStart);
			bHasBegunRotating = true;
		}
		if (GetActorRotation() != InspectRotationEnd) {
			AddActorWorldRotation(InspectRotationOffset);
		}
	}
	
	else
	{
		SetActorRotation(OriginalRotation);
		LaserHitbox->SetGenerateOverlapEvents(false);
		DamageHitbox->SetGenerateOverlapEvents(false);
		LaserBeam->SetVisibility(false);
	}
}

void AAIQueenSquidBase::MoveQueen(float DeltaSeconds)
{
	FVector Location = GetActorLocation();
	TimeInspected += DeltaSeconds;
	if (!bTookDamage) {
		if (TimeInspected < 1.f)
		{
			Location.Z = Location.Z - (InspectLowering * DeltaSeconds);
			SetActorLocation(Location);
		}
		else if (TimeInspected > 4.f)
		{
			if (Location != OriginalLocation) {
				
				
				Location.Z = Location.Z + (InspectLowering * DeltaSeconds);
				SetActorLocation(Location);
				SetActorRotation(OriginalRotation);
				LaserHitbox->SetGenerateOverlapEvents(false);
				DamageHitbox->SetGenerateOverlapEvents(false);
				LaserBeam->SetVisibility(false);
				
			}
		}
	}
	else if(bTookDamage)
	{
		
		if (TimeInspected<1.f) {
			Location.Z = Location.Z + (InspectLowering * DeltaSeconds);
			SetActorLocation(Location);
		}
	}
}
