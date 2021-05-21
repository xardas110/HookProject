// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueenSquidBase.h"
#include "AIQueenSquidTentacle.h"
#include "../Character/PlayerBase.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SphereComponent.h"
#include "../Character/Actors/HookProjectile.h"
#include "HookGame/Helpers/Macros.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

//#define _DEBUG_QUEENSQUIDBASE

AAIQueenSquidBase::AAIQueenSquidBase()
{
	DamageHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Hitbox"));
	DamageHitbox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, (TEXT("GrapplePoint")));
	LaserBeamStart = CreateDefaultSubobject<USceneComponent>(TEXT("Laser Beam Start"));
	//LaserBeamStart->AttachToComponent(GetMesh(), F, (TEXT("LaserSocket")));
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserParticle"));
	ParticleSystem->SetupAttachment(LaserBeamStart);
}

void AAIQueenSquidBase::BeginPlay()
{
	Super::BeginPlay();

	GrappleTracer->ComponentExtent->SetBoxExtent({ 500.f, 500.f, 500.f });
	
	SetupPlatforms();
	SetStartLocationRotation();
	SetStartValues();	
}

void AAIQueenSquidBase::SpawnPlatforms()
{
	ReturnIfNull(GetWorld());
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

void AAIQueenSquidBase::SetupPlatforms()
{
	for (int i = 0; i < NumberOfPlatforms; ++i)
	{
		SpawnPlatforms();
		SpawnedPlatforms++;
		PlatformSpawnPoint += PlatformSpawnOffset;
}

	for (AAIQueenSquidTentacle* Element : SpawnedTentacles)
	{
		Element->SetActorScale3D(GetActorScale3D() / 2);
	}
}

void AAIQueenSquidBase::SetStartLocationRotation()
{
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
}

void AAIQueenSquidBase::SetStartValues()
{
	DamageHitbox->OnComponentBeginOverlap.AddDynamic(this, &AAIQueenSquidBase::OnBeginOverlap);
	DamageHitbox->SetGenerateOverlapEvents(false);
	bIsAttacking = true;
	InspectRotationStart += GetActorRotation();
	ParticleSystem->SetVisibility(false);
}


void AAIQueenSquidBase::AttemptAttack()
{
#ifdef _DEBUG_QUEENSQUIDBASE
	UE_LOG(LogTemp, Warning, TEXT("Attempting Attack"));
#endif
	const auto PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	ReturnIfNull(PlayerPawn);
	ReturnIfNull(TentacleBP);
	AAIQueenSquidTentacle* ClosestTentacleToPlayer{ nullptr };
	float ClosestDistanceToPlayer = 100000.f;
	
	for (AAIQueenSquidTentacle* Element : SpawnedTentacles)
	{
		Element->bHasAttacked = false;
		if (!ClosestTentacleToPlayer) {
			ClosestTentacleToPlayer = Element;
			ClosestDistanceToPlayer = FVector::Dist(Element->GetActorLocation(), PlayerPawn->GetActorLocation());
		}
		else if(ClosestDistanceToPlayer>FVector::Dist(Element->GetActorLocation(), PlayerPawn->GetActorLocation()))
		{
			ClosestTentacleToPlayer = Element;
			ClosestDistanceToPlayer = FVector::Dist(Element->GetActorLocation(), PlayerPawn->GetActorLocation());
		}
	}
	ClosestTentacleToPlayer->Attack();
	NumberOfAttacks++;
}

void AAIQueenSquidBase::InspectPlatforms(float DeltaTime)
{
	bIsVulnerable = true;
	bIsAttacking = false;
	bIsInspecting = true;
}

void AAIQueenSquidBase::ShootLaser()
{
	if(!bFireLaser)
		return;
	
	FHitResult SphereResult;
	TArray<AActor*>IgnoreActors;
	FVector LaserBegin = LaserBeamStart->GetComponentLocation();
	FVector LaserEnd = GetActorLocation()+GetActorForwardVector() * LaserRange;
	auto bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), LaserBegin, LaserEnd, LaserRadius, ETraceTypeQuery::TraceTypeQuery2, true, IgnoreActors, EDrawDebugTrace::ForOneFrame, SphereResult,
		true, FLinearColor::Green);
	FVector LaserDirection = LaserEnd - LaserBegin;
	LaserDirection /= LaserDirection.Size();
	FRotator LaserRotation = LaserDirection.Rotation();
	LaserBeamStart->SetWorldRotation(LaserRotation);
	if (!bHit)
		return;
	
	ReturnIfNull(SphereResult.GetActor());
	//UE_LOG(LogTemp, Warning, TEXT(" Laser hit %s "), *SphereResult.Actor->GetName());
		if(SphereResult.GetActor()->IsA(APlayerBase::StaticClass()))
		{
			if (bPlayerHitByLaser)
				return;
			FDamageEvent QueenDamageEvent;
			Cast<APlayerBase>(SphereResult.GetActor())->TakeDamage(LaserDamage, QueenDamageEvent, Controller, this);
			bPlayerHitByLaser = true;
		}
		else
		{
			IgnoreActors.Add(SphereResult.GetActor());
		}
	
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
			//LaserHitbox->SetGenerateOverlapEvents(false);
			DamageHitbox->SetGenerateOverlapEvents(false);
			//LaserBeam->SetVisibility(false);
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
		bPlayerHitByLaser = true;
