#pragma once
#include "control_player.h"
#include <memory>
#include "communication.h"

class ControlPlayerLocal : public ControlPlayer
{
private:
	std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> m_hub;
	std::optional<FrontendToBackendResponse>send_and_listen(BackendToFrontendEvent event);
public:
	ControlPlayerLocal(std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> input_hub);
	std::optional<CardChoiceResponse> request_card_choice(RequestCardChoice rcc) override;
	std::optional<CardColorChoiceResponse>request_color_choice(RequestColorChoice rcc) override;
};

