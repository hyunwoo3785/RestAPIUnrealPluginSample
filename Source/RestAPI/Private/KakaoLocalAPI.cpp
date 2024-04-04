#include "KakaoLocalAPI.h"
#include "Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Online/HTTP/Public/HttpModule.h"
#include "Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GenericPlatform/GenericPlatformHttp.h"


AKakaoLocalAPIActor::AKakaoLocalAPIActor()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SceneComponent->SetupAttachment(RootComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision Component"));
	BoxCollisionComponent->SetupAttachment(SceneComponent);
	

	// Create a follow camera
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	MeshComponent->SetupAttachment(SceneComponent);

	

	if (TObjectPtr<UStaticMesh> MeshObject = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), this->GetOuter(), TEXT("/Script/Engine.StaticMesh'/RestAPI/Meshes/SM_ChamferCube.SM_ChamferCube'"))))
	{
		MeshComponent->SetStaticMesh(MeshObject);
	}
	

	if (TObjectPtr<UMaterialInterface> MaterialObject = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), this->GetOuter(), TEXT("/Script/Engine.MaterialInstanceConstant'/RestAPI/Materials/MI_Solid_Blue.MI_Solid_Blue'"))))
	{
		MeshComponent->SetMaterial(0, MaterialObject);
	}
	
}


void AKakaoLocalAPIActor::RequestSearchingAddress(const FString& Address)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(FString::Printf(TEXT(ADDRESS_SEARCH_URL), *FGenericPlatformHttp::UrlEncode(Address)));
	Request->SetVerb(TEXT("GET"));

	Request->SetHeader(TEXT("Authorization"), FString(TEXT("KakaoAK ")) + KAKAO_REST_API_KEY);
	Request->OnProcessRequestComplete().BindUObject(this, &AKakaoLocalAPIActor::OnSearchingAddressReceived);
	Request->ProcessRequest();
}

void AKakaoLocalAPIActor::RequestSearchingKeyword(const FString& Keyword)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(FString::Printf(TEXT(KEYWORD_SEARCH_URL), *FGenericPlatformHttp::UrlEncode(Keyword)));
	Request->SetVerb(TEXT("GET"));

	Request->SetHeader(TEXT("Authorization"), FString(TEXT("KakaoAK ")) + KAKAO_REST_API_KEY);
	Request->OnProcessRequestComplete().BindUObject(this, &AKakaoLocalAPIActor::OnSearchingKeywordReceived);
	Request->ProcessRequest();
}

void AKakaoLocalAPIActor::RequestSearchingLocation(const FString& InLatitude, const FString& InLongitude)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetURL(FString::Printf(TEXT(PLACES_SEARCH_URL), *InLatitude, *InLongitude));
	Request->SetVerb(TEXT("GET"));

	Request->SetHeader(TEXT("Authorization"), FString(TEXT("KakaoAK ")) + KAKAO_REST_API_KEY);
	Request->OnProcessRequestComplete().BindUObject(this, &AKakaoLocalAPIActor::OnSearchingLocationReceived);
	Request->ProcessRequest();
}


void AKakaoLocalAPIActor::OnSearchingAddressReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	ensureMsgf(false, TEXT("Not implemented"));
}

void AKakaoLocalAPIActor::OnSearchingKeywordReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	ensureMsgf(false, TEXT("Not implemented"));
}

void AKakaoLocalAPIActor::OnSearchingLocationReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ensureMsgf(false, TEXT("Searching location request failed."));
	}

	FString Result = Response->GetContentAsString();

	if (Response->GetResponseCode() != 200)
	{
		ensureMsgf(false, TEXT("Responsed code : %d"), Response->GetResponseCode());
	}
	Places.Empty();
	
	TSharedPtr<FJsonObject> JsonObject;

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TArray<TSharedPtr<FJsonValue>> documents = JsonObject->GetArrayField("documents");

		for (TSharedPtr<FJsonValue> Value : documents)
		{
			const TSharedPtr<FJsonObject>& Obj = Value->AsObject();
			FKakaoPlace Place;
			Place.AddressName			= Obj->GetStringField("adress_name");
			Place.CategoryGroupCode		= Obj->GetStringField("category_group_code");
			Place.CategoryGroupName		= Obj->GetStringField("category_group_name");
			Place.CategoryName			= Obj->GetStringField("category_name");
			Place.Distance				= Obj->GetStringField("distance");
			Place.Id					= Obj->GetStringField("id");
			Place.Latitude				= Obj->GetStringField("y");
			Place.Longitude				= Obj->GetStringField("x");
			Place.PhoneNumber			= Obj->GetStringField("phone");
			Place.PlaceName				= Obj->GetStringField("place_name");
			Place.PlaceUrl				= Obj->GetStringField("place_url");
			Place.RoadAddressName		= Obj->GetStringField("road_address_name");

			Places.Add(Place);
		}
		LocationSearchingPlacesDelegate.Broadcast(Places);
	}
}

void AKakaoLocalAPIActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (BoxCollisionComponent)
	{
		BoxCollisionComponent->SetBoxExtent(FVector(BOX_COLLISION_EXTENT * CollsionBoxScale));
		BoxCollisionComponent->SetRelativeLocation(FVector(0, 0, BOX_COLLISION_EXTENT * CollsionBoxScale));
	}
	if (MeshComponent)
	{
		MeshComponent->SetRelativeScale3D(FVector(1.f * CollsionBoxScale, 1.f * CollsionBoxScale, BOX_TO_FLAT_SCALER));
		MeshComponent->SetRelativeLocation(FVector(0, 0, BOX_TO_FLAT_SCALER * 0.5f));
	}
}

void AKakaoLocalAPIActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	RequestSearchingLocation(Latitude, Longitude);
}
