#pragma once
#include <condition_variable>
#include <queue>
#include <mutex>
#include <variant>
#include "card.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <string>
#include <optional>
#include "card_color.h"

struct EventText {
    LocalizationKey key;
};

struct EventSkip {
    int player_id_1;
    int player_id_2;
};

struct EventColorChoosen {
    int player_id;
    CardColor color;
};

struct EventDraw {
    int player_id;
    int how_many;
};

struct EventReverse {
    int player_id;
};

using Events = std::variant<
    EventText,
    EventSkip,
    EventColorChoosen,
    EventDraw,
    EventReverse
>;

struct PlayerState {
    int player_id;
    std::string player_name;
    int player_card_count;
};

struct UpdatePlayerState {
    int player_id;
    int player_card_count;
};

struct LocalPlayerCards {
    int player_id;
    std::vector<Card> cards;
};

struct GameStarted {
    std::vector<PlayerState> player_states;
    Card starting_card;
};

struct GameLog {
    std::string message;
};

struct TurnUpdate {
    int current_turn_index;
    int cards_placed;
    int in_progress_draw_card_count;
    Events event;
    Card last_card_placed;
    std::vector<UpdatePlayerState> update_card_counts;
    std::vector<LocalPlayerCards> updated_local_player_cards;
};

struct RequestCardChoice {
    int player_id;
    std::vector<int> player_cards_id;
};

struct RequestColorChoice {
    int player_id;
};

struct GameOver {
    int winner_player_id;
};

using BackendToFrontendEvent = std::variant<
    GameStarted,
    GameLog,
    TurnUpdate,
    RequestCardChoice,
    RequestColorChoice,
    GameOver
>;




struct MenuAction {
    int choice_id;
};

struct CardChoiceResponse {
    int selected_indices;
};

struct CardColorChoiceResponse {
    CardColor color_code;
};

using FrontendToBackendResponse = std::variant<
    MenuAction,
    CardChoiceResponse,
    CardColorChoiceResponse
>;
template <typename T>
class CommunicationChannel
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_is_closed = false;
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(value));
        m_cv.notify_one();
    }
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return !m_queue.empty() || m_is_closed; });
        if (m_is_closed) {
            return std::nullopt;
        }
        T consumed = std::move(m_queue.front());
        m_queue.pop();

        return consumed;
    }
    bool has_message() {
        if (m_queue.empty()) {
            return false;
        }
        return true;
    }
    void close() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_is_closed = true;
        }
        m_cv.notify_all();
    }
};


template <typename T, typename U>
class CommunicationHub
{
private:
    CommunicationChannel<T> m_frontend_channel;
    CommunicationChannel<U> m_backend_channel;

    //T backend_to_frontend;
    //U frontend_to_backend;
public:
    void send_to_frontend(T value) {
        m_frontend_channel.push(std::move(value));
    }
    std::optional<T> listen_to_backend() {
        return m_frontend_channel.pop();
    }
    bool backend_has_message() {
        return m_frontend_channel.has_message();
    }
    void send_to_backend(U value) {
        m_backend_channel.push(std::move(value));
    }
    std::optional<U> listen_to_frontend() {
        return m_backend_channel.pop();
    }
    bool frontend_has_message() {
        return m_backend_channel.has_message();
    }
    void close_channels() {
        m_frontend_channel.close();
        m_backend_channel.close();
    }
};