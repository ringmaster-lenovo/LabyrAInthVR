// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkController.h"
#include "HttpModule.h"
#include "DTO/FinishGameResponseDTO.h"
#include "Interfaces/IHttpResponse.h"
#include "Serializers/FinishGameResponseDeserializer.h"
#include "Serializers/LabyrinthSerializer.h"

DEFINE_LOG_CATEGORY(LabyrAInthVR_Network_Log);

// Sets default values
ANetworkController::ANetworkController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANetworkController::BeginPlay()
{
	Super::BeginPlay();
	
	// Get the config directory path
	FString ConfigDir = FPaths::ProjectConfigDir();

	// Get the complete path to the NetworkController.ini
	FString NetworkConfigFilePath = FPaths::Combine(*ConfigDir, TEXT("NetworkController.ini"));
	
	NetworkConfigFilePath = FConfigCacheIni::NormalizeConfigIniPath(NetworkConfigFilePath);

	if (GConfig->GetString(TEXT("BackendSettings"), TEXT("BaseUrl"), BaseURL, NetworkConfigFilePath))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Url from the config file: %s"), *BaseURL);
	}
	else
	{
		UE_LOG(LabyrAInthVR_Network_Log, Warning, TEXT("Could not take the Url from config file. Using default value: %s"), *BaseURL);	
	}
	FString URL = BaseURL;
	LabyrinthURL = URL.Append(LabyrinthEndPoint);
	URL = BaseURL;
	LeaderboardUrl = URL.Append(LeaderboardEndPoint);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ANetworkController::GetRequest(FString Url, FString Method, FString* ObjectDTO)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	// Create an http request. The request will execute asynchronously, and call us back on the Lambda below
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = HttpModule.CreateRequest();

	// Set the http URL
	pRequest->SetURL(Url);
	
	// This is where we set the HTTP method (GET, POST, etc)
	if (!Method.Equals(TEXT("POST")) && !Method.Equals(TEXT("GET")))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Warning, TEXT("Incorrect Request Method: setting it to POST"));
		pRequest->SetVerb(TEXT("POST"));	
	}
	else
	{
		UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Setting method to: %s"), *Method);
		pRequest->SetVerb(Method);
	}
	if (Method.Equals(TEXT("POST")) && (ObjectDTO != nullptr))
	{
		UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Setting payload"));
		pRequest->SetContentAsString(*ObjectDTO);
		// Set the Header for a json content-type
		pRequest->SetHeader("Content-Type", TEXT("application/json"));
	}

	return pRequest;
}

void ANetworkController::GetLabyrinthFromBE(ULabyrinthRequestDTO* LabyrinthRequestDTO, ULabyrinthDTO* LabyrinthDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for input labyrinth"));
	FString JsonString = LabyrinthSerializer::SerializeLabyrinth(LabyrinthRequestDTO);

	FString Method = "POST";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LabyrinthURL, Method, &JsonString);
	
	// Set the callback, which will execute when the HTTP call is complete
	pRequest->OnProcessRequestComplete().BindLambda(
		// Here, we "capture" the 'this' pointer (the "&"), so our lambda can call this
		// class's methods in the callback.
		[this, LabyrinthDTO](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Connection SUCCESSFULL."));
				// We should have a JSON response - attempt to process it.
				// Validate http called us back on the Game Thread...
				FString ResponseContent = pResponse->GetContentAsString();
				if (LabyrinthSerializer::DeSerializeLabyrinth(ResponseContent, LabyrinthDTO))
				{
					OnLabyrinthReceived.Broadcast();
				}
				else
				{
					OnNetworkError.Broadcast();
					UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Cannot Deserialize Json Response"));
				}
			} 
			else
			{
			   switch (pRequest->GetStatus())
				{
					case EHttpRequestStatus::Failed_ConnectionError:
					{
			   			UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
						break;
					}
					default:
					{
						UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Request failed. "));
						break;
					}
				}
				OnNetworkError.Broadcast();
			  }
		});
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Starting the request for the labyrinth."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}

void ANetworkController::FinishGame(UFinishGameRequestDTO* FinishGameRequestDTO, UFinishGameResponseDTO* FinishGameResponseDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for FinishGameRequest"));
	
	TSharedPtr<FJsonObject> FinishGameDTOJson = MakeShareable(new FJsonObject);
	FinishGameDTOJson->SetStringField("username", FinishGameRequestDTO->Username);
	FinishGameDTOJson->SetNumberField("time", FinishGameRequestDTO->Time);
	FinishGameDTOJson->SetNumberField("level", FinishGameRequestDTO->Level);
	
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(FinishGameDTOJson.ToSharedRef(), JsonWriter);

	FString Method = "POST";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LeaderboardUrl, Method, &JsonString);
	
	pRequest->OnProcessRequestComplete().BindLambda(
		[this, FinishGameResponseDTO](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("FinishGameDTO sent succesfully."));

				if (FinishGameDeserializer::DeSerializeFinishGameResponse(pResponse->GetContentAsString(), FinishGameResponseDTO))
				{
					// OnFinishGameResponseReceived.Broadcast();
				}
				else
				{
					// OnFinishGameError.Broadcast();
				}
			} 
			else
			{
			   switch (pRequest->GetStatus())
				{
					case EHttpRequestStatus::Failed_ConnectionError:
					{
			   			UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("FinishGame: Connection failed."));
						break;
					}
					default:
					{
						UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("FinishGameRequest failed. "));
						break;
					}
				}
				// OnFinishGameError.Broadcast();
			}
		});
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Starting the request for the FinishGame."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}

void ANetworkController::GetAllLeaderboards(ULeaderBoardDTO* LeaderBoardDTO)
{
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Preparing Request for GetAllLeaderboards"));

	FString Method = "GET";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = GetRequest(LeaderboardUrl, Method, nullptr);
	
	pRequest->OnProcessRequestComplete().BindLambda(
		[this, LeaderBoardDTO](
			FHttpRequestPtr pRequest,
			FHttpResponsePtr pResponse,
			bool connectedSuccessfully) mutable
			{
			if (connectedSuccessfully)
			{
				if (DeserializeAllLeaderBoards(pResponse->GetContentAsString(), LeaderBoardDTO))
				{
					// TODO: implementare il deserialize e la gestione degli errori della leaderboard completa
					UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("All Leaderboards retrieved succesfully."));
				}
				else
				{
					
				}
			} 
			else {
			   switch (pRequest->GetStatus()) {
					case EHttpRequestStatus::Failed_ConnectionError:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("Connection failed."));
							break;
						}
					default:
						{
							UE_LOG(LabyrAInthVR_Network_Log, Error, TEXT("FinishGameRequest failed. "));
							break;
						}
					   }
			   }
		});
	UE_LOG(LabyrAInthVR_Network_Log, Display, TEXT("Starting the request for the GetAllLeaderBoards."));
	// Finally, submit the request for processing
	pRequest->ProcessRequest();
}

bool ANetworkController::DeserializeAllLeaderBoards(FString AllLeaderBoardsString, ULeaderBoardDTO* LeaderBoardDTO)
{
	// TODO: capire come implementare questo, capire come arriva la risposta di tutte le leaderboard
	return true;
}