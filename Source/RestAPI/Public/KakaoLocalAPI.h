#pragma once

#include "CoreMinimal.h"
#include "Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "KakaoLocalAPI.generated.h"

#define ADDRESS_SEARCH_URL "https://dapi.kakao.com/v2/local/search/address.json?query=%s"
#define KEYWORD_SEARCH_URL "https://dapi.kakao.com/v2/local/search/keyword.json?query=%s"
#define KEYWORD_SEARCH_URL_ADV "https://dapi.kakao.com/v2/local/search/keyword.json?query=%s&y=%s&x=%s&radius=%s" // keyword, y = latitude, x = longitude
#define CATEGORY_SEARCH_URL "https://dapi.kakao.com/v2/local/search/category.json?category_group_code=%s"
#define PLACES_SEARCH_URL "https://dapi.kakao.com/v2/local/search/category.json?category_group_code=%25%20&y=%s&x=%s&radius=50" //y = latitude, x = longitude, 검색반경은 임의로 50미터 설정

#define KAKAO_REST_API_KEY "448edda50741aed5926540433c13d421" /*테스트 앱용 REST API 키*/


USTRUCT(BlueprintType)
struct RESTAPI_API FKakaoPlace
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString AddressName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString RoadAddressName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString CategoryGroupCode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString CategoryGroupName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString CategoryName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString Distance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString PhoneNumber;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString PlaceName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString PlaceUrl;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString Latitude;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place")
	FString Longitude;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLocationSearchingPlacesDelegate, const TArray<FKakaoPlace>&, Places);

UCLASS(Abstract) 
class RESTAPI_API AKakaoLocalAPIActor : public AActor
{
	GENERATED_BODY()
public:	
	AKakaoLocalAPIActor();

	UFUNCTION(BlueprintCallable, Category = "KakaoAPI")
	void RequestSearchingAddress(const FString& Address);
	UFUNCTION(BlueprintCallable, Category = "KakaoAPI")
	void RequestSearchingKeyword(const FString& Keyword);
	UFUNCTION(BlueprintCallable, Category = "KakaoAPI")
	void RequestSearchingLocation(const FString& Latitude, const FString& Longitude);

	
private:
	void OnSearchingAddressReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnSearchingKeywordReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnSearchingLocationReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	UPROPERTY(BlueprintAssignable, Category = "KakaoAPI")
	FLocationSearchingPlacesDelegate LocationSearchingPlacesDelegate;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "KakaoAPI", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "KakaoAPI", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "KakaoAPI", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place", meta = (AllowPrivateAccess = "true"))
	FString Latitude;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place", meta = (AllowPrivateAccess = "true"))
	FString Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KakaoAPI | Place", meta = (AllowPrivateAccess = "true"))
	float CollsionBoxScale = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "KakaoAPI | Place")
	TArray<FKakaoPlace> Places;

	const int BOX_COLLISION_EXTENT = 50;
	const float BOX_TO_FLAT_SCALER = 0.01f;
public:
	void OnConstruction(const FTransform& Transform) override;


	void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
