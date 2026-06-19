#include <variant>
#include <memory>
#include "control_player_local.h"
#include "communication.h"

ControlPlayerLocal::ControlPlayerLocal(std::shared_ptr<CommunicationHub<BackendToFrontendEvent, FrontendToBackendResponse>> input_hub)
	:m_hub(input_hub)
{
}

std::optional<CardChoiceResponse> ControlPlayerLocal::request_card_choice(RequestCardChoice rcc)
{
	auto msg = send_and_listen(rcc);
	if (msg.has_value()) {
		if (const CardChoiceResponse* received_message = std::get_if<CardChoiceResponse>(&msg.value())) {
			CardChoiceResponse back_to_backend = CardChoiceResponse{ received_message->selected_indices };
			return back_to_backend;
		}
	}
	return std::nullopt;
}

std::optional<CardColorChoiceResponse> ControlPlayerLocal::request_color_choice(RequestColorChoice rcc)
{
	auto msg = send_and_listen(rcc);
	if (msg.has_value()) {
		if (const CardColorChoiceResponse* received_message = std::get_if<CardColorChoiceResponse>(&msg.value())) {
			CardColorChoiceResponse back_to_backend = CardColorChoiceResponse{ received_message->color_code };
			return back_to_backend;
		}
	}
	return std::nullopt;
}

std::optional<FrontendToBackendResponse> ControlPlayerLocal::send_and_listen(BackendToFrontendEvent event) {
	m_hub->send_to_frontend(event);
	return m_hub->listen_to_frontend();
}
