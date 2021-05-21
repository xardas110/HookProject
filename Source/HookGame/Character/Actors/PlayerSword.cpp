#include "PlayerSword.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../AI/AIBehaviorBase.h"
#include "HookGame/WorldActors/EnemySpawner.h"
#include "Particles/ParticleSystemComponent.h"


APlayerSword::APlayerSword()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SwordHandleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Sword Handle Location"));
	SwordHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("Sword Hitbox"));
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh"));
	SwordParticleSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Sword Particle Spawn Location"));
	SwordParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>("Sword Particle Trail");
	
	RootComponent = SwordHandleLocation;

	SwordHitbox->SetupAttachment(SwordHandleLocation);
	SwordMesh->SetupAttachment(SwordHitbox);
	SwordParticleSpawnLocation->SetupAttachment(SwordHitbox);
	SwordParticleTrail->SetupAttachment(SwordParticleSpawnLocation);
	
	SwordHitbox->OnComponentBeginOverlap.AddDynamic(this, &APlayerSword::OnBeginOverlap);
}

void APlayerSword::BeginPlay()
{
	Super::BeginPlay();
	SwordHitbox->SetGenerateOverlapEvents(false);
	SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordParticleTrail->Deactivate();

	SwordLength = SwordHitbox->GetScaledBoxExtent().X;
}

void APlayerSword::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;
		
	if (HitRegisteredComponentsInCurrentSwing.Contains(OtherActor))		
		return;

	HitRegisteredComponentsInCurrentSwing.Push(OtherActor);
	
	const auto PlayerController = UGameplayStatics::GetPlayerControllerFromID(GetWorld(), 0);
	ReturnIfNull(PlayerController);

	const FDamageEvent DMGEvent;
	OtherActor->TakeDamage(Damage, DMGEvent, PlayerController, this);
	
	if (const auto Ai = Cast<AAIBehaviorBase>(OtherActor))
	{
		if (!Ai->bIsDead)
			if (SliceSound->IsPlayable())
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), SliceSound, GetActorLocation());
	}
}

// Called every frame
void APlayerSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UBoxComponent* APlayerSword::GetCollisionComponent() const
{
	return SwordHitbox;
}

void APlayerSword::StartSwing()
{
	const auto CurrentBoxExtent = SwordHitbox->GetScaledBoxExtent();
	SwordHitbox->SetBoxExtent({ SwordLength, CurrentBoxExtent.Y , CurrentBoxExtent.Z });
	
	SwordParticleTrail->Activate();
	HitRegisteredComponentsInCurrentSwing.Empty();
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SwordHitbox->SetGenerateOverlapEvents(true);

	if (AttackSound->IsPlayable())
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
}

void APlayerSword::EndSwing()
{
	SwordParticleTrail->Deactivate();
	SwordHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordHitbox->SetGenerateOverlapEvents(false);
}

void APlayerSword::ComboSwing()
{
	HitRegisteredComponentsInCurrentSwing.Empty();
	
	if (AttackSound->IsPlayable())
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
}