#ifdef _DEBUG_QUEENSQUIDBASE
		UE_LOG(LogTemp, Warning, TEXT(" Laser Overlapped player"));
#endif
		const auto Player = GetWorld()->GetFirstPlayerController();
		ReturnIfNull(Player);
		FDamageEvent DamageEvent;
		Player->GetPawn()->TakeDamage(LaserDamage, DamageEvent, Controller, this);
			
	}
}

void AAIQueenSquidBase::BeginLaser()
{
	bFireLaser = true;
	ParticleSystem->SetVisibility(true);
}

void AAIQueenSquidBase::EndLaser()
{
	bFireLaser = false;
	ParticleSystem->SetVisibility(false);
}

void AAIQueenSquidBase::AttackCheck(float DeltaSeconds)
{
	TimeSinceAttack += DeltaSeconds;
	if (bIsAttacking)
	{
		if (TimeSinceAttack > TimeBetweenAttacks)
		{
			if (HowManyAttacks != 3)
			{
				bPlayerHitByLaser = false;
				AttemptAttack();
				HowManyAttacks++;
				TimeSinceAttack = 0.f;
				ParticleSystem->SetVisibility(false);
			}
			else
			{
				InspectPlatforms(DeltaSeconds);
				TimeSinceAttack = 0.f;
				HowManyAttacks = 0;
				TimeInspected = 0.f;
			}

		}

	}
	else
	{
		MoveQueen(DeltaSeconds);
	}
}

void AAIQueenSquidBase::InspectCheck(float DeltaSeconds)
{
	if (bIsInspecting)
	{
		ShootLaser();
		RotateQueen(DeltaSeconds);
	}
	else
	{
		SetActorRotation(OriginalRotation);
		DamageHitbox->SetGenerateOverlapEvents(false);
		bHasBegunRotating = false;
		bPlayerHitByLaser = false;
	}
}

void AAIQueenSquidBase::HealthCheck()
{
	if (HealthTotal < 1)
	{
		bIsInspecting = false;
		HandleDeath();
	}
}

void AAIQueenSquidBase::DespawnCountdown(float DeltaSeconds)
{
	DeathTimer += DeltaSeconds;
	if (DeathTimer > TimeUntilDespawn)
		Destroy();
}

void AAIQueenSquidBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GrappleTracer->ComponentExtent->SetWorldLocation(LaserBeamStart->GetComponentLocation());
	GrappleTracer->GrapplePoint->SetWorldLocation(LaserBeamStart->GetComponentLocation());
	
	FVector LaserLocation=GetMesh()->GetSocketLocation(TEXT("LaserSocket"));
	LaserBeamStart->SetWorldLocation(LaserLocation);

	AttackCheck(DeltaSeconds);
	InspectCheck(DeltaSeconds);
	HealthCheck();
	if(bIsDead)
		DespawnCountdown(DeltaSeconds);
	
	//RotateQueen(DeltaSeconds);
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
		DamageHitbox->SetGenerateOverlapEvents(true);
		
		if (!bHasBegunRotating) {
			RotationDirectionRandomizer();
			SetActorRotation(InspectRotationStart);
			bHasBegunRotating = true;
		}
		if (GetActorRotation() != InspectRotationEnd) {
			AddActorWorldRotation(InspectRotationOffset*DeltaSeconds);
		}
		else
		{
			bHasBegunRotating = false;
		}
	}
	
}

void AAIQueenSquidBase::RotationDirectionRandomizer() // Decides if the rotation should be inverted or not
{
	int32 Outcome = FMath::RandRange(0, 1);
	if (Outcome == 0)
		return;
	
	else
	{
		InspectRotationStart = InspectRotationStart * -1;
		InspectRotationOffset = InspectRotationOffset * -1;
		InspectRotationEnd = InspectRotationEnd * -1;
		return;
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
				DamageHitbox->SetGenerateOverlapEvents(false);				
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
